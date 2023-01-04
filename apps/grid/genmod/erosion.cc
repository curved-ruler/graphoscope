#include "erosion.hh"

#include <iostream>
#include <algorithm>
#include <cmath>

namespace ter {

terrain_data_square::terrain_data_square(int _w, int _h)
{
    if (_w <= 0 || _h <= 0)
    {
        std::cerr << "ERROR: terrain data dimensions" << std::endl;
    }
    else
    {
        W = _w; H = _h;
        height = new float[W*H];
        water  = new float[W*H];
        flow   = new cr::vec2[W*H];
        sediment1 = new float[W*H];
        sediment2 = new float[W*H];
        
        clear();
    }
}
terrain_data_square::~terrain_data_square()
{
    delete[] height;
    delete[] water;
    delete[] flow;
    delete[] sediment1;
    delete[] sediment2;
}

void terrain_data_square::clear()
{
    for (int i = 0 ; i < W*H ; ++i)
    {
        //height[i] = (i != W*H / 4) ? 0.0f : 25.0f;
        height[i] = 0.0f;
        water[i]  = 0.0f;
        flow[i].nill();
        sediment1[i] = 0.0f;
        sediment2[i] = 0.0f;
    }
}

void erosion_talus (float* hmap, int hw, int hh, float minslope)
{
    for (int y=0 ; y<hh ; ++y)
    {
        for (int x=0 ; x<hw ; ++x)
        {
            int xto = 0, yto = 0, x2, y2;
            float s = minslope;
            float s2;
            
            x2 = (x-1<0) ? hw-1 : x-1;
            y2 = y;
            s2 = hmap[y*hw+x] - hmap[y2*hw+x2]; if (s2 > s) { s = s2; xto = x2; yto = y2; }
            
            x2 = (x+1>=hw) ? 0 : x+1;
            y2 = y;
            s2 = hmap[y*hw+x] - hmap[y2*hw+x2]; if (s2 > s) { s = s2; xto = x2; yto = y2; }
            
            y2 = (y-1<0) ? hh-1 : y-1;
            x2 = x;
            s2 = hmap[y*hw+x] - hmap[y2*hw+x2]; if (s2 > s) { s = s2; xto = x2; yto = y2; }
            
            y2 = (y+1>=hh) ? 0 : y+1;
            x2 = x;
            s2 = hmap[y*hw+x] - hmap[y2*hw+x2]; if (s2 > s) { s = s2; xto = x2; yto = y2; }
            
            x2 = (x-1<0) ? hw-1 : x-1;
            y2 = (y-1<0) ? hh-1 : y-1;
            s2 = (hmap[y*hw+x] - hmap[y2*hw+x2]) / 1.4142135f; if (s2 > s) { s = s2; xto = x2; yto = y2; }
            //s2 = (hmap[y*hw+x] - hmap[y2*hw+x2]); if (s2 > s) { s = s2; xto = x2; yto = y2; }
            
            x2 = (x-1<0) ? hw-1 : x-1;
            y2 = (y+1>=hh) ? 0 : y+1;
            s2 = (hmap[y*hw+x] - hmap[y2*hw+x2]) / 1.4142135f; if (s2 > s) { s = s2; xto = x2; yto = y2; }
            //s2 = (hmap[y*hw+x] - hmap[y2*hw+x2]); if (s2 > s) { s = s2; xto = x2; yto = y2; }
            
            x2 = (x+1>=hw) ? 0 : x+1;
            y2 = (y-1<0) ? hh-1 : y-1;
            s2 = (hmap[y*hw+x] - hmap[y2*hw+x2]) / 1.4142135f; if (s2 > s) { s = s2; xto = x2; yto = y2; }
            //s2 = (hmap[y*hw+x] - hmap[y2*hw+x2]); if (s2 > s) { s = s2; xto = x2; yto = y2; }
            
            x2 = (x+1>=hw) ? 0 : x+1;
            y2 = (y+1>=hh) ? 0 : y+1;
            s2 = (hmap[y*hw+x] - hmap[y2*hw+x2]) / 1.4142135f; if (s2 > s) { s = s2; xto = x2; yto = y2; }
            //s2 = (hmap[y*hw+x] - hmap[y2*hw+x2]); if (s2 > s) { s = s2; xto = x2; yto = y2; }
            
            
            if (s > minslope)
            {
                float ramount = 50.0f;
                //if (yto != y && xto != x) ramount *= 1.4142135f;
                
                hmap[y*hw+x]     -= std::log(s-minslope / ramount);
                hmap[yto*hw+xto] += std::log(s-minslope / ramount);
            }
        }
    }
}

void erosion_hydraulic (terrain_data_square* map)
{
    float rain = 0.2f;
    float evap = 0.2f;
    //float hmax = 50.0f;
    //float hmin = -50.0f;
    float smax = 2.0f;
    float dt = 1.0f;
    float dwh;
    
    int hw = map->W;
    int hh = map->H;
    
    int sdropped = 0;
    int staken   = 0;
    float vmax   = 0.0f;
    
    for (int y=0 ; y<hh ; ++y)
    {
        for (int x=0 ; x<hw ; ++x)
        {
            map->water[y*hw+x] += rain;
        }
    }
    
    for (int y=0 ; y<hh ; ++y)
    {
        for (int x=0 ; x<hw ; ++x)
        {
            float dv = 0.0f;
            cr::vec2 v(0.0f, 0.0f);
            int x2, y2;
            
            x2 = (x-1<0) ? hw-1 : x-1;
            y2 = y;
            dwh = map->height[y*hw+x] + map->water[y*hw+x] - map->height[y2*hw+x2] - map->water[y2*hw+x2];
            if (dwh > dv) { dv = dwh; v = {-1.0f, 0.0f}; v.setlength(std::min(dwh, map->water[y*hw+x])); }
            
            x2 = (x+1>=hw) ? 0 : x+1;
            y2 = y;
            dwh = map->height[y*hw+x] + map->water[y*hw+x] - map->height[y2*hw+x2] - map->water[y2*hw+x2];
            if (dwh > dv) { dv = dwh; v = {1.0f, 0.0f}; v.setlength(std::min(dwh, map->water[y*hw+x])); }
            
            y2 = (y-1<0) ? hh-1 : y-1;
            x2 = x;
            dwh = map->height[y*hw+x] + map->water[y*hw+x] - map->height[y2*hw+x2] - map->water[y2*hw+x2];
            if (dwh > dv) { dv = dwh; v = {0.0f, -1.0f}; v.setlength(std::min(dwh, map->water[y*hw+x])); }
            
            y2 = (y+1>=hh) ? 0 : y+1;
            x2 = x;
            dwh = map->height[y*hw+x] + map->water[y*hw+x] - map->height[y2*hw+x2] - map->water[y2*hw+x2];
            if (dwh > dv) { dv = dwh; v = {0.0f, 1.0f}; v.setlength(std::min(dwh, map->water[y*hw+x])); }
            
            x2 = (x-1<0) ? hw-1 : x-1;
            y2 = (y-1<0) ? hh-1 : y-1;
            dwh = map->height[y*hw+x] + map->water[y*hw+x] - map->height[y2*hw+x2] - map->water[y2*hw+x2];
            dwh /= 1.4142135f;
            if (dwh > dv) { dv = dwh; v = {-1.0f, -1.0f}; v.setlength(std::min(dwh, map->water[y*hw+x])); }
            
            x2 = (x-1<0) ? hw-1 : x-1;
            y2 = (y+1>=hh) ? 0 : y+1;
            dwh = map->height[y*hw+x] + map->water[y*hw+x] - map->height[y2*hw+x2] - map->water[y2*hw+x2];
            dwh /= 1.4142135f;
            if (dwh > dv) { dv = dwh; v = {-1.0f, 1.0f}; v.setlength(std::min(dwh, map->water[y*hw+x])); }
            
            x2 = (x+1>=hw) ? 0 : x+1;
            y2 = (y-1<0) ? hh-1 : y-1;
            dwh = map->height[y*hw+x] + map->water[y*hw+x] - map->height[y2*hw+x2] - map->water[y2*hw+x2];
            dwh /= 1.4142135f;
            if (dwh > dv) { dv = dwh; v = {1.0f, -1.0f}; v.setlength(std::min(dwh, map->water[y*hw+x])); }
            
            x2 = (x+1>=hw) ? 0 : x+1;
            y2 = (y+1>=hh) ? 0 : y+1;
            dwh = map->height[y*hw+x] + map->water[y*hw+x] - map->height[y2*hw+x2] - map->water[y2*hw+x2];
            dwh /= 1.4142135f;
            if (dwh > dv) { dv = dwh; v = {1.0f, 1.0f}; v.setlength(std::min(dwh, map->water[y*hw+x])); }
            
            float vlen = v.length();
            
            //v.maximize(2.0f);
            if (vlen > vmax) { vmax = vlen; }
            map->flow[y*hw+x] = v;
            
            
            float tresh = 0.1f;
            if (vlen > tresh)
            {
                float ds = vlen * 1.0f;
                ds = std::max(ds, smax - map->sediment1[y*hw+x]);
                
                map->height[y*hw+x]    -= ds;
                //if (map->height[y*hw+x]>hmax) map->height[y*hw+x] = hmax;
                //if (map->height[y*hw+x]<hmin) map->height[y*hw+x] = hmin;
                map->sediment1[y*hw+x] += ds;
                
                //if (map->sediment1[y*hw+x] < 0.0f) map->sediment1[y*hw+x] = 0.0f;
                //if (map->sediment1[y*hw+x] > smax) map->sediment1[y*hw+x] = smax;
                
                ++staken;
            }
            else
            {
                float ds = ( map->sediment1[y*hw+x] * (tresh - vlen) ) * 1.0f;
                ds = std::min(ds, map->sediment1[y*hw+x]);
                
                map->height[y*hw+x]    += ds;
                //if (map->height[y*hw+x]>hmax) map->height[y*hw+x] = hmax;
                //if (map->height[y*hw+x]<hmin) map->height[y*hw+x] = hmin;
                map->sediment1[y*hw+x] -= ds;
                
                //if (map->sediment1[y*hw+x] < 0.0f) map->sediment1[y*hw+x] = 0.0f;
                //if (map->sediment1[y*hw+x] > smax) map->sediment1[y*hw+x] = smax;
                
                ++sdropped;
            }
        }
    }
    std::cout << "DROP: " << sdropped << ",  TAKE: " << staken << ", VMAX: " << vmax << std::endl;
    
    
    int x1, x2, y1, y2;
    float sx, sy, dx, dy;
    //float wt;
    for (int y=0 ; y<hh ; ++y)
    {
        for (int x=0 ; x<hw ; ++x)
        {
            sx = x - (map->flow[y*hw+x]).x * dt;
            sy = y - (map->flow[y*hw+x]).y * dt;
            x1 = int(std::round(sx));
            y1 = int(std::round(sy));
            dx = sx - (float)(x1);
            dy = sy - (float)(y1);
            x2 = (dx > 0) ? x1+1 : x1-1;
            y2 = (dy > 0) ? y1+1 : y1-1;
            
            x1 = cr::modi(x1, hw);
            x2 = cr::modi(x2, hw);
            y1 = cr::modi(y1, hh);
            y2 = cr::modi(y2, hh);
            
            if (dx < 0.0f) dx = -dx;
            if (dy < 0.0f) dy = -dy;
            map->sediment2[y*hw+x] = ((1.0f - dx)*map->sediment1[y1*hw+x1] + (dx)*map->sediment1[y1*hw+x2]) * (1.0f - dy) +
                                     ((1.0f - dx)*map->sediment1[y2*hw+x1] + (dx)*map->sediment1[y2*hw+x2]) * (dy);
            
            /*
            x2 = (x-1<0) ? hw-1 : x-1;
            y2 = y;
            wt = map->height[y*hw+x] + map->water[y*hw+x] - map->height[y2*hw+x2] - map->water[y2*hw+x2];
            wt = (wt > 0.0f) ? wt*tube*map->water[y*hw+x] : wt*tube*map->water[y2*hw+x2];
            if (wt > 0.0f)
            {
                if (map->water[y*hw+x] >= wt)
                {
                    map->water[y*hw+x] -= wt;
                    map->water[y2*hw+x2] += wt;
                }
            }
            else
            {
                if (map->water[y2*hw+x2] >= -wt)
                {
                    map->water[y*hw+x] -= wt;
                    map->water[y2*hw+x2] += wt;
                }
            }
            
            x2 = (x+1>=hw) ? 0 : x+1;
            y2 = y;
            wt = map->height[y*hw+x] + map->water[y*hw+x] - map->height[y2*hw+x2] - map->water[y2*hw+x2];
            wt = (wt > 0.0f) ? wt*tube*map->water[y*hw+x] : wt*tube*map->water[y2*hw+x2];
            if (wt > 0.0f)
            {
                if (map->water[y*hw+x] >= wt)
                {
                    map->water[y*hw+x] -= wt;
                    map->water[y2*hw+x2] += wt;
                }
            }
            else
            {
                if (map->water[y2*hw+x2] >= -wt)
                {
                    map->water[y*hw+x] -= wt;
                    map->water[y2*hw+x2] += wt;
                }
            }
            
            y2 = (y-1<0) ? hh-1 : y-1;
            x2 = x;
            wt = map->height[y*hw+x] + map->water[y*hw+x] - map->height[y2*hw+x2] - map->water[y2*hw+x2];
            wt = (wt > 0.0f) ? wt*tube*map->water[y*hw+x] : wt*tube*map->water[y2*hw+x2];
            if (wt > 0.0f)
            {
                if (map->water[y*hw+x] >= wt)
                {
                    map->water[y*hw+x] -= wt;
                    map->water[y2*hw+x2] += wt;
                }
            }
            else
            {
                if (map->water[y2*hw+x2] >= -wt)
                {
                    map->water[y*hw+x] -= wt;
                    map->water[y2*hw+x2] += wt;
                }
            }
            
            y2 = (y+1>=hh) ? 0 : y+1;
            x2 = x;
            wt = map->height[y*hw+x] + map->water[y*hw+x] - map->height[y2*hw+x2] - map->water[y2*hw+x2];
            wt = (wt > 0.0f) ? wt*tube*map->water[y*hw+x] : wt*tube*map->water[y2*hw+x2];
            if (wt > 0.0f)
            {
                if (map->water[y*hw+x] >= wt)
                {
                    map->water[y*hw+x] -= wt;
                    map->water[y2*hw+x2] += wt;
                }
            }
            else
            {
                if (map->water[y2*hw+x2] >= -wt)
                {
                    map->water[y*hw+x] -= wt;
                    map->water[y2*hw+x2] += wt;
                }
            }
            
            x2 = (x-1<0) ? hw-1 : x-1;
            y2 = (y-1<0) ? hh-1 : y-1;
            wt = (map->height[y*hw+x] + map->water[y*hw+x] - map->height[y2*hw+x2] - map->water[y2*hw+x2]) / 1.4142135f;
            wt = (wt > 0.0f) ? wt*tube*map->water[y*hw+x] : wt*tube*map->water[y2*hw+x2];
            if (wt > 0.0f)
            {
                if (map->water[y*hw+x] >= wt)
                {
                    map->water[y*hw+x] -= wt;
                    map->water[y2*hw+x2] += wt;
                }
            }
            else
            {
                if (map->water[y2*hw+x2] >= -wt)
                {
                    map->water[y*hw+x] -= wt;
                    map->water[y2*hw+x2] += wt;
                }
            }
            
            x2 = (x-1<0) ? hw-1 : x-1;
            y2 = (y+1>=hh) ? 0 : y+1;
            wt = (map->height[y*hw+x] + map->water[y*hw+x] - map->height[y2*hw+x2] - map->water[y2*hw+x2]) / 1.4142135f;
            wt = (wt > 0.0f) ? wt*tube*map->water[y*hw+x] : wt*tube*map->water[y2*hw+x2];
            if (wt > 0.0f)
            {
                if (map->water[y*hw+x] >= wt)
                {
                    map->water[y*hw+x] -= wt;
                    map->water[y2*hw+x2] += wt;
                }
            }
            else
            {
                if (map->water[y2*hw+x2] >= -wt)
                {
                    map->water[y*hw+x] -= wt;
                    map->water[y2*hw+x2] += wt;
                }
            }
            
            x2 = (x+1>=hw) ? 0 : x+1;
            y2 = (y-1<0) ? hh-1 : y-1;
            wt = (map->height[y*hw+x] + map->water[y*hw+x] - map->height[y2*hw+x2] - map->water[y2*hw+x2]) / 1.4142135f;
            wt = (wt > 0.0f) ? wt*tube*map->water[y*hw+x] : wt*tube*map->water[y2*hw+x2];
            if (wt > 0.0f)
            {
                if (map->water[y*hw+x] >= wt)
                {
                    map->water[y*hw+x] -= wt;
                    map->water[y2*hw+x2] += wt;
                }
            }
            else
            {
                if (map->water[y2*hw+x2] >= -wt)
                {
                    map->water[y*hw+x] -= wt;
                    map->water[y2*hw+x2] += wt;
                }
            }
            
            x2 = (x+1>=hw) ? 0 : x+1;
            y2 = (y+1>=hh) ? 0 : y+1;
            wt = (map->height[y*hw+x] + map->water[y*hw+x] - map->height[y2*hw+x2] - map->water[y2*hw+x2]) / 1.4142135f;
            wt = (wt > 0.0f) ? wt*tube*map->water[y*hw+x] : wt*tube*map->water[y2*hw+x2];
            if (wt > 0.0f)
            {
                if (map->water[y*hw+x] >= wt)
                {
                    map->water[y*hw+x] -= wt;
                    map->water[y2*hw+x2] += wt;
                }
            }
            else
            {
                if (map->water[y2*hw+x2] >= -wt)
                {
                    map->water[y*hw+x] -= wt;
                    map->water[y2*hw+x2] += wt;
                }
            }
            */
        }
    }
    
    for (int y=0 ; y<hh ; ++y)
    {
        for (int x=0 ; x<hw ; ++x)
        {
            map->sediment1[y*hw+x] = map->sediment2[y*hw+x];
        }
    }
    
    for (int y=0 ; y<hh ; ++y)
    {
        for (int x=0 ; x<hw ; ++x)
        {
            map->water[y*hw+x] -= evap;
            if (map->water[y*hw+x] < 0.0f) map->water[y*hw+x] = 0.0f;
        }
    }
}

void erosion_hydraulic_b (terrain_data_square* map)
{
    float rain = 0.2f;
    float evap = rain;
    
    int hw = map->W;
    int hh = map->H;
    
    for (int y=0 ; y<hh ; ++y)
    {
        for (int x=0 ; x<hw ; ++x)
        {
            map->water[y*hw+x] += rain;
        }
    }
    
    //int xx1, xx2, yy1, yy2;
    //float sx, sy, dx, dy;
    
    for (int y=0 ; y<hh ; ++y)
    {
        for (int x=0 ; x<hw ; ++x)
        {
            /*
            sx = x - (map->flow[y*hw+x]).x * dt;
            sy = y - (map->flow[y*hw+x]).y * dt;
            xx1 = std::round(sx);
            yy1 = std::round(sy);
            dx = sx - (float)(xx1);
            dy = sy - (float)(yy1);
            xx2 = (dx > 0) ? xx1+1 : xx1-1;
            yy2 = (dy > 0) ? yy1+1 : yy1-1;
            
            xx1 = cr::modi(xx1, hw);
            xx2 = cr::modi(xx2, hw);
            yy1 = cr::modi(yy1, hh);
            yy2 = cr::modi(yy2, hh);
            
            if (dx < 0.0f) dx = -dx;
            if (dy < 0.0f) dy = -dy;
            map->sediment2[y*hw+x] = ((1.0f - dx)*map->sediment1[yy1*hw+xx1] + (dx)*map->sediment1[yy1*hw+xx2]) * (1.0f - dy) +
                                     ((1.0f - dx)*map->sediment1[yy2*hw+xx1] + (dx)*map->sediment1[yy2*hw+xx2]) * (dy);
            */
            
            int x2, y2, x2c, y2c;
            float dh, dhw, dhwc, wt, ht;
            
            x2c = (x-1<0) ? hw-1 : x-1;
            y2c = y;
            dhwc = map->height[y*hw+x] + map->water[y*hw+x] - map->height[y2c*hw+x2c] - map->water[y2c*hw+x2c];

            
            x2 = (x+1>=hw) ? 0 : x+1;
            y2 = y;
            dhw = map->height[y*hw+x] + map->water[y*hw+x] - map->height[y2*hw+x2] - map->water[y2*hw+x2];
            if (dhw > dhwc) { x2c = x2; y2c = y2; dhwc = dhw; }

            
            y2 = (y-1<0) ? hh-1 : y-1;
            x2 = x;
            dhw = map->height[y*hw+x] + map->water[y*hw+x] - map->height[y2*hw+x2] - map->water[y2*hw+x2];
            if (dhw > dhwc) { x2c = x2; y2c = y2; dhwc = dhw; }

            
            y2 = (y+1>=hh) ? 0 : y+1;
            x2 = x;
            dhw = map->height[y*hw+x] + map->water[y*hw+x] - map->height[y2*hw+x2] - map->water[y2*hw+x2];
            if (dhw > dhwc) { x2c = x2; y2c = y2; dhwc = dhw; }

            
            x2 = (x-1<0) ? hw-1 : x-1;
            y2 = (y-1<0) ? hh-1 : y-1;
            dhw = (map->height[y*hw+x] + map->water[y*hw+x] - map->height[y2*hw+x2] - map->water[y2*hw+x2]);
            if (dhw > dhwc) { x2c = x2; y2c = y2; dhwc = dhw; }

            
            x2 = (x-1<0) ? hw-1 : x-1;
            y2 = (y+1>=hh) ? 0 : y+1;
            dhw = (map->height[y*hw+x] + map->water[y*hw+x] - map->height[y2*hw+x2] - map->water[y2*hw+x2]);
            if (dhw > dhwc) { x2c = x2; y2c = y2; dhwc = dhw; }

            
            x2 = (x+1>=hw) ? 0 : x+1;
            y2 = (y-1<0) ? hh-1 : y-1;
            dhw = (map->height[y*hw+x] + map->water[y*hw+x] - map->height[y2*hw+x2] - map->water[y2*hw+x2]);
            if (dhw > dhwc) { x2c = x2; y2c = y2; dhwc = dhw; }

            
            x2 = (x+1>=hw) ? 0 : x+1;
            y2 = (y+1>=hh) ? 0 : y+1;
            dhw = (map->height[y*hw+x] + map->water[y*hw+x] - map->height[y2*hw+x2] - map->water[y2*hw+x2]);
            if (dhw > dhwc) { x2c = x2; y2c = y2; dhwc = dhw; }
            
            if (dhwc > 0.05f)
            {
                dh = (map->height[y*hw+x] - map->height[y2c*hw+x2c]);
                wt = std::min(dhwc * 0.1f, map->water[y*hw+x]);
                ht = std::min(wt / 2.0f, std::abs(dh));
                //if (dh > 0.0f)
                //{
                    map->height[y*hw+x]     -= ht;
                    map->height[y2c*hw+x2c] += ht;
                //}
                map->water[y*hw+x]     -= wt;
                map->water[y2c*hw+x2c] -= wt;
            }
            
            //map->flow[y*hw+x] = v;
            
            //float magn = v.length();
            //v.normalize();
            
            //float C  = 0.1f * v.length();
            //float sd = C - map->sediment2[y*hw+x];
            
            //map->height[y*hw+x]    -= sd * 0.1f;
            //map->sediment1[y*hw+x] += sd * 0.1f;
            
            //map->height[y*hw+x]    += (map->sediment2[y*hw+x] - C) * 0.2f;
            //map->sediment1[y*hw+x] = map->sediment2[y*hw+x] - (map->sediment2[y*hw+x] - C) * 0.2f;
            //if (map->sediment1[y*hw+x] > 0.3f) map->sediment1[y*hw+x] = 0.3f;
            //if (map->sediment1[y*hw+x] < 0.0f) map->sediment1[y*hw+x] = 0.0f;
            
            
            /*
            v0.x = x;
            v0.y = y;
            v += v0;
            x2 = cr::modi((int)(v.x), hw);
            y2 = cr::modi((int)(v.y), hh);
            map->height[y*hw+x]   -= flow*magn;
            map->height[y2*hw+x2] += flow*magn;
            */
        }
    }
    
    for (int y=0 ; y<hh ; ++y)
    {
        for (int x=0 ; x<hw ; ++x)
        {
            map->water[y*hw+x] -= evap;
            if (map->water[y*hw+x] < 0.0f) map->water[y*hw+x] = 0.0f;
        }
    }
}

}
