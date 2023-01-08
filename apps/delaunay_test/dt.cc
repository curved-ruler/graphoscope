
#include <vector>
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



class plt_scene : public scene
{
    private:
        std::string target_dir;
        
        float minmax = 10.0f;
        int N = 30;
        std::vector<cr::vec2> pts;
        std::vector<int> tris;
        
        cr::mesh_ux del;
        cr::rrr_buffers del_gpu;
        
        gsgl::r_nshaded* rrr;
        gsgl::r_vshaded* rrr_v;
        
        cr::material col;
        
        float scale;
        float axis;
        float rotation;
        bool  rotdir;
        float xd, yd;
        
        void init_halo()
        {
            cr::fdice dice(120, -minmax, minmax);
            for (int i=0 ; i<N ; ++i)
            {
                cr::vec2 p;
                p.x = dice.next();
                p.y = dice.next();
                pts.push_back(p);
            }
            for (size_t i=0 ; i<pts.size() ; ++i)
            {
                cr::vec3 a( pts[i].x, pts[i].y, 0.0f );
                
                col.albedo.x = 1.0f;
                col.albedo.y = 0.0f;
                col.albedo.z = 0.0f;
                del.add_pnt(a,col);
            }
            
            cr::delaunay(pts, tris);
            
            for (size_t i=0 ; i<tris.size()/3 ; ++i)
            {
                cr::vec3 a( pts[tris[i*3]].x,   pts[tris[i*3]].y,   0.0f );
                cr::vec3 b( pts[tris[i*3+1]].x, pts[tris[i*3+1]].y, 0.0f );
                cr::vec3 c( pts[tris[i*3+2]].x, pts[tris[i*3+2]].y, 0.0f );
                
                col.albedo.x = 1.0f;
                col.albedo.y = 1.0f;
                col.albedo.z = 1.0f;
                del.add_tri(a,b,c,col);
                
                col.albedo.x = 0.0f;
                col.albedo.y = 1.0f;
                col.albedo.z = 0.0f;
                del.add_lin(a,b,col);
                del.add_lin(b,c,col);
                del.add_lin(c,a,col);
            }
            
            gsgl::MUXtoGPU(del, del_gpu);
        }
        
    public:
        plt_scene(cr::scripter& _conf) : scene(_conf)
        {
            _conf.getvalue("folders.target", target_dir);
            
            cr::camera* cam0 = new cr::camera();
            cam0->near = 10.0f;
            cam0->far = 5000.0f;
            cam0->focal = 100.0f;
            cam0->pos  = cr::vec3(0.0f, 0.0f,  100.0f);
            cam0->look = cr::vec3(0.0f, 0.0f, -1.0f);
            cam0->up   = cr::vec3(0.0f, 1.0f,  0.0f);
            cam0->oheight2 = 2.0f;
            cam0->pheight2 = 2.0f;
            cam0->aspect = 1.0f;
    
            cameras.push_back(cam0);
            
            rrr = new gsgl::r_nshaded();
            rrr->setup(&rmode);
            rrr_v = new gsgl::r_vshaded();
            rrr_v->setup(&rmode);
            
            
            rmode.back[0] = 0.0f;
            rmode.back[1] = 0.0f;
            rmode.back[2] = 0.0f;
            rmode.back[3] = 1.0f;
            //sysconf.getarray("window.backgr", rmode.back, 3);
            
            scale = 1.0f;
            xd = 0.0f;
            yd = 0.0f;
            
            init_halo();
            
            //pan(-1.50f, 0.0f);
        }
        
        virtual ~plt_scene()
        {
            delete rrr;
            delete rrr_v;
        }
        
        virtual void step(float /*dt*/) {}
        
        virtual void save() {}
        
        virtual void init_render() {}
        virtual void pre_render() {}
        
        virtual void render()
        {
            cr::mat4 model = cr::scale_mat(scale);
            
            model *= cr::move_mat(cr::vec3(xd,yd,0));
            //model *= cr::rotx_mat(rotation);
            //model *= cr::roty_mat(axis);
            
            framebuf->use();
            
            rrr->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
            rrr->pre_render();
            
            rmode.camtype = 1;
            rmode.colourmode = 2;
            
            rmode.objtype = 1;
            rrr->render(*(cameras[used_cam]), model, del_gpu);
            
            rmode.objtype = 0;
            rmode.pointsize = 5;
            rrr->render(*(cameras[used_cam]), model, del_gpu);

            framebuf->render();
        }
        
        void pan(float x, float y)
        {
            xd += x;
            yd += y;
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
            
            scale = s2;
            /*
            if (s2 > 0.000001f && s2 < 1000.0f)
            {
                scale = s2;
            }
            */
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
                    //++(rmode.camtype); if (rmode.camtype >= cr::render_mode::cam_n) rmode.camtype=0;
                    break;
                    
                case keys::O :
                    //++(rmode.objtype); if (rmode.objtype >= cr::render_mode::obj_n) rmode.objtype=0;
                    break;
                
                case keys::P :
                    break;
            
                case keys::K :
                    //++(rmode.colourmode); if (rmode.colourmode >= cr::render_mode::col_n) rmode.colourmode=0;
                    break;
                
                case keys::X :
                    pan(0.01f, 0.0f);
                    break;
                    
                case keys::Y :
                    pan(0.0f, 0.01f);
                    break;
                
                default:
                    break;
            }
        }
};

int main ()
{
    cr::scripter ttconf("plane_transform.conf");
    
    global_loop = new glfw_loop(ttconf);
    scene* s = new plt_scene(ttconf);
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
