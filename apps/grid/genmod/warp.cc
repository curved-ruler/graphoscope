#include "warp.hh"

#include <crcc/crcc.hh>
#include <iostream>


namespace ter {

/*
float used_base_a (float xi, float yi)
{
    return 15.0f * std::cos((xi / 2.0f)*cr::dtor * (yi / 2.0f)*cr::dtor);
}
    
float used_base_b (float xi, float yi)
{
    return 15.0f * std::cos((xi*yi / 100.0f )*cr::dtor);
}

float used_base_c (float xi, float yi)
{
    return 15.0f * std::cos(xi*cr::dtor) * std::cos(yi*cr::dtor);
}
*/

noise::noise()
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
noise::~noise()
{
    delete[] pre_rnd;
}

float noise::value_noise (float x, float y)
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

float noise::base (float x, float y)
{
    return value_noise(x/xscale, y/yscale);
}

float noise::fbm (float x, float y)
{
    return          base(x, y) +
           0.5f   * base(x/2.0f, y/2.0f) +
           0.25f  * base(x/4.0f, y/4.0f) +
           0.125f * base(x/8.0f, y/8.0f);
}

float noise::domain_warp (float x, float y)
{
    /*
    float x2 = fbm(x + 10.0f, y + 22.1f);
    float y2 = fbm(x, y);
    return fbm(x + 4*x2, y + 4*y2);
    */
    
    //return fbm(x,y);
    
    float x2 = fbm(x + 10.1f, y + 7.9f);
    float y2 = fbm(x, y);
    float x3 = fbm(x + 4*x2 + 10.9f, y + 4*y2 + 9.1f);
    float y3 = fbm(x + 4*x2 + 21.1f, y + 4*y2 + 3.3f);
    return fbm(x + 4*x3, y + 4*y3);
    
}

}
