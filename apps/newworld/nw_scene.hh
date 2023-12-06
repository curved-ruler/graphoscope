#pragma once

#include <apps/appbase/scene.hh>
#include <crcc/crcc.hh>
#include <gsgl/gsgl.hh>
#include "plain_planet.hh"

class nw_scene : public scene
{
    private:
        std::string target_dir;
        std::string v_s;
        std::string p_s;

        float maxDt;
        
        static const int V_FOOT   = 0;
        static const int V_AEROPL = 1;
        static const int V_SPACES = 2;
        static const int V_FREE   = 3;
        int used_vehicle;
        
        cr::aeroplane* plane;
        cr::foot_cam* foot;
        
        bool showplane;
        cr::rrr_buffers planegpu;
        
        cr::vec3 vpos1;
        float distUpdatePlanet;
        plain_planet* planet;
        
        int objrender;

    private:
        void perspsh(float d);
        
    public:
        nw_scene (cr::scripter& conf);
        virtual ~nw_scene ();
        
        virtual void step (float dt);
        virtual void save ();
        
        virtual void init_render ();
        virtual void pre_render  ();
        virtual void render      ();
        
        virtual void keyaction (int key, int action, int mods);
        virtual void mousedrag (float xpos, float ypos, float xdiff, float ydiff, int dragbutton);
        virtual void mousemove (float xpos, float ypos, float xdiff, float ydiff);
};
