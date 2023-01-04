#include "palette.hh"

#include <cmath>
#include <iostream>

#include "cconst.hh"



namespace cr
{


palette::palette (int s) {
    if (s>0) {
        size = s;
        basecols = new vec3[size];
    } else {
        size = 0;
        basecols = 0;
    }
}
    
palette::~palette () {
    delete[] basecols;
}







vec3 palette::get01 (float x) const {
    /*
    float delta = (float)size - 1.0;
    float small = trunc(x / delta);
    int smi = (int)small;
    float alpha = x-small;
    float beta = 1.0 - alpha;
    */
    float xx = x*(size-1);
    float small = trunc(xx);
    int smi = (int)small;
    if (smi < 0) smi = 0;
    if (smi >= size-1) smi = size-2;
    float alpha = xx - smi;
    float beta = 1.0f - alpha;
    
    vec3 ret;
    ret.x = basecols[smi].x * beta + basecols[smi+1].x * alpha;
    ret.y = basecols[smi].y * beta + basecols[smi+1].y * alpha;
    ret.z = basecols[smi].z * beta + basecols[smi+1].z * alpha;
    //ret.a = basecols[smi].a * beta + basecols[smi+1].a * alpha;
    
    return ret;
}

vec3 palette::get_contcut (float pos) const {
    if (size == 0) return BLACK;
    
    float x;
    if (pos < 0.0f) {
        x = 0.0001f;
    } else if (pos > 1.0f) {
        x = 0.9999f;
    } else {
        x = pos;
    }
    
    return get01(x);
}

vec3 palette::get_contmod (float pos) const {
    if (size == 0) return BLACK;
    
    float x = pos - trunc(pos);
    if (x < 0.0f) x += 1.0f;
    
    return get01(x);
}

vec3 palette::get_disccut (float ifloat) const {
    if (size == 0) return BLACK;
    
    int x = (int)ifloat;
    if (x < 0) {
        x = 0;
    } else if (x >= size) {
        x = size-1;
    }
    return basecols[x];
}

vec3 palette::get_discmod (float ifloat) const {
    if (size == 0) return BLACK;
    int i = (int)ifloat;
    int x = i % size;
    if (x < 0) x+= size;
    return basecols[x];
}







void palette::setmode (colmode m) {
    cm = m;
}
colmode palette::getmode () const {
    return cm;
}

void palette::setcol (vec3 c, int i) {
    if (i>=0 && i<size) basecols[i] = c;
}
vec3 palette::getcol (float pos) const {
    switch (cm) {
        case CONT_CUT: return get_contcut(pos);
        case CONT_MOD: return get_contmod(pos);
        case DISC_CUT: return get_disccut(pos);
        case DISC_MOD: return get_discmod(pos);
        default: return BLACK;
    }
}


//------------------------------------------------------

height_pal::height_pal(int _n)
{
    if (_n > 1)
    {
        colnum = _n;
        cols = new vec3[colnum];
        limits = new float[colnum-1];
    }
    else
    {
        colnum = 0;
        cols = 0;
        limits = 0;
    }
}

height_pal::height_pal(const height_pal& base, int sub)
{
    if (sub > 0 && base.colnum > 0)
    {
        colnum = base.colnum + (base.colnum - 2) * (sub-1);
        cols = new vec3[colnum];
        limits = new float[colnum-1];
    
        cols[0] = base.cols[0];
        limits[0] = base.limits[0];
    
        cols[colnum-1]   = base.cols[base.colnum-1];
        limits[colnum-2] = base.limits[base.colnum-2];
    
        for (int p = 1 ; p < base.colnum-1 ; ++p)
        {
            for (int i = 0 ; i<sub ; ++i)
            {
                limits[1 + (p-1)*(sub) + i] = cr::mix(base.limits[p-1], base.limits[p], sub, i);
                cols[1 + (p-1)*(sub) + i]   = cr::mix(base.cols[p-1], base.cols[p], (float)sub, (float)i);
            }
        }
    }
    else
    {
        colnum = 0;
        cols = 0;
        limits = 0;
    }
}
height_pal::~height_pal()
{
    delete[] cols;
    delete[] limits;
}
    
// TODO binary search?
unsigned int height_pal::get(float height)
{
    int i = 0;
    while ((i < colnum-1) && (limits[i] < height)) ++i;
    return (unsigned)i;
}

}
