#pragma once

#include <apps/appbase/scene.hh>
#include <crcc/crcc.hh>
#include <gscc/gscc.hh>
#include <gsgl/gsgl.hh>
#include <gsui/gsui.hh>
#include "cubic_sphere_planet.hh"

class universe : public scene
{
    private:
        std::string target_dir;
        std::string font_dir;
        std::string v_s;

        float max_dt;
        
        int used_vehicle;
        bool showplane;
        
        cr::aeroplane* plane;
        cr::mesh_ux* planeCpu;
        cr::rrr_buffers planeGpu;
        
        cr::foot_cam* foot_cam;
        cr::spectator_cam* spec_cam;
        
        cr::vec3 vpos1;
        /*
        std::string p_s;
        cr::FootCam* foot;
        float distUpdatePlanet;
        */
        
        std::vector<cubic_sphere_planet*> planets;
        
        std::string renderengine;
        int rei;
        gscc::r_pathtracer* renderer_pt;
        
        gsui::uiengine* uie;
        
    private:
        void perspsh(float d);
        
    public:
        universe (cr::scripter& conf);
        virtual ~universe ();
        
        virtual void step (float dt) override;
        virtual void save () override;
        
        virtual void init_render () override;
        virtual void pre_render  () override;
        virtual void render      () override;
        
        virtual void keyaction (int key, int action, int mods) override;
        virtual void mousedrag (float xpos, float ypos, float xdiff, float ydiff, int dragbutton) override;
        virtual void mousemove (float xpos, float ypos, float xdiff, float ydiff) override;
};
