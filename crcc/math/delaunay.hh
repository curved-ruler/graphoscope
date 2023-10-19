
#pragma once

#include <vector>

#include "../base.hh"
#include "mat.hh"

namespace cr {

// a,b,c should be counterclockwise
float incircle   (vec2 a, vec2 b, vec2 c, vec2 d); // > 0 true
// signed area
float tri_s_area (vec2 a, vec2 b, vec2 c);
vec3 barycentric (vec2 a, vec2 b, vec2 c, vec2 p);
bool tri_inside  (vec2 a, vec2 b, vec2 c, vec2 p);


struct delaunay_maker
{
    std::vector<vec2>  p; // first three points are the 'all included' tirangle
    std::vector<ivec3> t; // first triangle      is the 'all included' triangle
    
    // check ab edge of the i.th triangle
    void checkflip (size_t i, int a, int b);
};

void delaunay_step(delaunay_maker& m, const std::vector<vec2>& pts, size_t pti);
void delaunay (const std::vector<vec2>& pts, std::vector<int>& out);

} 
