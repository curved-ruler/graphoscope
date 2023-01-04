#pragma once

#include <apps/appbase/scene.hh> 
#include "simple_planet.hh"

class grid_scene : public scene
{
    private:
        std::string target_dir;
        std::string planet_out;
        gsgl::r_nshaded* rrr_n;
        gsgl::r_vshaded* rrr_v;
        
        bool sflow;
        int objrender;

        simple_planet* planet;
        //int px, py;
        
    public:
        grid_scene(cr::scripter& _conf);
        virtual ~grid_scene();
        
        // currently: milliseconds
        virtual void step(float dt);
        
        virtual void save();
        
        virtual void init_render();
        virtual void pre_render();
        virtual void render();
        
        void lookup (float a);
        void rotate (float a);
        
        virtual void keyaction   (int key, int action, int mods);
        //virtual void mousebutton (int key, int action, int mods);
        virtual void mousescroll (float x, float y);
        virtual void mousedrag   (float xpos, float ypos, float xdiff, float ydiff, int btn);
        //virtual void mousemove   (float xpos, float ypos, float xdiff, float ydiff);

};
