
#include "col.hh"

#include "../math/utils.hh"

namespace cr
{

vec3 hsv2rgb (vec3 c)
{
    vec3 cx2 = vec3(c.x*6.0, c.x*6.0, c.x*6.0) + vec3(0.0, 4.0, 2.0);
    
    cx2.x = modf(cx2.x, 6.0f);
    cx2.y = modf(cx2.y, 6.0f);
    cx2.z = modf(cx2.z, 6.0f);
    
    vec3 rgb = absv( cx2-vec3(3.0, 3.0, 3.0) ) - vec3(1.0, 1.0, 1.0);
    
    if (rgb.x < 0.0) { rgb.x = 0.0; }  if (rgb.x > 1.0) { rgb.x = 1.0; }
    if (rgb.y < 0.0) { rgb.y = 0.0; }  if (rgb.y > 1.0) { rgb.y = 1.0; }
    if (rgb.z < 0.0) { rgb.z = 0.0; }  if (rgb.z > 1.0) { rgb.z = 1.0; }
    
    return mix( vec3(1.0, 1.0, 1.0), rgb, c.y) * c.z;
}

vec3 rgb2hsv (vec3 c)
{
    vec4 k = vec4(0.0, -1.0/3.0, 2.0/3.0, -1.0);
    vec4 p = mix(vec4(c.z, c.y, k.w, k.z), vec4(c.y, c.z, k.x, k.y), (c.z<c.y) ? 1.0 : 0.0);
    vec4 q = mix(vec4(p.x, p.y, p.w, c.x), vec4(c.x, p.y, p.z, p.x), (p.x<c.x) ? 1.0 : 0.0);
    float d = q.x - std::min(q.w, q.y);
    return vec3(abs(q.z + (q.w - q.y) / (6.0*d+0.0000001)),
                d / (q.x+0.0000001),
                q.x);
}

}
