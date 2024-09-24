#pragma once

#include <crcc/crcc.hh>
#include <gsgl/gsgl.hh>

#include <vector>
#include <string>


class cell
{
    private:
        cr::rrr_buffers gpuPointers;
        cr::mesh_ux* geometry;
        
        //unsigned int objectNum
        //drawable** objects
    public:
        bool usedInNextIter;
        
        // : public cr::vec_placed
        cr::vec3 pos;
        cr::vec3 look;
        cr::vec3 up;
        float scale;
        
    public:
        cell(const std::string& file, float dx, float dy, float s);
        ~cell();
        
        void draw (const cr::camera& c, cr::renderer* rrr) const;
        void drawcpu (const cr::camera& c, cr::renderer* rrr) const;
        
        float height (float xf, float yf, float tilesize);
};
    
class plain_planet
{
    private:
        float tilesize;
        float scale;
        int w, h, pw, ph;
        int xo1, yo1, xn1, yn1;
        
        std::string base_dir;
        std::string* partFiles;
        bool partsAreEmpty;
        cell** parts;
        cell** newparts;
        
        //unsigned int drawdist;
        
    private:
        void initcells();
        void modWindowAndPosition();
        //void modcellCoords(int i, int j, int& i2, int& j2) const;
        
    public:
        plain_planet  (const std::string& mapfile);
        ~plain_planet ();
        
        void get_surface (cr::vec3 test_in, cr::vec3& out);
        
        void updatecells (float x, float y);
        
        void draw (const cr::camera& cam, cr::renderer* rrr) const;
        void drawcpu (const cr::camera& cam, cr::renderer* rrr) const;
        
        bool newpos (cr::vec3 in, cr::vec3& newp);
};
