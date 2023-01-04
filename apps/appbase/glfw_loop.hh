#pragma once

#include <map>
#include <chrono>
#include <thread>

#include <crcc/crcc.hh>
#include <gsgl/gsgl.hh>
#include <GLFW/glfw3.h>

#include "scene.hh"




typedef std::chrono::microseconds time_meas;

class glfw_loop
{
    public:
        float minframetime;
        
        GLFWwindow* window;
        
        std::map<int, int> keymap;
    
    private:
        scene* data;
        
        unsigned int DEF_WIDTH, DEF_HEIGHT;
        unsigned int W, H;
        bool fullscreen;
        
        std::chrono::time_point<std::chrono::steady_clock> t1, t2;
        std::chrono::duration<float> dt;
        
        std::string sessionid;
        std::string screen_prefix;
        unsigned int scr_counter;
        unsigned int scr_channel_comps;
        unsigned char* screen_s;
        
        int dragbutton;
        bool mouse_drag;
        bool mouse_move_event;
        bool eat_cursor;
        double mx, my, mxo, myo;
        
        std::string target_dir;
        std::string screen_dir;
        std::string gif_dir;
        
        std::string appname;
        
    private:
        void create_keymap    ();
        void toggleFullscreen ();
        int  find_key         (int glfw_key);
        int  find_action      (int glfw_act);
        int  map_mods         (int glfw_mods);
        int  find_mousebutton (int glfw_mb);
        
    public:
        glfw_loop  (cr::scripter& config_file);
        ~glfw_loop ();
        
        void save_screen (bool gif);
        
        void set_scene (scene* _scene);
        
        void key         (int keycode, int scancode, int action, int mods);
        void chr         (unsigned int codepoint);
        void scroll      (double xoffset, double yoffset);
        void cursorpos   (double xpos, double ypos);
        void mousebutton (int button, int action, int mods);
        
        void one_loop ();
        int  loop ();
        void done ();
};
