#pragma once

#include <crcc/crcc.hh>
#include <gsgl/gsgl.hh>

#include <vector>
#include <string>

//#define DEBUG_CRACK

struct uv_sphere { float fi; float th; };

class uv_sphere_planet
{
    public:
        cr::vec3 pos;
        cr::vec3 look;
        cr::vec3 up;
        float scale;
        
        int N; // must be power of 2
        int K;
        float th0, dth, dfi;
        float radius;
        
        int hy, hx;
        float* heights;
        float hmin, hmax;
        
        cr::mesh_ux planetMesh;
        cr::mesh_ux contourMesh;
        cr::mesh_ux debugMesh;
        cr::rrr_buffers planetGpu;
        cr::rrr_buffers debugBuff;
        
        cr::fdice* dice01;
        ter::del_noise* noised;
        cr::height_pal* pal;
        
        cr::scripter& conf;
        
    public:
        //uv_sphere_planet (const std::string& mapfile);
         uv_sphere_planet (cr::scripter& _conf, float _r, int _part_size, int _part_k);
        ~uv_sphere_planet ();
        
        void create_mesh();
        void create_contour();
        
        void stat();
        
        void ds();
        void noise1();
        void del_noise();
        void crack(int k, float d);
        
        //void get_surface (cr::vec3 test_in, cr::vec3& out);
        //bool newpos (cr::vec3 in, cr::vec3& newp);
        
        //void updateTerrainParts (float x, float y);
        //void draw (const cr::camera& cam, cr::renderer* rrr) const;
        void draw (const cr::camera& cam, cr::mat4 tr, cr::renderer* rrr) const;

}; // class uv2_sphere_planet
