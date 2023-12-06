#pragma once

#include <cmath>
#include <iostream>

#include "utils.hh"
#include "vec.hh"
#include "mat.hh"
#include "quat.hh"



namespace cr {

// v should be normalized
// returns a vector (not normalized!) which is orthogonal to v
template <typename T>
vec3_t<T> norm_ortho (vec3_t<T> v)
{
    vec3_t<T> xi((T)1, (T)0, (T)0);
    vec3_t<T> yi((T)0, (T)1, (T)0);
    T cosangle = dot(v, xi);
    if (cr::abs(cosangle) < 0.9)
    {
        return cross(v, xi);
    }
    else
    {
        return cross(v, yi);
    }
}

template <typename T>
inline bool same_facing (vec3_t<T> v1, vec3_t<T> v2)
{
    return (dot(v1,v2) > 0);
}

template <typename T>
inline mat4_t<T> scale_mat (T sx, T sy, T sz)
{
    return mat4_t<T>({
        sx,  0,  0,  0,
         0, sy,  0,  0,
         0,  0, sz,  0,
         0,  0,  0,  1
    });
}

template <typename T>
inline mat4_t<T> scale_mat (T sx)
{
    return scale_mat(sx, sx, sx);
}

template <typename T>
mat4_t<T> move_mat (vec3_t<T> pos)
{
    mat4_t<T> tr = {
        1, 0, 0, pos.x,
        0, 1, 0, pos.y,
        0, 0, 1, pos.z,
        0, 0, 0,   1
    };
    return tr;
}

template <typename T>
mat4_t<T> rotx_mat (T a)
{
    T S = std::sin(a);
    T C = std::cos(a);
    mat4_t<T> tr = {
        1,  0,  0,  0,
        0,  C, -S,  0,
        0,  S,  C,  0,
        0,  0,  0,  1
    };
    return tr;
}

template <typename T>
mat4_t<T> roty_mat (T a)
{
    T S = std::sin(a);
    T C = std::cos(a);
    mat4_t<T> tr = {
        C,  0,  S,  0,
        0,  1,  0,  0,
       -S,  0,  C,  0,
        0,  0,  0,  1
    };
    return tr;
}

template <typename T>
mat4_t<T> rotz_mat (T a)
{
    T S = std::sin(a);
    T C = std::cos(a);
    mat4_t<T> tr = {
        C, -S,  0,  0,
        S,  C,  0,  0,
        0,  0,  1,  0,
        0,  0,  0,  1
    };
    return tr;
}

template <typename T>
mat4_t<T> rot_mat (T alpha, vec3_t<T> axis)
{
    vec3_t<T> t = axis;
    t.normalize();
    T S = std::sin(alpha);
    T C = std::cos(alpha);
    mat4_t<T> tr = {
        C*(1 - t.x*t.x) + t.x*t.x,     t.y*t.x*(1-C) - S*t.z,       t.z*t.x*(1-C) + S*t.y,     0,
          t.x*t.y*(1-C) + S*t.z,     C*(1 - t.y*t.y) + t.y*t.y,     t.z*t.y*(1-C) - S*t.x,     0,
          t.x*t.z*(1-C) - S*t.y,       t.y*t.z*(1-C) + S*t.x,     C*(1 - t.z*t.z) + t.z*t.z,   0,
                   0,                           0,                           0,                1
    };
    return tr;
}

template <typename T>
vec3_t<T> rot_q (quat_t<T> q, vec3_t<T> v)
{
    quat_t<T> qq = q.conj();
    quat_t<T> qv(0, v.x, v.y, v.z);
    quat_t<T> ret = q * qv * qq;
    return vec3_t<T>(ret.i, ret.j, ret.k);
}

inline void lu_constrain(vec3& look, vec3& up)
{
    look.normalize();
    
    vec3 u = cross(look, up);
    
    up = cross(u, look);
    up.normalize();
}

// NOT TESTED
inline quat lu_transf(vec3 look, vec3 up)
{
    vec3 xi = vec3(1,0,0);
    vec3 zi = vec3(0,0,1);
    
    quat q1 = quat(0, 1,0,0);
    vec3 v1 = cross(xi, look);
    v1.normalize();
    
    if (v1.lenn() > 0.05)
    {
        q1   = quat( std::acos(dot(xi, look)), v1 );
        zi   = rot_q(q1, zi);
    }
    
    quat q2 = quat(0, 1,0,0);
    vec3 v2 = cross(zi, up);
    v2.normalize();
    
    if (v2.lenn() > 0.05)
    {
        q2   = quat( std::acos(dot(zi, up)), v2 );
    }
    
    return q1*q2;
}

}
