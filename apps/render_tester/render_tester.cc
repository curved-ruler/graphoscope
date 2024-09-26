
#include <apps/appbase/glfw_loop.hh>
#include <apps/appbase/scene.hh>
#include <crcc/crcc.hh>
#include <gscc/gscc.hh>
#include <gsgl/gsgl.hh>

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>


glfw_loop* global_loop;

void key_callback         (GLFWwindow* /*wnd*/, int key, int scancode, int action, int mods) { global_loop->key(key, scancode, action, mods); }
void scroll_callback      (GLFWwindow* /*wnd*/, double xoffset, double yoffset)              { global_loop->scroll(xoffset, yoffset); }
void cursorpos_callback   (GLFWwindow* /*wnd*/, double xpos, double ypos)                    { global_loop->cursorpos(xpos, ypos); }
void mousebutton_callback (GLFWwindow* /*wnd*/, int button, int action, int mods)            { global_loop->mousebutton(button, action, mods); }



class simple_scene : public scene
{
    private:
        std::string screen_prefix;
        unsigned int scr_counter;
        float max_dt;
        
        cr::mesh_ux* m_cpu;
        cr::rrr_buffers m_gpu;
        
        cr::spectator_cam* spec_cam;
        
        std::string renderengine;
        gscc::r_pathtracer* renderer_pt;
        int objrender;

    private:
        void perspsh(float d) { cameras[used_cam]->persp_shift(d); }
        
    public:
        simple_scene (cr::scripter& conf) : scene(conf)
        {
            /*
            std::string s;
            conf.getvalue("folders.target", s, "");
            screen_prefix = cr::spath(s);
            conf.getvalue("folders.screen_dir", s, "");
            screen_prefix = cr::spath(screen_prefix + s);
            screen_prefix += "gif";
            screen_prefix = cr::spath(screen_prefix);
            conf.getvalue("folders.screen_prefix", s, "rt_");
            screen_prefix += s;
            cr::idice d((unsigned int)time(NULL), 0, 25);
            int cc;
            for (int i=0 ; i<7 ; i++) {
                cc = d.next() + 'a';
                screen_prefix += (char)cc;
            }
            screen_prefix += "_";
            scr_counter = 0;
            */
            max_dt = 2.0f;
            
            cr::camera* cam = new cr::camera();
            cam->near = 0.1f;
            cam->far = 100.0f;
            cam->focal = 1.0f;
            cam->setfov(25.0f * cr::dtor);
            cam->persp_to_ortho();
            cam->aspect = 1.0f;
            cam->pos  = cr::vec3(0.0f, 0.0f, 0.0f);
            cam->look = cr::vec3(1.0f, 0.0f, 0.0f);
            cam->up   = cr::vec3(0.0f, 0.0f, 1.0f);
            cameras.push_back(cam);
            used_cam = 0;
            spec_cam = new cr::spectator_cam(cameras[0]);
            
            conf.getvalue("render.engine", renderengine, "gsgl");
            conf.getvalue("render.frames", rmode.frames, 1);
            conf.getvalue("render.thread_n", rmode.thread_n, 1);
            //std::cout << "PATH_N: " << rmode.path_n << std::endl;
            renderer_pt = new gscc::r_pathtracer();
            renderer_pt->setup(&rmode);
            
            rmode.camtype = 1;
            rmode.objtype = 1; objrender = 1;
            rmode.colourmode = 2;
            rmode.proctype = 0;
            
            cr::renderer* rrr_n = new gsgl::r_nshaded();
            cr::renderer* rrr_v = new gsgl::r_vshaded();
            rrr_n->setup(&rmode);
            rrr_v->setup(&rmode);
            renderers.push_back(rrr_n);
            renderers.push_back(rrr_v);
            
            cr::material m; m.albedo={0,1,0};
            cr::vec3 a = {3, 0, 0};
            cr::vec3 b = {3, 0, 2};
            cr::vec3 c = {3, 2, 0};
            cr::vec3 d = {3, 2, 2};
            cr::vec3 n = cr::cross(b-a, c-a);
            m_cpu = new cr::mesh_ux(2, true, 4, false, 0, false);
            m_cpu->set_tri_n(0, a, b, c, m, n);
            m_cpu->set_tri_n(1, b, c, d, m, n);
            m_cpu->set_lin(0, a, b, m);
            m_cpu->set_lin(1, b, c, m);
            m_cpu->set_lin(2, c, d, m);
            m_cpu->set_lin(3, d, a, m);
            gsgl::MUXtoGPU(*m_cpu, m_gpu);
        }
        virtual ~simple_scene ()
        {
            delete m_cpu;
            gsgl::clearGPU(m_gpu);
            delete spec_cam;
            delete renderer_pt;
        }
        
        virtual void step (float dt) override { spec_cam->tick(dt); }
        virtual void save () override {}
        
        virtual void init_render () override {}
        virtual void pre_render  () override {}
        virtual void render      () override
        {
            framebuf->use();
            
            if (renderengine == "gsgl")
            {
                renderers[used_render]->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
                renderers[used_render]->pre_render();
                renderers[used_render]->render(*(cameras[used_cam]), cr::mat4(), m_gpu);
            }
            else
            {
                renderer_pt->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
                renderer_pt->pre_render();
                renderer_pt->render(*(cameras[used_cam]), cr::mat4(), *m_cpu);
                screen_tex->setdata(renderer_pt->screen);
                /*
                std::stringstream savename("");
                savename << screen_prefix;
                savename << std::setfill('0') << std::setw(3) << scr_counter;
                savename << ".png";
                stbi_write_png(savename.str().c_str(), rmode.screen_w, rmode.screen_h, 3, renderer_pt->screen_char, rmode.screen_w*3);
                ++scr_counter;
                */
            }
            
            framebuf->render();
        }
        
        virtual void keyaction (int key, int action, int mods) override
        {
            switch (key)
            {
                case keys::I :
                    ++(rmode.camtype); if (rmode.camtype >= cr::render_mode::cam_n) rmode.camtype=0;
                    break;
                    
                case keys::O :
                    ++(objrender); if (objrender >= cr::render_mode::obj_n) objrender=0;
                    break;
                    
                case keys::P :
                    ++(rmode.proctype); if (rmode.proctype >= cr::render_mode::pop_n) rmode.proctype=0;
                    break;
                    
                case keys::K :
                    ++(rmode.colourmode); if (rmode.colourmode >= cr::render_mode::col_n) rmode.colourmode=0;
                    break;
                    
                case keys::BUT_1 :
                    //if (action == keys::PRESS) showplane = !showplane;
                    break;
                
                case keys::BUT_2 :
                    if (action == keys::PRESS) perspsh(-0.02f);
                    break;
                
                case keys::BUT_3 :
                    if (action == keys::PRESS) perspsh(0.02f);
                    break;
                
                default:
                    spec_cam->step(key, action, mods);
            }
        }
        virtual void mousedrag (float /*xpos*/, float /*ypos*/, float xdiff, float ydiff, int /*dragbutton*/) override
        {
            if (xdiff > -1000.0f && xdiff < 1000.0f && ydiff > -1000.0f && ydiff < 1000.0f)
            {
                spec_cam->look(-xdiff * 0.4f * cr::dtor, -ydiff * 0.4f * cr::dtor);
            }
        }
        virtual void mousemove (float /*xpos*/, float /*ypos*/, float /*xdiff*/, float /*ydiff*/) override {}
};

int main ()
{
    std::string confstr = cr::read_file("conf.conf");
    cr::scripter ttconf(confstr);
    
    global_loop = new glfw_loop(ttconf);
    scene* nws = new simple_scene(ttconf);
    global_loop->set_scene(nws);
    
    glfwSetKeyCallback        (global_loop->window, key_callback);
    glfwSetScrollCallback     (global_loop->window, scroll_callback);
    glfwSetCursorPosCallback  (global_loop->window, cursorpos_callback);
    glfwSetMouseButtonCallback(global_loop->window, mousebutton_callback);
    
    
    int ret = global_loop->loop();
    global_loop->done();
    
    delete nws;
    delete global_loop;
    return ret;
}
