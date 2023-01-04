
#include <cmath>
#include <iostream>

#include <apps/appbase/scene.hh>
#include <apps/appbase/glfw_loop.hh>
#include <crcc/crcc.hh>
#include <gsgl/gsgl.hh>

glfw_loop* global_loop;

void key_callback         (GLFWwindow* /*wnd*/, int key, int scancode, int action, int mods) { global_loop->key(key, scancode, action, mods); }
void scroll_callback      (GLFWwindow* /*wnd*/, double xoffset, double yoffset)              { global_loop->scroll(xoffset, yoffset); }
void cursorpos_callback   (GLFWwindow* /*wnd*/, double xpos, double ypos)                    { global_loop->cursorpos(xpos, ypos); }
void mousebutton_callback (GLFWwindow* /*wnd*/, int button, int action, int mods)            { global_loop->mousebutton(button, action, mods); }


class c_scene : public scene
{
    private:
        std::string target_dir;
        gsgl::r_nshaded* rrr;
        
        cr::mesh_ux mesh;
        cr::rrr_buffers meshbuffers;

        cr::rrr_buffers helperbuf0;
        cr::rrr_buffers helperbuf1;
        cr::rrr_buffers helperbuf2;
        cr::rrr_buffers helperbuf3;
        cr::mat4 model;
        cr::mat4 model1;
        cr::mat4 model2;
        cr::mat4 model3;
        bool show_helper;
        cr::material col;
        
        float time, dt, dtp;
        int sweep;

        float tri_size;
        
        float w0, w1, w2;
        cr::vec3 r1, r2, rr1, rr2, rpen;

        //cr::vec3 a1, a2, p1, p2;
        
        int global_mat;
        float scale;
        float axis;
        float rotation;
        bool  rotdir;
        float panx, pany;
        
    public:
        c_scene(cr::scripter& _conf) : scene(_conf)
        {
            _conf.getvalue("folders.target", target_dir);
            _conf.getvalue("window.global_mat", global_mat, 0);

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
            
            
            rrr = new gsgl::r_nshaded();
            rrr->setup(&rmode);
            
            rmode.back[0] = 0.0f;
            rmode.back[1] = 0.0f;
            rmode.back[2] = 0.0f;
            rmode.back[3] = 1.0f;
            sysconf.getarray("window.backgr", rmode.back, 3);
            
            col.albedo = {0.0f, 0.0f, 0.0f};
            float c[3];
            sysconf.getarray("window.col", c, 3);
            col.albedo.x = c[0];
            col.albedo.y = c[1];
            col.albedo.z = c[2];
            _conf.getvalue("window.alpha", rmode.alpha, 0.1f);
            
            scale    = 1.0f;
            rotation = 0.0f;
            axis     = 0.0f;
            panx     = 0.0f;
            pany     = 0.0f;
            time     = 0.0f;
            
            _conf.getvalue("data.dtp", dtp, 20);
            dt   = cr::pi / dtp;
            //std::cout << "T " << time << std::endl;
            
            meshbuffers.lin_buf = 0;
            
            rmode.pointcnt = 1;
            rmode.pointcoords[0] = 0.0f;
            rmode.pointcoords[1] = 0.0f;
            
            _conf.getvalue("data.pt_size", rmode.pointsize, 3.0f);
            _conf.getvalue("data.tri_size", tri_size, 1.0f);
            
            
            _conf.getvalue("data.sweep", sweep, 10);
            
            _conf.getvalue("data.w0",   w0,   0.1f);
            _conf.getvalue("data.r1x",  r1.x, 1.0f);
            _conf.getvalue("data.r1y",  r1.y, 1.0f);
            r1.z = 0.0f;
            _conf.getvalue("data.r2x",  r2.x, 1.0f);
            _conf.getvalue("data.r2y",  r2.y, 1.0f);
            r2.z = 0.0f;
            
            _conf.getvalue("data.w1",   w1,    0.1f);
            _conf.getvalue("data.rr1x", rr1.x, 1.0f);
            _conf.getvalue("data.rr1y", rr1.y, 0.1f);
            rr1.z = 0.0f;
            
            _conf.getvalue("data.w2",   w2,   0.1f);
            _conf.getvalue("data.rr2x", rr2.x, 1.0f);
            _conf.getvalue("data.rr2y", rr2.y, 0.1f);
            rr2.z = 0.0f;

            _conf.getvalue("data.rtx", rpen.x, 1.0f);
            _conf.getvalue("data.rty", rpen.y, 0.1f);
            rpen.z = 0.0f;
            
            int calc_w12;
            _conf.getvalue("data.calc_w12", calc_w12, 0);
            if (calc_w12)
            {
                float w1mul, w2mul;
                _conf.getvalue("data.w1mul", w1mul, 1.0);
                _conf.getvalue("data.w2mul", w2mul, 1.0);
                
                w1 = 0.0f;
                float rr1len = rr1.length();
                if (std::fabs(rr1len) > cr::eps) { w1 = (-w0) * w1mul * ( r1.length() - rr1.length() ) / rr1len; }
                
                w2 = 0.0f;
                float rr2len = rr2.length();
                if (std::fabs(rr2len) > cr::eps) { w2 = (-w0) * w2mul * ( r2.length() - rr2.length() ) / rr2len; }
            }
            
            create_helper();
            show_helper = false;
            
            rmode.camtype = 0;
            rmode.objtype = 1;
            rmode.colourmode = 2;
            
            step_rot();
            gsgl::MUXtoGPU(mesh, meshbuffers);
        }

        void create_helper ()
        {
            cr::material matr; matr.albedo = {0.8f, 0.0f, 0.0f};
            cr::material matg; matg.albedo = {0.0f, 0.8f, 0.0f};
            cr::material matc; matc.albedo = {0.0f, 0.6f, 0.6f};
            cr::mesh_ux m0;
            cr::mesh_ux m1;
            cr::mesh_ux m2;
            cr::mesh_ux m3;

            float zz = 0.1f;
            float cross;
            sysconf.getvalue("data.cross", cross, 1.5f);

            // pen
            m3.add_lin(cr::vec3(0.0f, 0.0f, zz), cr::vec3(rpen.x, rpen.y, zz), matc);
            m3.add_lin(cr::vec3(0.0f, 0.0f, zz), cr::vec3(50.0f, 0.0f, zz), matc);
            m3.add_lin(cr::vec3(rpen.x-cross, rpen.y-cross, zz), cr::vec3(rpen.x+cross, rpen.y+cross, zz), matc);
            m3.add_lin(cr::vec3(rpen.x-cross, rpen.y+cross, zz), cr::vec3(rpen.x+cross, rpen.y-cross, zz), matc);

            // coordinate lines
            float tav = 200.0f;
            m0.add_lin(cr::vec3(-tav,0.0f,zz),cr::vec3(tav,0.0f,zz),matg);
            m0.add_lin(cr::vec3(0.0f,-tav,zz),cr::vec3(0.0f,tav,zz),matg);
            
            // rotating points
            m1.add_lin(cr::vec3(rr1.x-cross, rr1.y-cross,zz), cr::vec3(rr1.x+cross, rr1.y+cross,zz), matr);
            m1.add_lin(cr::vec3(rr1.x-cross, rr1.y+cross,zz), cr::vec3(rr1.x+cross, rr1.y-cross,zz), matr);
            m2.add_lin(cr::vec3(rr2.x-cross, rr2.y-cross,zz), cr::vec3(rr2.x+cross, rr2.y+cross,zz), matr);
            m2.add_lin(cr::vec3(rr2.x-cross, rr2.y+cross,zz), cr::vec3(rr2.x+cross, rr2.y-cross,zz), matr);

            // gear circles
            int nn = 50;
            float tt = 0.0f;
            float dtt = 2.0f * cr::pi / (float)nn;
            float rr1len = std::sqrt(rr1.x*rr1.x + rr1.y*rr1.y);
            float rr2len = std::sqrt(rr2.x*rr2.x + rr2.y*rr2.y);
            for (int i=0 ; i<nn ; ++i)
            {
                m1.add_lin(cr::vec3(rr1len*std::sin(tt),     rr1len*std::cos(tt), zz),
                           cr::vec3(rr1len*std::sin(tt+dtt), rr1len*std::cos(tt+dtt), zz),
                           matr);
                m2.add_lin(cr::vec3(rr2len*std::sin(tt),     rr2len*std::cos(tt), zz),
                           cr::vec3(rr2len*std::sin(tt+dtt), rr2len*std::cos(tt+dtt), zz),
                           matr);
                tt += dtt;
            }

            gsgl::MUXtoGPU(m0, helperbuf0);
            gsgl::MUXtoGPU(m1, helperbuf1);
            gsgl::MUXtoGPU(m2, helperbuf2);
            gsgl::MUXtoGPU(m3, helperbuf3);
        }
        
        virtual ~c_scene()
        {
            delete rrr;
            gsgl::clearGPU(meshbuffers);
            gsgl::clearGPU(helperbuf0);
            gsgl::clearGPU(helperbuf1);
            gsgl::clearGPU(helperbuf2);
            gsgl::clearGPU(helperbuf3);
        }
        
        virtual void step(float /*dt*/) {}
        
        virtual void save() {}
        
        void calc_matrices()
        {
            cr::mat4 mrot = cr::rotz_mat(-w0*time);
            model = cr::scale_mat(scale);
            model *= cr::rotx_mat(rotation);
            model *= cr::roty_mat(axis);
            model *= cr::move_mat(cr::vec3(panx, pany, 0.0f));
            if (global_mat) { model *= mrot; }
            
            cr::mat4 m0  = cr::rotz_mat(w0*time);
            cr::mat4 m1  = cr::move_mat(r1);
            cr::mat4 m1r = cr::rotz_mat(w1*time);
            cr::mat4 m2  = cr::move_mat(r2);
            cr::mat4 m2r = cr::rotz_mat(w2*time);
            
            cr::vec3 pp1 = m0*m1*m1r*rr1;
            cr::vec3 pp2 = m0*m2*m2r*rr2;
            float firt2 = 0.0f;
            float x = pp2.x-pp1.x;
            float y = pp2.y-pp1.y;
            if (std::fabs(x) > cr::eps || std::fabs(y) > cr::eps) { firt2 = std::atan2(y, x); }
            
            cr::mat4 m3  = cr::move_mat(pp1);
            cr::mat4 m3r = cr::rotz_mat(firt2);
            
            model1 = m0*m1*m1r;
            model2 = m0*m2*m2r;
            model3 = m3*m3r;
        }
        
        void step_rot()
        {
            /*
            // t
            cr::vec3 a1(r1*std::sin(w0*time + fi1), r1*std::cos(w0*time + fi1), 0.0f);
            cr::vec3 a2(r2*std::sin(w0*time + fi2), r2*std::cos(w0*time + fi2), 0.0f);
            
            cr::vec3 p1(rr1*std::sin(w1*time + ffi1), rr1*std::cos(w1*time + ffi1), 0.0f);
            cr::vec3 p2(rr2*std::sin(w2*time + ffi2), rr2*std::cos(w2*time + ffi2), 0.0f);

            cr::vec3 pp1 = a1+p1;
            cr::vec3 pp2 = a2+p2;
            float firt2 = std::atan2(pp2.y-pp1.y, pp2.x-pp1.x);
            cr::vec3 v0 = pp1 + cr::vec3(rt*std::sin(firt+firt2), rt*std::cos(firt+firt2), 0.0f);

            // t+dt
            cr::vec3 a1n(r1*std::sin(w0*(time+dt) + fi1), r1*std::cos(w0*(time+dt) + fi1), 0.0f);
            cr::vec3 a2n(r2*std::sin(w0*(time+dt) + fi2), r2*std::cos(w0*(time+dt) + fi2), 0.0f);
            
            cr::vec3 p1n(rr1*std::sin(w1*(time+dt) + ffi1), rr1*std::cos(w1*(time+dt) + ffi1), 0.0f);
            cr::vec3 p2n(rr2*std::sin(w2*(time+dt) + ffi2), rr2*std::cos(w2*(time+dt) + ffi2), 0.0f);

            cr::vec3 pp1n = a1n+p1n;
            cr::vec3 pp2n = a2n+p2n;
            float firt2n = std::atan2(pp2n.y-pp1n.y, pp2n.x-pp1n.x);
            cr::vec3 v1 = pp1n + cr::vec3(rt*std::sin(firt+firt2n), rt*std::cos(firt+firt2n), 0.0f);
            */
            
            calc_matrices();
            cr::vec3 v0 = model3*rpen;
            v0.z = 7.0f * (std::sin((w1)*time) * std::cos((w2)*time));
            time += dt;
            calc_matrices();
            cr::vec3 v1 = model3*rpen;
            v1.z = 7.0f * (std::sin((w1)*time) * std::cos((w2)*time));
            
            mesh.add_lin(v0, v1, col);
            //mesh.add_tri(v10, v10+v11, v20+v21, col);
            //mesh.add_tri(v20+v21, v10+v11, v20, col);
        }
        void update_gpu ()
        {
            gsgl::clearGPU(meshbuffers);
            gsgl::MUXtoGPU(mesh, meshbuffers);
        }
        
        virtual void init_render() {}
        virtual void pre_render() {}
        
        virtual void render()
        {
            framebuf->use();
            
            //glLineWidth(1.5);
            
            //rmode.objbuf = rmode.objtype < 3 ? rmode.objtype : rmode.objtype-3;
            
            rrr->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
            rrr->pre_render();
            rrr->render(*(cameras[used_cam]), model, meshbuffers);
            if (show_helper)
            {
                rrr->render(*(cameras[used_cam]), model, helperbuf0);
                rrr->render(*(cameras[used_cam]), model*model1, helperbuf1);
                rrr->render(*(cameras[used_cam]), model*model2, helperbuf2);
                rrr->render(*(cameras[used_cam]), model*model3, helperbuf3);
            }
            /*
            if (used_render_2 >= 0)
            {
                renderers[used_render_2]->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
                glEnable(GL_POLYGON_OFFSET_FILL);
                glPolygonOffset(1, 1);
                renderers[used_render_2]->render(*(cameras[used_cam]), cr::mat4(), meshbuffers);
                glDisable(GL_POLYGON_OFFSET_FILL);
                if (show_helper)
                {
                    renderers[used_render_2]->render(*(cameras[used_cam]), model, helperbuf0);
                    renderers[used_render_2]->render(*(cameras[used_cam]), model*model1, helperbuf1);
                    renderers[used_render_2]->render(*(cameras[used_cam]), model*model2, helperbuf2);
                    renderers[used_render_2]->render(*(cameras[used_cam]), model*model3, helperbuf3);
                }
            }
            */
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
            if ((axis > 90.0f*cr::dtor) && (axis < 270.0f*cr::dtor)) { rotdir = false; }
            else { rotdir = true; }
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
                    rotation += y*cr::dtor;
                }
                else
                {
                    rotation -= y*cr::dtor;
                }
                axis -= x*cr::dtor;
        
                // Ensure [0,360]
                float full = 360.0f*cr::dtor;
                axis = axis - std::floor(axis/full)*full;
                rotation = rotation - std::floor(rotation/full)*full;
                
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
                    ++(rmode.camtype); if (rmode.camtype >= cr::render_mode::cam_n) rmode.camtype=0;
                    break;
                
                case keys::O :
                    //++(rmode.objtype); if (rmode.objtype >= cr::render_mode::obj_n) rmode.objtype=0;
                    break;
                
                case keys::P :
                    break;
            
                case keys::K :
                    ++(rmode.colourmode); if (rmode.colourmode >= cr::render_mode::col_n) rmode.colourmode=0;
                    break;

                case keys::BUT_1:
                    show_helper = ! show_helper;
                    break;
                
                case keys::S :
                    for (int i=0 ; i<sweep ; ++i)
                    {
                        step_rot();
                    }
                    update_gpu();
                    std::cout << "SWEEP_T " << time << std::endl;
                    break;
                
                case keys::T :
                    step_rot();
                    update_gpu();
                    std::cout << "T " << time << std::endl;
                    break;
                
                
                
                default:
                    break;
            }
        }
};

int main ()
{
    cr::scripter ttconf("cycloid.conf");
    
    global_loop = new glfw_loop(ttconf);
    scene* s = new c_scene(ttconf);
    global_loop->set_scene(s);
    
    
    glfwSetKeyCallback        (global_loop->window, key_callback);
    glfwSetScrollCallback     (global_loop->window, scroll_callback);
    glfwSetCursorPosCallback  (global_loop->window, cursorpos_callback);
    glfwSetMouseButtonCallback(global_loop->window, mousebutton_callback);
    
    
    
    int ret = global_loop->loop();
    global_loop->done();
    
    delete s;
    delete global_loop;
    return ret;
}
