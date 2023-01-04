#pragma once

#include <apps/appbase/scene.hh>
#include <crcc/crcc.hh>
#include <gsgl/gsgl.hh>
#include "uv_sphere_planet.hh"



class planetmaker_base : public scene
{
    private:
        std::string target_dir;
        std::string planet_out;
        
        ////planet
        uv_sphere_planet* planet;
        cr::vec_placed mtr;
        cr::mat4 mtr2;
        float axis;
        float rotation;
        bool rotdir;
        int lod;
        float radius;
        
        int cam_handling;
        
        int   simn;
        float simr;
        
        cr::mesh_ux* worldCoord;
        cr::rrr_buffers worldCoordGpu;
        
        /*
        int patch_lod;
        float patch_r;
        int patch_fi0;
        int patch_fin;
        int patch_th0;
        int patch_thn;
        cr::MeshB* modelgeom;
        r::RrrBuffers gpuModel;
        */
        
    public:
        planetmaker_base(cr::scripter& conf);
        virtual ~planetmaker_base();
        
        // currently: milliseconds
        virtual void step(float clock) override;

        void startrot();
        
        virtual void save() override;
        
        virtual void init_render() override;
        virtual void pre_render() override;
        virtual void render() override;
        
        virtual void keyaction   (int key, int action, int mods) override;
        virtual void mousescroll (float x, float y) override;
        virtual void mousedrag   (float xpos, float ypos, float xdiff, float ydiff, int btn) override;
        virtual void mousebutton (int key, int action, int mods) override;

};
