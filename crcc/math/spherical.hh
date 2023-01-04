
#include <cmath>
#include "vec.hh"

namespace cr {

inline vec3 sphere_cartesian(float r, float th, float fi)
{
    return vec3(r*std::sin(th)*std::cos(fi),
                r*std::sin(th)*std::sin(fi),
                r*std::cos(th));
}
inline float sphere_radius(vec3 v)
{
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

}
