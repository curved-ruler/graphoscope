
#include <iostream>
#include <string>

#include "planetmaker_base.hh"
#include <apps/appbase/glfw_loop.hh>
#include <crcc/crcc.hh>

glfw_loop* global_loop;

void key_callback         (GLFWwindow* /*wnd*/, int key, int scancode, int action, int mods) { global_loop->key(key, scancode, action, mods); }
void scroll_callback      (GLFWwindow* /*wnd*/, double xoffset, double yoffset)              { global_loop->scroll(xoffset, yoffset); }
void cursorpos_callback   (GLFWwindow* /*wnd*/, double xpos, double ypos)                    { global_loop->cursorpos(xpos, ypos); }
void mousebutton_callback (GLFWwindow* /*wnd*/, int button, int action, int mods)            { global_loop->mousebutton(button, action, mods); }

int main ()
{
    std::string confstr = cr::read_file("planetmaker.conf");
    cr::scripter ttconf(confstr);
    
    global_loop = new glfw_loop(ttconf);
    scene* mts = new planetmaker_base(ttconf);
    global_loop->set_scene(mts);
    
    glfwSetKeyCallback        (global_loop->window, key_callback);
    glfwSetScrollCallback     (global_loop->window, scroll_callback);
    glfwSetCursorPosCallback  (global_loop->window, cursorpos_callback);
    glfwSetMouseButtonCallback(global_loop->window, mousebutton_callback);
    
    int ret = global_loop->loop();
    global_loop->done();
    
    delete mts;
    delete global_loop;
    
    return ret;
}
