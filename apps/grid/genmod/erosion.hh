#pragma once

#include <crcc/crcc.hh>

namespace ter {

struct terrain_data_square
{
    int W,H;
    float* height;
    float* water;
    cr::vec2* flow;
    float* sediment1;
    float* sediment2;
    
    terrain_data_square  (int _w, int _h);
    ~terrain_data_square ();
    
    void clear ();
    
    inline float geth(int x, int y)    { return height[y*W + x]; }
    inline float getfx(int _x, int _y) { return flow[_y*W + _x].x; }
    inline float getfy(int _x, int _y) { return flow[_y*W + _x].y; }
};

void erosion_talus (float* hmap, int hw, int hh, float minslope);
void erosion_hydraulic   (terrain_data_square* map);
void erosion_hydraulic_b (terrain_data_square* map);
    
}
