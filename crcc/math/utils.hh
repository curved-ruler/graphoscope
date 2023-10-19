#pragma once

#include <cmath>

#include "mconst.hh"



namespace cr {

    template <typename T>
    inline T abs (T val)
    {
        return ((val < 0) ? -val : val);
    }
    
    template <typename T>
    inline bool zero (T x, T e = eps)
    {
        if (x < -e || x > e) return false;
        return true;
    }
    
    template <typename T>
    inline T powi (T b, T e)
    {
        T ret = b;
        for (T i = T(1) ; i < T(e) ; ++i) { ret *= b; }
        return ret;
    }
    
    template <typename T>
    inline T modi (T x, T m)
    {
        if (x >= 0) return (x - (x/m)*m);
        else        return (((-x-1)/m) + 1)*m + x;
    }
    
    template <typename T>
    inline T modf (T x, T m)
    {
        return (x - floor(x / m)*m);
    }
    
    template <typename T>
    inline T mix (T a, T b, real t)
    {
        return a*(1.0f-t) + b*t;
    }
    template <typename T>
    inline T mix (T a, T b, int sub, int i)
    {
        return (a*(sub-i) + b*(i)) / (sub);
    }
    template <typename T>
    inline T mix (T a, T b, real sub, real i)
    {
        return (a*(sub-i) + b*(i)) / (sub);
    }
}
