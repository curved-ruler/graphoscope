
#include "noise.hh"

#include <cmath>

namespace ter {

cr::vec2 hash ( cr::vec2 p )
{
    cr::vec2 k = cr::vec2( 0.3183099, 0.3678794 );
    cr::vec2 x = p*k + cr::vec2( 0.3678794, 0.3183099 );
    return cr::vec2(-1.0 + 2.0*cr::fract( k.x*16.0*cr::fract( x.x*x.y*(x.x+x.y)) ),
                    -1.0 + 2.0*cr::fract( k.y*16.0*cr::fract( x.x*x.y*(x.x+x.y)) ));
}

cr::vec3 noised ( cr::vec2 x )
{
    cr::vec2 i = cr::floor(x);
    cr::vec2 f = cr::fract(x);

    cr::vec2 u = f*f*f*(f*(f*6.0-cr::vec2(15.0, 15.0))+cr::vec2(10.0,10.0));
    cr::vec2 du = f*f*(f*(f-cr::vec2(2.0,2.0))+cr::vec2(1.0,1.0)) * 30.0;
    
    cr::vec2 ga = hash( i + cr::vec2(0.0,0.0) );
    cr::vec2 gb = hash( i + cr::vec2(1.0,0.0) );
    cr::vec2 gc = hash( i + cr::vec2(0.0,1.0) );
    cr::vec2 gd = hash( i + cr::vec2(1.0,1.0) );
    
    float va = cr::dot( ga, f - cr::vec2(0.0,0.0) );
    float vb = cr::dot( gb, f - cr::vec2(1.0,0.0) );
    float vc = cr::dot( gc, f - cr::vec2(0.0,1.0) );
    float vd = cr::dot( gd, f - cr::vec2(1.0,1.0) );

    cr::vec2 der = ga + (gb-ga)*u.x + (gc-ga)*u.y + (ga-gb-gc+gd)*u.x*u.y +
                   du * (cr::vec2(u.y,u.x)*(va-vb-vc+vd) + cr::vec2(vb,vc) - cr::vec2(va,va));
    return cr::vec3( va + (vb-va)*u.x + (vc-va)*u.y + (va-vb-vc+vd)*u.x*u.y,
                     der.x, der.y );
}

}
