
#include <cmath>
#include <cstring>
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



class mt_scene : public scene
{
    private:
        std::string target_dir;
        std::string mesh_file;
        
        cr::mesh_ux* mux;
        cr::mesh_ix* mix;
        cr::rrr_buffers mux_gpu;
        cr::rrr_buffers mix_gpu;
        
        gsgl::r_nshaded* rrr_n;
        gsgl::r_vshaded* rrr_v;
        int objrender;
        
        float panx, pany;
        float scale;
        float axis;
        float rotation;
        bool  rotdir;
        
        cr::mat4 modelmat;
        
    public:
        mt_scene(cr::scripter& _conf) : scene(_conf)
        {
            _conf.getvalue("folders.target", target_dir);
            target_dir = cr::spath(target_dir);
            _conf.getvalue("data.mesh", mesh_file);
            
            cr::camera* cam0 = new cr::camera();
            cam0->near = 1.0f;
            cam0->far = 500.0f;
            cam0->focal = 10.0f;
            cam0->pos  = cr::vec3(0.0f, 0.0f,  40.0f);
            cam0->look = cr::vec3(0.0f, 0.0f, -1.0f);
            cam0->up   = cr::vec3(0.0f, 1.0f,  0.0f);
            cam0->oheight2 = 2.0f;
            cam0->pheight2 = 2.0f;
            cam0->aspect = 1.0f;
    
            cameras.push_back(cam0);
            
            
            
            rrr_n = new gsgl::r_nshaded();
            rrr_v = new gsgl::r_vshaded();
            rrr_n->setup(&rmode);
            rrr_v->setup(&rmode);
            renderers.push_back(rrr_n);
            renderers.push_back(rrr_v);
            
            
            rmode.back[0] = 0.0f;
            rmode.back[1] = 0.0f;
            rmode.back[2] = 0.0f;
            rmode.back[3] = 1.0f;
            sysconf.getarray("window.backgr", rmode.back, 3);
            
            //col.albedo = {0.0f, 0.0f, 0.0f};
            //float c[3];
            //sysconf.getarray("window.col", c, 3);
            //col.albedo.x = c[0];
            //col.albedo.y = c[1];
            //col.albedo.z = c[2];
            
            scale = 1.0f;
            axis  = 0.0f;
            rotation = 0.0f;
            
            rmode.camtype = 1;
            objrender = 1;
            rmode.colourmode = 2;
            pan(-1.50f, 0.0f);
            
            mix = nullptr;
            mux = nullptr;
            
            //mix = cr::bm1_to_meshix(target_dir + mesh_file);
            //gsgl::MIXtoGPU(*mix, mix_gpu);
            mux = cr::bm1_to_meshux(target_dir + mesh_file);
            gsgl::MUXtoGPU(*mux, mux_gpu);
        }
        
        virtual ~mt_scene()
        {
            delete mix;
            delete mux;
            gsgl::clearGPU(mix_gpu);
            gsgl::clearGPU(mux_gpu);
        }
        
        virtual void step(float /*dt*/) {}
        
        virtual void save() {}
        
        virtual void init_render() {}
        virtual void pre_render() {}
        virtual void render()
        {
            calc_matrices();
            
            framebuf->use();
            
            rmode.objtype = (objrender < 3) ? objrender : objrender-3;
            used_render   = (rmode.objtype < 2) ? 0 : 1;
            used_render_2 = (objrender < 3) ? -1 : 1;
            
            renderers[used_render]->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
            renderers[used_render]->pre_render();
            renderers[used_render]->render(*cameras[used_cam], modelmat, mux_gpu);
    
            if (used_render_2 >= 0)
            {
                rmode.objtype = 2;
                
                renderers[used_render_2]->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
                glEnable(GL_POLYGON_OFFSET_FILL);
                glPolygonOffset(1.0f, 1.0f);
                renderers[used_render_2]->render(*cameras[used_cam], modelmat, mux_gpu);
                glDisable(GL_POLYGON_OFFSET_FILL);
            }
            
            framebuf->render();
        }
        
        void pan(float x, float y)
        {
            panx += x;
            pany += y;
        }
        
        void calc_matrices ()
        {
            modelmat.identity();
            modelmat *= cr::scale_mat(scale);
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
            float s2 = scale * std::pow(1.25f,y);
            
            scale = s2;
            
            //std::cout << "scroll " << y << " - scale " << scale << std::endl;
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
            }
            else if (btn == keys::MOUSE_L)
            {
                pan(-0.02f*x, -0.02f*y);
            }
        }
        
        virtual void keyaction (int key, int action, int /*mods*/)
        {
            //int nump   = 50;
            //float dpan = -3.0f / nump;
            
            if (action == keys::RELEASE) return;
            switch (key)
            {
                case keys::I :
                    ++(rmode.camtype); if (rmode.camtype >= cr::render_mode::cam_n) rmode.camtype=0;
                    break;
                    
                case keys::O :
                    ++(objrender); if (objrender >= cr::render_mode::obj_n) objrender=0;
                    break;
                
                case keys::P :
                    break;
            
                case keys::K :
                    ++(rmode.colourmode); if (rmode.colourmode >= cr::render_mode::col_n) rmode.colourmode=0;
                    break;
                
                case keys::W :
                    /*
                    for (int i=0 ; i<nump ; ++i)
                    {
                        pan(dpan, 0.0f);
                        transform();
                        r::updateGPU(gridgeom_tr, halo);
                        global_loop->one_loop();
                        global_loop->key(GLFW_KEY_F8, 0, GLFW_PRESS, 0);
                        
                    }
                    */
                    break;
                
                case keys::X :
                    pan(0.01f, 0.0f);
                    break;
                    
                case keys::Y :
                    pan(0.0f, 0.01f);
                    break;
                
                case keys::R :
                    //relax_z();
                    //update_gpu();
                    break;
                
                default:
                    break;
            }
        }
};

int main ()
{
    std::string confstr = cr::read_file("model-tester.conf");
    cr::scripter ttconf(confstr);
    
    global_loop = new glfw_loop(ttconf);
    scene* s = new mt_scene(ttconf);
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
