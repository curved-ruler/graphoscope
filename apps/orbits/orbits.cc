
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


class o_scene : public scene
{
    private:
        std::string target_dir;
        
        cr::mesh_ux mesh;
        cr::rrr_buffers meshbuffers;
        
        float time, dt, dtp;
        int nump;
        float mixf;
        int sweep;
        float tri_size;
        
        float r1, w1, rd1, wd1, rf1, wf1;
        float r2, w2, rd2, wd2;
        
        float scale;
        float axis;
        float rotation;
        bool  rotdir;
        float panx, pany;
        
    public:
        o_scene(cr::scripter& _conf) : scene(_conf)
        {
            _conf.getvalue("folders.target", target_dir);
            
            cr::camera* cam0 = new cr::camera();
            cam0->near = 1.0f;
            cam0->far = 1000.0f;
            cam0->focal = 10.0f;
            cam0->pos  = cr::vec3(0.0f, 0.0f,  10.0f);
            cam0->look = cr::vec3(0.0f, 0.0f, -1.0f);
            cam0->up   = cr::vec3(0.0f, 1.0f,  0.0f);
            cam0->oheight2 = 2.0f;
            cam0->pheight2 = 2.0f;
            cam0->aspect = 1.0f;
    
            cameras.push_back(cam0);
            
            cr::renderer* renderer0 = new gsgl::r_matrix_point();
            cr::renderer* renderer1 = new gsgl::r_matrix_line();
            cr::renderer* renderer2 = new gsgl::r_matrix_flat();
            cr::renderer* renderer3 = new gsgl::r_fish1_point();
            cr::renderer* renderer4 = new gsgl::r_fish1_line();
            cr::renderer* renderer5 = new gsgl::r_fish1_flat();
            cr::renderer* renderer6 = new gsgl::r_pp4_point();
            cr::renderer* renderer7 = new gsgl::r_pp4_line();
            cr::renderer* renderer8 = new gsgl::r_pp4_flat();
            renderer0->setup(&rmode);
            renderer1->setup(&rmode);
            renderer2->setup(&rmode);
            renderer3->setup(&rmode);
            renderer4->setup(&rmode);
            renderer5->setup(&rmode);
            renderer6->setup(&rmode);
            renderer7->setup(&rmode);
            renderer8->setup(&rmode);
            renderers.push_back(renderer0);
            renderers.push_back(renderer1);
            renderers.push_back(renderer2);
            renderers.push_back(renderer3);
            renderers.push_back(renderer4);
            renderers.push_back(renderer5);
            renderers.push_back(renderer6);
            renderers.push_back(renderer7);
            renderers.push_back(renderer8);
            
            
            rmode.back[0] = 1.0f;
            rmode.back[1] = 1.0f;
            rmode.back[2] = 1.0f;
            rmode.back[3] = 1.0f;
            
            scale = 1.0f;
            
            _conf.getvalue("data.dtp", dtp, 20);
            
            meshbuffers.lin_buf = 0;
            time = 0.0f;
            dt   = cr::pi / dtp;
            std::cout << "T " << time << std::endl;
            
            rmode.pointcnt = 1;
            rmode.pointcoords[0] = 0.0f;
            rmode.pointcoords[1] = 0.0f;
            
            _conf.getvalue("data.nump", nump, 20);
            _conf.getvalue("data.mixf", mixf, 0.5f);
            _conf.getvalue("data.pt_size", rmode.pointsize, 3.0f);
            _conf.getvalue("data.tri_size", tri_size, 1.0f);
            _conf.getvalue("data.alpha", rmode.alpha, 0.1f);
            
            _conf.getvalue("data.sweep", sweep, 10);
            
            _conf.getvalue("data.r1",  r1,  1.0f);
            _conf.getvalue("data.w1",  w1,  0.1f);
            _conf.getvalue("data.rd1", rd1, 1.0f);
            _conf.getvalue("data.wd1", wd1, 0.1f);
            _conf.getvalue("data.rf1", rf1, 1.0f);
            _conf.getvalue("data.wf1", wf1, 0.1f);
            
            _conf.getvalue("data.r2",  r2,  9.0f);
            _conf.getvalue("data.w2",  w2,  0.1f);
            _conf.getvalue("data.rd2", rd2, 9.0f);
            _conf.getvalue("data.wd2", wd2, 0.1f);
            
            
            
            step_rot();
            
            nextColMode();
            nextColMode();
            nextCamMode();
            nextObjMode();
        }
        
        virtual ~o_scene()
        {
            gsgl::clearGPU(meshbuffers);
        }
        
        virtual void step(float /*dt*/) {}
        
        virtual void save() {}
        
        void step_rot()
        {
            cr::material col; col.albedo = {0.0f, 0.0f, 0.0f};
            
            cr::vec3 v10(r1*std::sin(w1*time),   r1*std::cos(w1*time),   0.0f);
            cr::vec3 v11(rd1*std::sin(wd1*time), rd1*std::cos(wd1*time), 0.0f);
            cr::vec3 v12(rf1*std::sin(wf1*time), rf1*std::cos(wf1*time), 0.0f);
            
            cr::vec3 v20(r2*std::sin(w2*time),   r2*std::cos(w2*time),   0.0f);
            cr::vec3 v21(rd2*std::sin(wd2*time), rd2*std::cos(wd2*time), 0.0f);
            
            
            //mesh.add_lin(v10, v10+v11, col);
            //mesh.add_lin(v10+v11, v10+v11+v12, col);
            
            cr::vec3 v10b(r1*std::sin(w1*(time+dt)),   r1*std::cos(w1*(time+dt)),   0.0f);
            cr::vec3 v11b(rd1*std::sin(wd1*(time+dt)), rd1*std::cos(wd1*(time+dt)), 0.0f);
            cr::vec3 v12b(rf1*std::sin(wf1*(time+dt)), rf1*std::cos(wf1*(time+dt)), 0.0f);
            
            mesh.add_lin(v10+v11+v12, v10b+v11b+v12b, col);
            
            //mesh.add_tri(v10, v10+v11, v20+v21, col);
            //mesh.add_tri(v20+v21, v10+v11, v20, col);
            
            /*
            for (int i=0 ; i<nump ; ++i)
            {
                cr::vec3 vi = cr::mix(v10, v20, (float)(nump+1), (float)(i+1));
                cr::vec3 vm = (v10-v20).withlength(0.5f * tri_size);
                cr::vec3 vt = cr::vec3(-vm.y, vm.x, 0.0f).withlength(tri_size * cr::sqrt3 / 2.0f);
                
                mesh.add_tri(vi, vi+vt+vm, vi+vt-vm, col);
            }
            */
            
            
            /*
            cr::vec3 vi = cr::mix(v10, v20, mixf);
            cr::vec3 vm = (v10-v20).withlength(0.5f);
            cr::vec3 vt = cr::vec3(-vm.y, vm.x, 0.0f).withlength(cr::sqrt3 / 2.0f);
            mesh.add_tri(vi, vi+vt+vm, vi+vt-vm, col);
            */
            
            /*
            ////// Bezier
            float p   = 0.0f;
            float dp  = 1.0f / (nump-1.0f);
            for (int j=0 ; j<nump-1 ; ++j)
            {
                float p1 = (1.0f-p)*(1.0f-p)*(1.0f-p);
                float p2 = 3.0f*p*(1.0f-p)*(1.0f-p);
                float p3 = 3.0f*p*p*(1.0f-p);
                float p4 = p*p*p;
                
                cr::vec3 vi = v10 * p1 + (v10+v11) * p2 + (v20+v21) * p3 + v20 * p4;
                
                cr::vec3 vt = (v11) * (3.0f*p*(1.0f-p)*(1.0f-p)) + (v20+v21-v10-v11)*(6.0f*(1.0f-p)*p) + v21*(-3.0f*p*p);
                
                cr::vec3 vm = cr::vec3(vt.x, -vt.y, 0.0f);
                
                vt *= (tri_size * cr::sqrt3 / 2.0f);
                vm *= (0.5f * tri_size);
                mesh.add_tri(vi, vi+vt+vm, vi+vt-vm, col);
                
                p += dp;
            }
            */
            
            gsgl::clearGPU(meshbuffers);
            gsgl::MUXtoGPU(mesh, meshbuffers);
        }
        
        virtual void init_render() { /*renderers[used_render]->initRender(screen_width, screen_height);*/ }
        virtual void pre_render() { /*renderers[used_render]->preRender();*/ }
        virtual void render()
        {
            cr::mat4 model = cr::scale_mat(scale);
            model *= cr::rotx_mat(rotation);
            model *= cr::roty_mat(axis);
            model *= cr::move_mat(cr::vec3{ panx, pany, 0.0f });
            
            framebuf->use();
            
            renderers[used_render]->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
            renderers[used_render]->pre_render();
            renderers[used_render]->render(*(cameras[used_cam]), model, meshbuffers);
            
            if (used_render_2 >= 0)
            {
                renderers[used_render_2]->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
                glEnable(GL_POLYGON_OFFSET_FILL);
                glPolygonOffset(1, 1);
                renderers[used_render_2]->render(*(cameras[used_cam]), model, meshbuffers);
                glDisable(GL_POLYGON_OFFSET_FILL);
            }

            framebuf->render();
        }
        
        void pan(float x, float y)
        {
            panx += x;
            pany += y;
            
            /*
            for (unsigned int i=0 ; i<mesh->linn*2 ; ++i)
            {
                mesh->lins[i*6]     += x;
                mesh->lins[i*6 + 1] += y;
            }
            
            for (unsigned int i=0 ; i<mesh->trin*3 ; ++i)
            {
                mesh->tris[i*9]     += x;
                mesh->tris[i*9 + 1] += y;
            }
            */
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
            }
            else if (btn == keys::MOUSE_L)
            {
                pan(-0.02f*x / scale, -0.02f*y / scale);
                gsgl::updateGPU(mesh, meshbuffers);
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
                
                case keys::S :
                    for (int i=0 ; i<sweep ; ++i)
                    {
                        time += dt;
                        step_rot();
                    }
                    std::cout << "SWEEP_T " << time << std::endl;
                    break;
                
                case keys::T :
                    time += dt;
                    std::cout << "T " << time << std::endl;
                    step_rot();
                    break;
                
                
                
                default:
                    break;
            }
        }
};

int main ()
{
    cr::scripter ttconf("orbits.conf");
    
    global_loop = new glfw_loop(ttconf);
    scene* s = new o_scene(ttconf);
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
