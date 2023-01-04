
#include <cmath>
#include <iostream>
#include <vector>

#include <apps/appbase/scene.hh>
#include <apps/appbase/glfw_loop.hh>
#include <crcc/crcc.hh>
#include <gsgl/gsgl.hh>

glfw_loop* global_loop;

void key_callback         (GLFWwindow* /*wnd*/, int key, int scancode, int action, int mods) { global_loop->key(key, scancode, action, mods); }
void scroll_callback      (GLFWwindow* /*wnd*/, double xoffset, double yoffset)              { global_loop->scroll(xoffset, yoffset); }
void cursorpos_callback   (GLFWwindow* /*wnd*/, double xpos, double ypos)                    { global_loop->cursorpos(xpos, ypos); }
void mousebutton_callback (GLFWwindow* /*wnd*/, int button, int action, int mods)            { global_loop->mousebutton(button, action, mods); }



namespace gsgl {


class r_matrix_p2 : public cr::renderer
{
    private:
        cr::precompiler pc;
        renderprog* vf;
        int status;
        cr::render_mode* settings;
        
    public:
        r_matrix_p2() { vf = nullptr; }
        virtual ~r_matrix_p2() { delete vf; }
        
        virtual void setup (cr::render_mode* rm) override
        {
            settings = rm;
            
            std::string vs = pc.process(settings->shader_dir, "matrix.glsl.v");
            std::string fs = pc.process(settings->shader_dir, "simple.glsl.f");
            
            vf = new renderprog(vs, fs);
            status = vf->get_state();
        }
        virtual void init_render (unsigned int w, unsigned int h) override
        {
            glUseProgram(vf->get_progid());
            
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_CULL_FACE);
            
            glPointSize(settings->pointsize);
            
            glViewport(0, 0, w, h);
        }
        virtual void pre_render () override
        {
            GLfloat depth = 1.0f;
            glClearBufferfv(GL_COLOR, 0, settings->back);
            glClearBufferfv(GL_DEPTH, 0, &depth);
        }
        virtual void render (const cr::camera& cam, const cr::mat4& modeltr, const cr::rrr_buffers& bufs) override
        {
            if (settings->colourmode & 1) {
                glDisable(GL_DEPTH_TEST);
                glEnable(GL_BLEND);
            } else {
                glEnable(GL_DEPTH_TEST);
                glDisable(GL_BLEND);
            }
            
            //glBindVertexArray(mesh->vao);
            
            glBindBuffer(GL_ARRAY_BUFFER, bufs.pnt_buf);
            
            glEnableVertexAttribArray(obj::POS_BUF_ID);
            glEnableVertexAttribArray(obj::COL_BUF_ID);
            
            glVertexAttribPointer(obj::POS_BUF_ID, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), BUFFER_OFFSET(NULL));
            glVertexAttribPointer(obj::COL_BUF_ID, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), BUFFER_OFFSET(3 * sizeof(float)));
            
            cr::mat4 view = cam.view_mat();
            cr::mat4 proj = (settings->shadertype & 0x01) ? cam.persp_mat() : cam.ortho_mat();
            cr::mat4 pvm  = proj * view * modeltr;
            bool succ = vf->set_uniform_m4("pvm", pvm);
            succ &= vf->set_uniform_int("colm", (int)((settings->colourmode & 2) >> 1));
            succ &= vf->set_uniform_v3("base_color", cr::vec3{ settings->col[0], settings->col[1], settings->col[2] });
            if (settings->colourmode & 1) succ &= vf->set_uniform_float("alpha", settings->col[3]);
            else                          succ &= vf->set_uniform_float("alpha", 1.0);
            
            //std::cout << mesh->linn << std::endl;
            glDrawArrays(GL_POINTS, 0, bufs.pntn);
            
            glDisableVertexAttribArray(obj::POS_BUF_ID);
            glDisableVertexAttribArray(obj::COL_BUF_ID);
        }
        virtual void render (const cr::camera& /*cam*/, const cr::mat4& /*modeltr*/, const cr::mesh_ux& /*mesh*/) override {}
        virtual void render (const cr::camera& /*cam*/, const cr::mat4& /*modeltr*/, const cr::mesh_ix& /*mesh*/) override {}
};


}


class c_scene : public scene
{
    private:
        std::string target_dir;
        
        cr::mesh_ux mesh;
        cr::rrr_buffers meshbuffers;
        int base_n;
        int base_circ;
        float base_r;
        float* bases;
        
        cr::mesh_ux hlp;
        cr::rrr_buffers helperbuf;
        bool show_helper;
        cr::material col;
        
        int sweep;

        float tri_size;
        
        int base_last;
        int base_last2;
        cr::vec3 v;
        float sx, sy, sz;
        int diff;
        cr::idice* dice;
        cr::idice* ddice;
        cr::fdice* sdice;
        
        cr::mat4 model;
        float scale;
        float axis;
        float rotation;
        bool  rotdir;
        float panx, pany;
        
    public:
        c_scene(cr::scripter& _conf) : scene(_conf)
        {
            _conf.getvalue("folders.target", target_dir);
            
            show_helper = false;
            
            
            float camp;
            _conf.getvalue("window.camp", camp, 100.0f);
            cr::camera* cam0 = new cr::camera();
            cam0->near = 1.0f;
            cam0->far = 1000.0f;
            cam0->focal = camp;
            cam0->pos  = cr::vec3(0.0f, 0.0f,  camp);
            cam0->look = cr::vec3(0.0f, 0.0f, -1.0f);
            cam0->up   = cr::vec3(0.0f, 1.0f,  0.0f);
            cam0->oheight2 = 20.0f;
            cam0->pheight2 = 20.0f;
            cam0->aspect = 1.0f;

            cameras.push_back(cam0);
            
            cr::renderer* renderer0 = new gsgl::r_matrix_p2();
            renderer0->setup(&rmode);
            renderers.push_back(renderer0);
            
            
            rmode.back[0] = 0.0f;
            rmode.back[1] = 0.0f;
            rmode.back[2] = 0.0f;
            rmode.back[3] = 1.0f;
            sysconf.getarray("window.backgr", rmode.back, 3);
            
            col.albedo = {1.0f, 1.0f, 1.0f};
            float c[3];
            sysconf.getarray("window.col", c, 3);
            col.albedo.x = c[0];
            col.albedo.y = c[1];
            col.albedo.z = c[2];
            _conf.getvalue("window.alpha", rmode.alpha, 0.1f);
            
            scale = 1.0f;
            
            //rmode.pointcnt = 1;
            //rmode.pointcoords[0] = 0.0f;
            //rmode.pointcoords[1] = 0.0f;
            
            _conf.getvalue("data.pnt_size", rmode.pointsize, 3.0f);
            _conf.getvalue("data.tri_size", tri_size, 1.0f);
            
            
            _conf.getvalue("data.sweep", sweep, 1000);
            
            _conf.getvalue("data.sx", sx, 0.5f);
            _conf.getvalue("data.sy", sy, 0.5f);
            _conf.getvalue("data.sz", sz, 0.5f);
            
            base_last = 0;
            _conf.getvalue("data.base_n",    base_n,     5);
            _conf.getvalue("data.base_circ", base_circ,  1);
            _conf.getvalue("data.base_r",    base_r,    50.0f);
            _conf.getvalue("data.diff",      diff,       2);
            bases = new float[base_n*3];
            dice  = new cr::idice(120, 0, base_n-1);
            sdice = new cr::fdice(121, 0.45f, 0.55f);
            ddice = new cr::idice(121, -diff, diff);
            
            if (base_circ == 1)
            {
                float dtt = 2.0f * cr::pi / (float)base_n;
                float tt  = (base_n%2 == 0) ? dtt/2.0f : 0.0f;
                for (int i=0 ; i<base_n ; ++i)
                {
                    bases[i*3]     = base_r*std::sin(tt);
                    bases[i*3 + 1] = base_r*std::cos(tt);
                    bases[i*3 + 2] = 0.0f;
                    tt += dtt;
                }
            }
            else if (base_circ == 2)
            {
                bases[0] = 0.0f;
                bases[1] = 0.0f;
                bases[2] = 0.0f;
                float dtt = 2.0f * cr::pi / (float)(base_n-1);
                float tt  = (base_n%2 == 0) ? 0.0f : dtt/2.0f;
                for (int i=1 ; i<base_n ; ++i)
                {
                    bases[i*3]     = base_r*std::sin(tt);
                    bases[i*3 + 1] = base_r*std::cos(tt);
                    bases[i*3 + 2] = 0.0f;
                    tt += dtt;
                }
            }
            else if (base_circ == 3)
            {
                float dtt = 4.0f * cr::pi / (float)base_n;
                float tt  = (base_n%2 == 0) ? dtt/2.0f : 0.0f;
                for (int i=0 ; i<base_n/2 ; ++i)
                {
                    bases[i*6]     = base_r*std::sin(tt);
                    bases[i*6 + 1] = base_r*std::cos(tt);
                    bases[i*6 + 2] = 0.0f;
                    tt += dtt;
                }
                for (int i=0 ; i<base_n/2 ; ++i)
                {
                    int j = i+1;
                    if (j >= base_n/2) j = 0;
                    bases[i*6 + 3] = (bases[i*6]     - bases[j*6]) / 2.0f;
                    bases[i*6 + 4] = (bases[i*6 + 1] - bases[j*6 + 1]) / 2.0f;
                    bases[i*6 + 5] = (bases[i*6 + 2] - bases[j*6 + 2]) / 2.0f;
                }
            }
            else if (base_circ == 4)
            {
                int i = 1;
                while (i*i <= base_n) { ++i; }
                --i;
                base_n = i*i;
                for (int x=0 ; x<i ; ++x)
                for (int y=0 ; y<i ; ++y)
                {
                    bases[(y*i + x)*3]     = float(i-1) / -2.0f + float(x);
                    bases[(y*i + x)*3 + 1] = float(i-1) / -2.0f + float(y);
                    bases[(y*i + x)*3 + 2] = 0.0f;
                }
            }
            else
            {
                sysconf.getarray("data.bases", bases, base_n*3);
            }
            v.x = bases[0] * 3.0f;
            v.y = bases[1] * 3.0f;
            v.z = bases[2];
            
            base_last2 = -1;
            base_last  = 0;
            
            
            
            chaos_step(sweep);
            
            calc_matrices();
            
            nextColMode();
        }
        
        virtual ~c_scene()
        {
            gsgl::clearGPU(meshbuffers);
            gsgl::clearGPU(helperbuf);
            delete[] bases;
            delete dice;
            delete ddice;
            delete sdice;
        }
        
        virtual void step(float /*dt*/) {}
        
        virtual void save() {}
        
        void calc_matrices()
        {
            model = cr::scale_mat(scale);
            model *= cr::rotx_mat(rotation);
            model *= cr::roty_mat(axis);
            model *= cr::move_mat(cr::vec3(panx, pany, 0.0f));
        }
        
        inline int next_base ()
        {
            int i;
            
            do
            {
                i = dice->next();
            }
            //while (i == base_last || i == base_last2);
            while (i == base_last);
            //while (false);
            //while ((base_last - i == 1) || (base_last - i == 1-base_n));
            //while (base_last == base_last2 && (base_last - i == 1 || base_last - i == -1 || base_last - i == (base_n-1) || base_last - i == (1-base_n)));
            //while (base_last - i == 1 || base_last - i == -1 || base_last - i == (base_n-1) || base_last - i == (1-base_n));
            
            /*
            i = (base_last + ddice->next());
            while (i < 0)       { i+=base_n; }
            while (i >= base_n) { i-=base_n; }
            */
            
            base_last2 = base_last;
            base_last  = i;
            return base_last;
        }
        
        void chaos_step(int s)
        {
            for (int i=0 ; i<s ; ++i)
            {
                int bi = next_base();
                int diffb = std::abs(base_last - base_last2);
                if (diffb > base_n/2) diffb = base_n - diffb;
                float sc = 1.0f - 1.0f/float(diffb);
                //float phi = (1.0f + std::sqrt(5.0f)) / 2.0f;
                //float sc = phi;
                v.x = (bases[bi*3]     - v.x) * sx + v.x;
                v.y = (bases[bi*3 + 1] - v.y) * sy + v.y;
                v.z = (bases[bi*3 + 2] - v.z) * sz + v.z;
                //v.x = (bases[bi*3]     - v.x) * sc + v.x;
                //v.y = (bases[bi*3 + 1] - v.y) * sc + v.y;
                //v.z = (bases[bi*3 + 2] - v.z) * sc + v.z;
                mesh.add_pnt(v, col);
            }
            
            gsgl::clearGPU(meshbuffers);
            gsgl::MUXtoGPU(mesh, meshbuffers);
        }
        
        virtual void init_render() { /*renderers[used_render]->initRender(screen_width, screen_height);*/ }
        virtual void pre_render() { /*renderers[used_render]->preRender();*/ }
        virtual void render()
        {
            framebuf->use();
            
            renderers[used_render]->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
            renderers[used_render]->pre_render();
            renderers[used_render]->render(*(cameras[used_cam]), model, meshbuffers);
            if (show_helper)
            {
                renderers[used_render]->render(*(cameras[used_cam]), model, helperbuf);
            }
            
            if (used_render_2 >= 0)
            {
                renderers[used_render_2]->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
                glEnable(GL_POLYGON_OFFSET_FILL);
                glPolygonOffset(1, 1);
                renderers[used_render_2]->render(*(cameras[used_cam]), cr::mat4(), meshbuffers);
                glDisable(GL_POLYGON_OFFSET_FILL);
                if (show_helper)
                {
                    renderers[used_render_2]->render(*(cameras[used_cam]), model, helperbuf);
                }
            }

            framebuf->render();
        }
        
        void pan(float x, float y)
        {
            panx += x*10.0f;
            pany += y*10.0f;
            calc_matrices();
        }
        
        
        void startrot()
        {
            if ((axis > 90.0f) && (axis < 270.0f)) {
                rotdir = false;
            } else {
                rotdir = true;
            }
        }
        
        virtual void mousescroll (float /*x*/, float y)
        {
            //float sw_base = 1.25f;
            //float sw = (y > 0.0f) ? (sw_base) * y : (-1.0f / sw_base) * y;
            //float s2 = scale * sw;
            float s2 = scale * std::pow(1.25f,y);
            if (s2 > 0.000001f && s2 < 1000.0f)
            {
                scale = s2;
                calc_matrices();
            }
        }

        virtual void mousebutton (int /*key*/, int action, int /*mods*/)
        {
            if (action == keys::PRESS) startrot();
        }
        
        virtual void mousedrag (float /*xp*/, float /*yp*/, float x, float y, int btn)
        {
            if (btn == keys::MOUSE_M)
            {
                if (rotdir)
                {
                    rotation += y;
                }
                else
                {
                    rotation -= y;
                }
                axis -= x;
        
                // Ensure [0,360]
                axis = axis - std::floor(axis/360.0f)*360.0f;
                rotation = rotation - std::floor(rotation/360.0f)*360.0f;
                
                calc_matrices();
            }
            else if (btn == keys::MOUSE_L)
            {
                pan(-0.02f*x / scale, -0.02f*y / scale);
                gsgl::updateGPU(mesh, meshbuffers);
            }
        }
        
        virtual void keyaction (int key, int action, int /*mods*/)
        {
            if (action == keys::RELEASE) return;
            
            switch (key)
            {
                case keys::I :
                    nextCamMode();
                    break;
                
                case keys::O :
                    nextObjMode();
                    break;
                
                case keys::P :
                    break;
            
                case keys::K :
                    nextColMode();
                    break;

                case keys::BUT_1:
                    show_helper = ! show_helper;
                    break;
                
                case keys::S :
                    break;
                
                case keys::T :
                    break;
                
                
                
                default:
                    break;
            }
        }
};

int main ()
{
    cr::scripter ttconf("chaos_game.conf");
    
    global_loop = new glfw_loop(ttconf);
    scene* s = new c_scene(ttconf);
    global_loop->set_scene(s);
    
    
    glfwSetKeyCallback        (global_loop->window, key_callback);
    glfwSetScrollCallback     (global_loop->window, scroll_callback);
    glfwSetCursorPosCallback  (global_loop->window, cursorpos_callback);
    glfwSetMouseButtonCallback(global_loop->window, mousebutton_callback);
    
    
    
    int ret = global_loop->loop();
    global_loop->done();
    
    delete global_loop;
    delete s;
    return ret;
}
