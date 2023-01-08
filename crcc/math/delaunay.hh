
#pragma once

#include <vector>

#include "../base.hh"
#include "mat.hh"

namespace cr {

float incircle (vec2 a, vec2 b, vec2 c, vec2 d); // > 0 true
float inside   (vec2 a, vec2 b, vec2 c, vec2 p); // < 0 true
float area     (vec2 a, vec2 b, vec2 c);


void delaunay (const std::vector<vec2>& pts, std::vector<int>& out);

} 
