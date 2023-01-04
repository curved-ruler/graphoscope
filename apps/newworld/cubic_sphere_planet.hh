#pragma once

#include <crcc/crcc.hh>
#include <gsgl/gsgl.hh>

#include <vector>
#include <string>


enum class comp// : u8
{
    north = 0, // north pole, or up, or z
    front = 1, // look, or x
    east  = 2,
    south = 3, // south pole
    back  = 4,
    west  = 5,
    count = 6
}; // enum comp

struct uv_cubic
{
    cr::vec3 p;
    cr::vec3 u;
    cr::vec3 v;
    float    n;
};

struct cspart_id
{
    comp c;
    int  x0;
    int  y0;
    int  lod;
    int  sides; // u8
};

class cubic_sphere_planet;
class terrain_part
{
    private:
        cubic_sphere_planet* planet;
        cspart_id id;
        
        cr::mesh_ux* geom;
        cr::rrr_buffers gpuPointers;
        //float* height_map;
        
    public:
        bool usedInNextIter;
        
    public:
        terrain_part(cubic_sphere_planet* _planet, cspart_id _id);
        ~terrain_part();
        
        void draw    (const cr::camera& c, cr::renderer* rrr) const;
        void draw_cpu(const cr::camera& c, cr::renderer* rrr) const;
        
        //float height (float xf, float yf, float tilesize);
}; // class terrain_part


class cubic_sphere_planet : public cr::vec_placed
{
    public:
        int lod;
        int full_lod;
        
        int part_size; // must be power of 2
        int part_n;
        float radius;
        uv_cubic base[static_cast<int>(comp::count)];
        
        //std::string heights[comp::count];
        //std::string name;
        
        void xy_to_3d(cr::vec3& a, const cspart_id& id, int x, int y);

        
    private:
        std::vector<terrain_part*> terrain;
        
        void load_terrain();
        
    public:
        //cubic_sphere_planet (const std::string& mapfile);
         cubic_sphere_planet (const cr::vec_placed& _place, int _part_size, int _part_n, int _full_lod, int _lod);
        ~cubic_sphere_planet ();
        
        //void get_surface (cr::vec3 test_in, cr::vec3& out);
        //bool newpos (cr::vec3 in, cr::vec3& newp);
        
        //void updateTerrainParts (float x, float y);
        void draw     (const cr::camera& cam, cr::renderer* rrr) const;
        void draw_cpu (const cr::camera& cam, cr::renderer* rrr) const;
}; // class cubic_sphere_planet
