
#include "del_noise.hh"

#include <crcc/crcc.hh>
#include <iostream>
#include <vector>


namespace ter {


del_noise::del_noise()
{
    prex = 768;
    pre_rnd = new float[prex*prex];
    
    cr::fdice dice (120,   0.0f, 2.0f*cr::pi);
    cr::fdice dicer(110,   0.0f, (prex-4) / 2.0f);
    cr::fdice dice2(320, -10.0f, 10.0f);
    std::vector<cr::vec2> pts;
    std::vector<float>    pt_value;
    std::vector<int>      tris;
    
    cr::vec2 p0;
    p0.x = -1;     p0.y = -1;
    pts.push_back(p0);
    p0.x = -1;     p0.y = prex+1;
    pts.push_back(p0);
    p0.x = prex+1; p0.y = prex+1;
    pts.push_back(p0);
    p0.x = prex+1; p0.y = -1;
    pts.push_back(p0);
    
    pt_value.push_back(0);
    pt_value.push_back(0);
    pt_value.push_back(0);
    pt_value.push_back(0);
    
    for (int i=0 ; i<70 ; ++i)
    {
        cr::vec2 p;
        float a = dice.next();
        float r = dicer.next();
        p.x = (r * std::cos(a)) + (prex/2);
        p.y = (r * std::sin(a)) + (prex/2);
        float pf;
        pf = dice2.next();
        pts.push_back(p);
        pt_value.push_back(pf);
    }
    
    delaunay(pts, tris);
    
    for (int x=0 ; x<prex ; ++x)
    {
        for (int y=0 ; y<prex ; ++y)
        {
            float val = 0.0f;
            
            cr::vec2 pxy(x, y);
            
            for (size_t ti=0 ; ti<tris.size()/3 ; ++ti)
            {
                cr::vec2 va = pts[ tris[ti*3] ];
                cr::vec2 vb = pts[ tris[ti*3+1] ];
                cr::vec2 vc = pts[ tris[ti*3+2] ];
            
                if ( cr::tri_inside(va, vb, vc, pxy) )
                {
                    cr::vec3 bary = barycentric(va, vb, vc, pxy);
                    /*
                    val = bary.x * pt_value[ tris[ti*3] ] +
                          bary.y * pt_value[ tris[ti*3+1] ] +
                          bary.z * pt_value[ tris[ti*3+2] ];
                    */
                    val = 30.0f * std::max(bary.x, std::max(bary.y, bary.z)) - 15.0f;
                    break;
                }
            }
            pre_rnd[y*prex + x] = val;
        }
    }
}
del_noise::~del_noise()
{
    delete[] pre_rnd;
}

float del_noise::value_noise (float x, float y)
{
    int x1 = int(std::round(x));
    int y1 = int(std::round(y));
            
    x1 = cr::modi(x1, prex);
    y1 = cr::modi(y1, prex);
    
    return pre_rnd[y1*prex + x1];
}

float del_noise::base (float x, float y)
{
    return value_noise(x/xscale-xp, y/yscale-yp);
}

float del_noise::fbm (float x, float y)
{
    return          base(x, y) +
           0.5f   * base(x*2.0f, y*2.0f) +
           0.25f  * base(x*4.0f, y*4.0f) +
           0.125f * base(x*8.0f, y*8.0f);
}

float del_noise::domain_warp (float x, float y)
{
    /*
    float x2 = fbm(x + 10.0f, y + 22.1f);
    float y2 = fbm(x, y);
    return fbm(x + 4*x2, y + 4*y2);
    */
    
    
    float x2 = base(x + 10.1f, y + 7.9f);
    float y2 = base(x, y);
    float x3 = base(x + 4*x2 + 10.9f, y + 4*y2 + 9.1f);
    float y3 = base(x + 4*x2 + 21.1f, y + 4*y2 + 3.3f);
    return base(x + 4*x3, y + 4*y3);
    
    /*
    float x2 = fbm(x + 10.1f, y + 7.9f);
    float y2 = fbm(x + 10.1f, y + 7.9f);
    float x3 = fbm(x + 4*x2 + 10.9f, y + 4*y2 + 9.1f);
    float y3 = fbm(x + 4*x2 + 10.9f, y + 4*y2 + 9.1f);
    return fbm(x + 4*x3, y + 4*y3);
    */
}

}
