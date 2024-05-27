
#include "noise.hh"

#include <crcc/crcc.hh>
#include <iostream>


namespace ter {

noise_s::noise_s  () {}
noise_s::~noise_s () {}

float noise_s::noise (cr::vec2 x)
{
    float a = std::cos(x.x) + std::cos(x.y);
    if (a > 5.0f)  a = 5.0f;
    if (a < -5.0f) a = -5.0f;
    return a;
}

float noise_s::fbm (cr::vec2 x, int on, float g)
{
    float f = 1.0;
    float a = 1.0;
    float t = 0.0;
    for (int i=0; i<on; ++i)
    {
        t += a * noise(x*f);
        f *= 2.0;
        a *= g;
    }
    return t;
}

float noise_s::domain_warp (cr::vec2 x, int on, float hh)
{
    
    cr::vec2 q = cr::vec2( fbm( x + cr::vec2(0.0f,0.0f), on, hh ),
                           fbm( x + cr::vec2(5.2f,1.3f), on, hh ) );

    cr::vec2 r  = cr::vec2( fbm( x + q*4.0f + cr::vec2(1.7f,9.2f), on, hh ),
                            fbm( x + q*4.0f + cr::vec2(8.3f,2.8f), on, hh ) );

    float f = fbm( x + r*4.0f, on, hh );
    
    /*
    cr::vec2 q = cr::vec2( noise( x + cr::vec2(0.0f,0.0f) ),
                           noise( x + cr::vec2(5.2f,1.3f) ) );

    cr::vec2 r  = cr::vec2( noise( x + q*4.0f + cr::vec2(1.7f,9.2f) ),
                            noise( x + q*4.0f + cr::vec2(8.3f,2.8f) ) );

    float f = noise( x + r*4.0f );
    */
    return f;
}



noise_val::noise_val()
{
    prex = 100;
    prey = 100;
    pre_rnd = new float[prex*prey];
    
    cr::fdice dice(120, -10.0f, 10.0f);
    for (int x=0 ; x<prex ; ++x)
    {
        for (int y=0 ; y<prey ; ++y)
        {
            pre_rnd[y*prex + x] = dice.next();
            //std::cout << pre_rnd[y*prex + x] << " ";
        }
        //std::cout << std::endl;
    }
}
noise_val::~noise_val()
{
    delete[] pre_rnd;
}

float noise_val::value_noise (float x, float y)
{
    int x1 = int(std::round(x));
    int y1 = int(std::round(y));
    float dx = x - (float)(x1);
    float dy = y - (float)(y1);
    int x2 = (dx > 0) ? x1+1 : x1-1;
    int y2 = (dy > 0) ? y1+1 : y1-1;
            
    x1 = cr::modi(x1, prex);
    x2 = cr::modi(x2, prex);
    y1 = cr::modi(y1, prey);
    y2 = cr::modi(y2, prey);
    
    //if (dx > 1.1f || dx < -1.1f || dy > 1.1f || dy < -1.1f) { std::cout << dx << " " << dy << std::endl; }
    
    if (dx < 0.0f) dx = -dx;
    if (dy < 0.0f) dy = -dy;
    
    return ((1.0f - dx) * pre_rnd[y1*prex+x1] + (dx) * pre_rnd[y1*prex+x2]) * (1.0f - dy) +
           ((1.0f - dx) * pre_rnd[y2*prex+x1] + (dx) * pre_rnd[y2*prex+x2]) * (dy);
}

float noise_val::base (float x, float y)
{
    return value_noise(x/xscale, y/yscale);
}

float noise_val::fbm (float x, float y)
{
    return          base(x, y) +
           0.5f   * base(x/2.0f, y/2.0f) +
           0.25f  * base(x/4.0f, y/4.0f) +
           0.125f * base(x/8.0f, y/8.0f);
}

float noise_val::domain_warp (float x, float y)
{
    /*
    float x2 = fbm(x + 10.0f, y + 22.1f);
    float y2 = fbm(x, y);
    return fbm(x + 4*x2, y + 4*y2);
    */
    
    float x2 = fbm(x + 10.1f, y + 7.9f);
    float y2 = fbm(x, y);
    float x3 = fbm(x + 4*x2 + 10.9f, y + 4*y2 + 9.1f);
    float y3 = fbm(x + 4*x2 + 21.1f, y + 4*y2 + 3.3f);
    return fbm(x + 4*x3, y + 4*y3);
    
}







noise_grad::noise_grad  () {}
noise_grad::~noise_grad () {}

cr::vec2 noise_grad::hash (cr::vec2 p)
{
    cr::vec2 k = cr::vec2( 0.3183099, 0.3678794 );
    cr::vec2 x = p*k + cr::vec2( 0.3678794, 0.3183099 );
    return cr::vec2(-1.0 + 2.0*cr::fract( k.x*16.0*cr::fract( x.x*x.y*(x.x+x.y)) ),
                    -1.0 + 2.0*cr::fract( k.y*16.0*cr::fract( x.x*x.y*(x.x+x.y)) ));
}

cr::vec3 noise_grad::grad_noise (cr::vec2 x)
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

cr::vec3 noise_grad::base (float x, float y)
{
    return grad_noise( cr::vec2(x/xscale, y/yscale) );
}

float noise_grad::fbm (float x, float y, int on, float g)
{
    float f = 1.0;
    float a = 1.0;
    float t = 0.0;
    for (int i=0; i<on; ++i)
    {
        t += a * grad_noise(cr::vec2(f*std::fabs(x), f*std::fabs(y))).x;
        f *= 2.0;
        a *= g;
    }
    return t;
}

float noise_grad::domain_warp (float x, float y, int on, float hh)
{
    cr::vec2 q = cr::vec2( fbm( x+0.0f, y+0.0f, on, hh ),
                           fbm( x+5.2f, y+1.3f, on, hh ) );

    cr::vec2 r1 = cr::vec2(x,y) + q*4.0f + cr::vec2(1.7f,9.2f);
    cr::vec2 r2 = cr::vec2(x,y) + q*4.0f + cr::vec2(8.3f,2.8f);
    cr::vec2 r  = cr::vec2( fbm( r1.x, r1.y, on, hh ),
                            fbm( r2.x, r2.y, on, hh ) );

    cr::vec2 p = cr::vec2(x,y) + r*4.0f;
    float f = fbm( p.x, p.y, on, hh );
    
    /*
    if (f > 200.0f)
    {
        std::cout << "HIGH: val: " << f << ",  x: " << x << ",  y: " << y << ",  q: " << q.str() << ", r: " << r.str() << ",  p: " << p.str() << std::endl;
    }
    */
    
    return f;
}

}
