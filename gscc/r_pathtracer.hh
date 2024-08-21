
#pragma once

#include <random>

#include <crcc/crcc.hh>


namespace gscc {


class r_pathtracer : public cr::renderer
{
    public:
        cr::render_mode* settings;
        float* screen;
        float* depth;
        float max_depth;
        unsigned int sw, sh;
        
        std::mt19937* rgen;
        std::vector<unsigned int> perm;
        unsigned int ipixel;
        
        int thread_k;
        int path_n;
        
    public:
        r_pathtracer();
        virtual ~r_pathtracer();
        
        virtual void setup       (cr::render_mode* settings) override;
        virtual void init_render (unsigned int w, unsigned int h) override;
        virtual void pre_render  () override;
        
        void render_pixel (int pix, const cr::camera& cam, const cr::mat4& modeltr, const cr::mesh_ux& mesh);
        virtual void render      (const cr::camera& cam, const cr::mat4& modeltr, const cr::mesh_ux& mesh) override;
};

}
