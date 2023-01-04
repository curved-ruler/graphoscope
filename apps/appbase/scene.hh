#pragma once

#include <vector>

#include <crcc/crcc.hh>
#include <gsgl/gsgl.hh>

class scene
{
    protected:
        cr::scripter& sysconf;
        
        int used_render;
        int used_render_2;
        cr::render_mode rmode;
        
        gsgl::texture2* screen_tex;
        gsgl::framebuffer* framebuf;
        
        unsigned int used_cam;
        std::vector<cr::camera*> cameras;
        
        GLuint vao;
        
    public:
        scene(cr::scripter& _conf);
        virtual ~scene();
        
        void resize_screen(unsigned int w, unsigned int h);
        virtual void resize();
        
        // currently: milliseconds
        virtual void step(float dt) = 0;
        
        virtual const char* to_clipboard();
        virtual void from_clipboard(const char* text);
        
        virtual void save();
        virtual void init_render();
        virtual void pre_render();
        virtual void render() = 0;
        
        virtual void keyaction   (int key, int action, int mods);
        virtual void chr         (unsigned int codepoint);
        virtual void mousebutton (int key, int action, int mods);
        virtual void mousescroll (float x, float y);
        virtual void mousedrag   (float xpos, float ypos, float xdiff, float ydiff, int dragbutton);
        virtual void mousemove   (float xpos, float ypos, float xdiff, float ydiff);
};
