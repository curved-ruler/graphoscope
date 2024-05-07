
#pragma once

#include <crcc/crcc.hh>

namespace ter {

// https://www.shadertoy.com/view/XdXBRH
cr::vec2 hash ( cr::vec2 p );

// noise from https://iquilezles.org/articles/gradientnoise/
cr::vec3 noised ( cr::vec2 x );

}
