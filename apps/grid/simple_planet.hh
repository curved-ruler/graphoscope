
#pragma once

#include <crcc/crcc.hh>
#include <gsgl/gsgl.hh>
#include "genmod/gen.hh"
#include "genmod/diamond_square.hh"
#include "genmod/erosion.hh"
#include "genmod/warp.hh"
#include "genmod/kernel.hh"
#include "genmod/game_of_life.hh"

#include <vector>
#include <string>

class simple_planet;
class terrain_part
{
    public:
        cr::vec3 pos;
        float scale;
        
        cr::mesh_ux* gridgeom;
        cr::rrr_buffers gpuGrid;
        
        cr::mesh_ux* flowgeom;
        cr::rrr_buffers gpuFlow;
        int flow_density;
        float flowz;
        
        cr::mesh_ux* cgeom;
        cr::rrr_buffers gpuContour;
        
        int size;
        
    public:
        bool usedInNextIter;
        
    public:
        terrain_part(const std::string& file, int partsize, float dx, float dy, float s);
        terrain_part(int partsize, float dx, float dy, float s);
        ~terrain_part();
        
        void clear ();
        void init_grid ();
        
        void setpos (float _x, float _y, float _z);
        void readpart       (simple_planet* planet, int px, int py);
        void readpart_plates(cr::height_pal* pal, ter::terrain_data_square* map, int px, int py);
        void create_contour (cr::height_pal* pal, ter::terrain_data_square* map, int px, int py);
        void save_contour   (const std::string& dir, const std::string& name_base);
};

class simple_planet
{
    friend class terrain_part;
    private:
        float tilesize;
        float scale;
        float gap;
        bool sflow;
        //int w, h;
        //int xo1, yo1, xn1, yn1;
        
        ter::terrain_data_square* map;
        ter::terrain_data_square* map_tmp;
        
        cr::height_pal* pal;
        cr::height_pal* pal2;
        
        ter::noise* noisegen;
        
        std::string base_dir;
        std::string* partFiles;
        bool partsAreEmpty;
        
        bool contoured;
        
        cr::idice* idicew;
        cr::idice* idiceh;
        cr::fdice* fd;
        
        //unsigned int drawdist;
        
    public:
        terrain_part** parts;
        int partsize;
        int pw, ph, W, H;
        float hmin, hmax;
        
    public:
        //simple_planet  (const std::string& mapfile);
        simple_planet  (int p_partsize, int p_x, int p_y);
        ~simple_planet ();
        
        void updateGpu(bool slice);
        //void updateterrain_parts ();
        //void updateterrain_parts (int x, int y);
        //void more();
        //void less();
        //void setdim (int _w, int _h);
        
        void reset ();
        
        void generate_ds   (bool slice, bool dsmap);
        void generate_warp (bool slice);
        void crack ();
        void erosion_step   ();
        void erosion_2_step ();
        void filter  ();
        void gol     ();
        void golinit ();
        void rd      ();
        void rdinit  ();
        void rdcalc  ();
        void rdcalc2 ();
        void quantize(float q);
        void edgefit (int k);
        void create_contour();
        
        void stat();
        
        void moregap    ();
        void lessgap    ();
        void switchflow ();

        void save (const std::string& dir, const std::string& name_base);
        void savepart (const std::string& dir, const std::string& name_base, int i, int j);
};
