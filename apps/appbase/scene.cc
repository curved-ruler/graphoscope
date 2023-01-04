#include "scene.hh"

#include <cmath>
#include <iostream>
#include <string>

#include <crcc/crcc.hh>



scene::scene(cr::scripter& _conf) : sysconf(_conf)
{
    used_cam = 0;
    
    framebuf   = nullptr;
    screen_tex = nullptr;
    
    sysconf.getvalue("folders.shader_dir", rmode.shader_dir);
    
    sysconf.getvalue("window.pixel_size", rmode.pixel_size, 1);
    if (rmode.pixel_size < 1) rmode.pixel_size = 1;
    
    sysconf.getarray("window.backgr", rmode.back, 3);
    sysconf.getarray("window.col",    rmode.col, 3);
    sysconf.getvalue("window.alpha",  rmode.col[3], 0.3f);
    
    sysconf.getvalue("window.width",  rmode.screen_w, 400);
    sysconf.getvalue("window.height", rmode.screen_h, 400);
    rmode.viewport[0] = 0;
    rmode.viewport[1] = 0;
    rmode.viewport[2] = rmode.screen_w;
    rmode.viewport[3] = rmode.screen_h;
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
}

scene::~scene()
{
    for (unsigned int ci = 0 ; ci < cameras.size() ; ++ci)
    {
        delete cameras[ci];
        cameras[ci] = 0;
    }
    cameras.clear();
    
    delete framebuf;
    delete screen_tex;
    
    glDeleteVertexArrays(1, &vao);
}

void scene::resize() {}

void scene::resize_screen(unsigned int w, unsigned int h)
{
    rmode.screen_w = w;
    rmode.screen_h = h;
    rmode.viewport[2] = w;
    rmode.viewport[3] = h;
    
    delete screen_tex;
    delete framebuf;
    screen_tex = new gsgl::texture2(w/rmode.pixel_size, h/rmode.pixel_size, GL_RGB);
    framebuf   = new gsgl::framebuffer(&rmode, screen_tex->id);
    
    for (unsigned int ci=0 ; ci < cameras.size() ; ++ci)
    {
        cameras[ci]->aspect = (float)w / (float)h;
    }
    
    resize();
    init_render();
}



const char* scene::to_clipboard() { return "X"; }
void scene::from_clipboard(const char* text) { if (text) std::cout << "From clipboard: " << text << std::endl; }




void scene::save() {}
void scene::init_render() {}
void scene::pre_render() {}

void scene::keyaction   (int /*key*/, int /*action*/, int /*mods*/) {}
void scene::chr         (unsigned int /*codepoint*/) {}
void scene::mousebutton (int /*key*/, int /*action*/, int /*mods*/) {}
void scene::mousescroll (float /*x*/, float /*y*/) {}
void scene::mousedrag   (float /*xpos*/, float /*ypos*/, float /*xdiff*/, float /*ydiff*/, int /*dragbutton*/) {}
void scene::mousemove   (float /*xpos*/, float /*ypos*/, float /*xdiff*/, float /*ydiff*/) {}

