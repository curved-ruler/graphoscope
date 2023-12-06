
#pragma once

#include <apps/appbase/scene.hh>
#include <crcc/crcc.hh>
#include <gsgl/gsgl.hh>

#include "planet.hh"


class universe : public scene
{
    private:
        std::string target_dir;

        float max_dt;
        
        cr::mesh_ux* shipCpu;
        cr::rrr_buffers shipGpu;
        cr::vec3 pos;
        cr::vec3 look;
        cr::vec3 up;
        float scale;
        
        cr::vec3 up2; cr::vec3 look2; float up2a, look2a; cr::mat4 a,b;
        
        //cr::mesh_ux* starsCpu;
        cr::rrr_buffers starsGpu;
        
        planet* p1;
        planet* p2;
        
        gsgl::r_nshaded* rrr_n;
        gsgl::r_vshaded* rrr_v;
        
    public:
        universe (cr::scripter& conf);
        virtual ~universe ();
        
        virtual void step (float dt) override;
        virtual void save () override;
        
        virtual void init_render () override;
        virtual void pre_render  () override;
        virtual void render      () override;
        
        virtual void keyaction (int key, int action, int mods) override;
        //virtual void mousedrag (float xpos, float ypos, float xdiff, float ydiff, int dragbutton) override;
        virtual void mousemove (float xpos, float ypos, float xdiff, float ydiff) override;
};
