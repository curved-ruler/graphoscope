
#include <iostream>
#include <string>

#include <apps/appbase/scene.hh>
#include <apps/appbase/glfw_loop.hh>
#include <crcc/crcc.hh>
#include <gsui/gsui.hh>
#include <gsgl/gsgl.hh>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
//#include <gcrypt.h>


glfw_loop* global_loop;

void key_callback         (GLFWwindow* /*wnd*/, int key, int scancode, int action, int mods) { global_loop->key(key, scancode, action, mods); }
void char_callback        (GLFWwindow* /*wnd*/, unsigned int codepoint)                      { global_loop->chr(codepoint); }
void scroll_callback      (GLFWwindow* /*wnd*/, double xoffset, double yoffset)              { global_loop->scroll(xoffset, yoffset); }
void cursorpos_callback   (GLFWwindow* /*wnd*/, double xpos, double ypos)                    { global_loop->cursorpos(xpos, ypos); }
void mousebutton_callback (GLFWwindow* /*wnd*/, int button, int action, int mods)            { global_loop->mousebutton(button, action, mods); }




class c_scene : public scene
{
    private:
        std::string target_dir;
        std::string font_dir;
        std::string font_conf;
        gs::uiengine* uie;
        gs::textarea* ta;
        cr::material col;
        
        float scale = 1.0f;
        
        gsgl::r_text rrr;
        cr::rrr_buffers rdata;
        GLuint cbuf;
        
    public:
        c_scene(cr::scripter& _conf) : scene(_conf)
        {
            sysconf.getvalue("folders.target", target_dir);
            target_dir = cr::spath(target_dir);
            
            rmode.back[0] = 0.0f;
            rmode.back[1] = 0.0f;
            rmode.back[2] = 0.0f;
            rmode.back[3] = 1.0f;
            sysconf.getarray("window.backgr", rmode.back, 3);
            sysconf.getvalue("window.alpha", rmode.alpha, 1.0f);
            
            col.albedo = {0.0f, 0.0f, 0.0f};
            float c[3];
            sysconf.getarray("window.col", c, 3);
            col.albedo.x = c[0];
            col.albedo.y = c[1];
            col.albedo.z = c[2];
            
            sysconf.getvalue("folders.font_dir", font_dir);
            font_dir = cr::spath(font_dir);
            sysconf.getvalue("ui.textconf", font_conf);
            
            std::string uie_conf = cr::read_file(target_dir + font_dir + font_conf);
            cr::scripter s(uie_conf);
            uie = new gs::uiengine(s);
            
            uie->settings.shader_dir = rmode.shader_dir;
            
            int w, h, ch;
            //std::cout << "FONT: " << (target_dir + font_dir + uie->settings.font_name).c_str() << std::endl;
            uie->settings.im = stbi_load((target_dir + font_dir + uie->settings.font_name).c_str(), &w, &h, &ch, 0);
            if (ch != 1)
            {
                std::cout << "Font image channel mismatch 1!=" << ch << std::endl;
            }
            if ((w != uie->settings.im_char_x*uie->settings.im_char_cols) && (h != uie->settings.im_char_y*uie->settings.im_char_rows))
            {
                std::cout << "Font image size mismatch" << std::endl;
                std::cout << "    config: [" << uie->settings.im_char_x*uie->settings.im_char_cols << ", " << uie->settings.im_char_y*uie->settings.im_char_rows << "]" << std::endl;
                std::cout << "    png   : [" << w << ", " << h << "]" << std::endl;
            }
            
            rrr.setup(&(uie->settings));
            
            ta = new gs::textarea();
            ta->setup(&(uie->settings));
            ta->setdims(scale, 0.0f /*xsep*/, 0.0f /*ysep*/, rmode.screen_w, rmode.screen_h, false /*wrap*/);
            
            std::string txt = cr::read_file(target_dir + "chartst.txt");
            ta->settext(txt);
            //ta->render();
            
            gsgl::TXTtoGPU(ta->renderdata, rdata);
            
            glGenBuffers(1, &cbuf);
            glBindBuffer(GL_ARRAY_BUFFER, cbuf);
            glBufferData(GL_ARRAY_BUFFER, ta->colors.size() * sizeof(uint32), ta->colors.data(), GL_STATIC_DRAW);
        }
        
        virtual ~c_scene()
        {
            delete ta;
            delete uie;
            glDeleteBuffers(1, &cbuf);
        }
        
        virtual void step(float /*dt*/) {}
        
        virtual void save() {}
        
        virtual void init_render() { /*renderers[used_render]->initRender(screen_width, screen_height);*/ }
        virtual void pre_render() { /*renderers[used_render]->preRender();*/ }
        virtual void render()
        {
            framebuf->use();
            
            rrr.init_render(cr::uvec4 { 0, 0, uint32(rmode.screen_w), uint32(rmode.screen_h) });
            rrr.pre_render();
            
            rrr.render(rdata, cbuf);
            
            framebuf->render();
        }
        
        virtual void mousescroll (float /*x*/, float y)
        {
            if (y < 0.0f) ta->scrolly( 1);
            else          ta->scrolly(-1);
            
            ta->render();
            gsgl::TXTtoGPU(ta->renderdata, rdata);
        }

        virtual void mousebutton (int /*key*/, int /*action*/, int /*mods*/)
        {
            //if (action == keys::PRESS) { std::cout << "MB" << std::endl; }
        }
        
        virtual void mousedrag (float /*xp*/, float /*yp*/, float /*x*/, float /*y*/, int /*btn*/)
        {
        }
        
        virtual void keyaction (int key, int action, int mods)
        {
            if (action == keys::RELEASE) return;
            
            if (key == keys::PLUS && (mods & keys::CONTROL_BIT))
            {
                if (scale < 50.5f)
                {
                    scale += 1.0f;
                    ta->setdims(scale, 0.0f /*xsep*/, 0.0f /*ysep*/, rmode.screen_w, rmode.screen_h, false /*wrap*/);
                    ta->render();
                    gsgl::TXTtoGPU(ta->renderdata, rdata);
                    glBindBuffer(GL_ARRAY_BUFFER, cbuf);
                    glBufferData(GL_ARRAY_BUFFER, ta->colors.size() * sizeof(uint32), ta->colors.data(), GL_STATIC_DRAW);
                }
            }
            else if (key == keys::MINUS && (mods & keys::CONTROL_BIT))
            {
                if (scale > 1.5f)
                {
                    scale -= 1.0f;
                    ta->setdims(scale, 0.0f /*xsep*/, 0.0f /*ysep*/, rmode.screen_w, rmode.screen_h, false /*wrap*/);
                    ta->render();
                    gsgl::TXTtoGPU(ta->renderdata, rdata);
                    glBindBuffer(GL_ARRAY_BUFFER, cbuf);
                    glBufferData(GL_ARRAY_BUFFER, ta->colors.size() * sizeof(uint32), ta->colors.data(), GL_STATIC_DRAW);
                }
            }
            /*
            else if (key == keys::ENTER)
            {
                ta->text += '\n';
                ta->settext(ta->text);
                //ta->render();
                gsgl::TXTtoGPU(ta->renderdata, rdata);
            }
            else if (key == keys::BACKSPACE)
            {
                if (ta->text.size() > 0)
                {
                    //TODO
                    ta->text.erase(ta->text.end()-1);
                    ta->settext(ta->text);
                    //ta->render();
                    gsgl::TXTtoGPU(ta->renderdata, rdata);
                }
            }
            */
        }
        
        virtual void chr (unsigned int /*c*/)
        {
            /*
            char cc[4];
            int t = cr::unicode_32_to_8(c, cc[0], cc[1], cc[2], cc[3]);
            if (t > 0)
            {
                for (int i=0 ; i<t ; ++i) { ta->text.push_back(cc[i]); }
                ta->settext(ta->text);
                //ta->render();
                gsgl::TXTtoGPU(ta->renderdata, rdata);
            }
            */
        }
};

int main ()
{
    std::string conf = cr::read_file("chars.conf");
    cr::scripter ttconf(conf);
    
    global_loop = new glfw_loop(ttconf);
    scene* s = new c_scene(ttconf);
    global_loop->set_scene(s);
    
    
    glfwSetKeyCallback        (global_loop->window, key_callback);
    glfwSetCharCallback       (global_loop->window, char_callback);
    glfwSetScrollCallback     (global_loop->window, scroll_callback);
    glfwSetCursorPosCallback  (global_loop->window, cursorpos_callback);
    glfwSetMouseButtonCallback(global_loop->window, mousebutton_callback);
    
    
    
    int ret = global_loop->loop();
    global_loop->done();
    delete s;
    delete global_loop;
    return ret;
}
