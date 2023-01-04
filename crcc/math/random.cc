
#include "random.hh"

#include <cmath>


namespace cr {

fdice::fdice(unsigned int seed, float min, float max)
{
    dist = new std::uniform_real_distribution<float>(min, max);
    gen = new std::mt19937(seed);
}
fdice::~fdice()
{
    delete dist;
    delete gen;
}
float fdice::next()
{
    return dist->operator()(*gen);
}



idice::idice(unsigned int seed, int min, int max)
{
    dist = new std::uniform_int_distribution<int>(min, max);
    gen = new std::mt19937(seed);
}
idice::~idice()
{
    delete dist;
    delete gen;
}
int idice::next()
{
    return dist->operator()(*gen);
}

// https://iquilezles.org/www/articles/gradientnoise/gradientnoise.htm
vec2 hash (vec2 x)
{
    const vec2 k = vec2( 0.3183099f, 0.3678794f );
    x = x*k + vec2(k.y, k.x);
    return fract( k*fract( x.x*x.y*(x.x+x.y))*16.0f ) * 2.0f - vec2(1.0f, 1.0f);
}

// returns 3D value noise (in .x)  and its derivatives (in .yz)
vec3 grad_noise (vec2 x)
{
    vec2 i = floor( x );
    vec2 f = fract( x );

    vec2 u = f*f*f*(f*(f*6.0f-vec2(15.0f, 15.0f)) + vec2(10.0f, 10.0f));
    vec2 du = f*f*(f*(f-vec2(2.0f, 2.0f)) + vec2(1.0f, 1.0f)) * 30.0f;
    
    vec2 ga = hash( i + vec2(0.0f, 0.0f) );
    vec2 gb = hash( i + vec2(1.0f, 0.0f) );
    vec2 gc = hash( i + vec2(0.0f, 1.0f) );
    vec2 gd = hash( i + vec2(1.0f, 1.0f) );
    
    float va = dot( ga, f - vec2(0.0f, 0.0f) );
    float vb = dot( gb, f - vec2(1.0f, 0.0f) );
    float vc = dot( gc, f - vec2(0.0f, 1.0f) );
    float vd = dot( gd, f - vec2(1.0f, 1.0f) );

    vec2 rv2 = ga + (gb-ga)*u.x + (gc-ga)*u.y + (ga-gb-gc+gd)*u.x*u.y +  // derivatives
               du * (vec2(u.y, u.x)*(va-vb-vc+vd) + vec2(vb,vc) - vec2(va, va));
    
    return vec3( va + u.x*(vb-va) + u.y*(vc-va) + u.x*u.y*(va-vb-vc+vd),   // value
                 rv2.x, rv2.y );
}

}
