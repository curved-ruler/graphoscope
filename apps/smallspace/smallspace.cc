
#include <iostream>

#include "universe.hh"
#include <apps/appbase/glfw_loop.hh>
#include <crcc/crcc.hh>

glfw_loop* global_loop;

void key_callback         (GLFWwindow* /*wnd*/, int key, int scancode, int action, int mods) { global_loop->key(key, scancode, action, mods); }
void scroll_callback      (GLFWwindow* /*wnd*/, double xoffset, double yoffset)              { global_loop->scroll(xoffset, yoffset); }
void cursorpos_callback   (GLFWwindow* /*wnd*/, double xpos, double ypos)                    { global_loop->cursorpos(xpos, ypos); }
void mousebutton_callback (GLFWwindow* /*wnd*/, int button, int action, int mods)            { global_loop->mousebutton(button, action, mods); }

int main ()
{
    cr::scripter ttconf(R"CONF(
folders = {
    target        : /stuff/S/graphoscope/graphoscope2/dist/workdir,
    shader_dir    : /stuff/S/graphoscope/graphoscope2/gsgl/glrender/shaders,
    font_dir      : fonts,
    screen_dir    : screens,
    screen_prefix : sp_
}

window = {
    width  : 1280,
    height : 720,
    backgr : [0.1, 0.1, 0.1],
    col    : [0.9, 0.9, 0.9],
    alpha  : 1.0,
    pixel_size : 1,
    point_size : 4,
    name   : "Grid"
}
)CONF");
    
    global_loop = new glfw_loop(ttconf);
    scene* nws = new universe(ttconf);
    global_loop->set_scene(nws);
    
    glfwSetKeyCallback        (global_loop->window, key_callback);
    //glfwSetScrollCallback     (global_loop->window, scroll_callback);
    glfwSetCursorPosCallback  (global_loop->window, cursorpos_callback);
    //glfwSetMouseButtonCallback(global_loop->window, mousebutton_callback);
    
    
    int ret = global_loop->loop();
    global_loop->done();
    
    delete nws;
    delete global_loop;
    return ret;
}
