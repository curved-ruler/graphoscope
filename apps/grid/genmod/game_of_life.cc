
#include "game_of_life.hh"

#include <cstring>
#include <cmath>


namespace ter {

float gof_func (float o, float sum)
{
    /*
    float param0 =  2.0;
    float param1 =  8.0;
    float param2 =  1.0f;
    float param3 =  1.0f;
    
    float R = 0.0f;
    if (param0+0.5f <= sum && sum <= param1-0.5f) R = 1.0f;
    else if (param0-0.5f <  sum && sum <  param0+0.5f) R = sum-param0+0.5f;
    else if (param1-0.5f <  sum && sum <  param1+0.5f) R = param1-sum+0.5f;
    
    float D = 0.0f;
    if (param2+0.5f <= sum && sum <= param3-0.5f) R = 1.0f;
    else if (param2-0.5f <  sum && sum <  param2+0.5f) R = sum-param2+0.5f;
    else if (param3-0.5f <  sum && sum <  param3+0.5f) R = param3-sum+0.5f;
    
    return o*R + (1-o)*D;
    */
    return -o+(sum / 8.0f);
}

void game_of_life (float* hmap, float* hmap2, int w, int h)
{
    for (int x=1 ; x<w-1 ; ++x)
    {
        for (int y=1 ; y<h-1 ; ++y)
        {
            float dmp = 40.0f;
            
            //float _o = (hmap[y*w + x]     / dmp) + 0.5f;
            float _a = (hmap[(y-1)*w + x] / dmp) + 0.5f;
            float _b = (hmap[(y+1)*w + x] / dmp) + 0.5f;
            float _c = (hmap[y*w + (x-1)] / dmp) + 0.5f;
            float _d = (hmap[y*w + (x+1)] / dmp) + 0.5f;
            
            float _e = (hmap[(y-1)*w + (x-1)] / dmp) + 0.5f;
            float _f = (hmap[(y-1)*w + (x+1)] / dmp) + 0.5f;
            float _g = (hmap[(y+1)*w + (x-1)] / dmp) + 0.5f;
            float _h = (hmap[(y+1)*w + (x+1)] / dmp) + 0.5f;
            
            //hmap2[y*w + x] = dmp * (std::sqrt(std::fabs(o)) * std::pow(std::fabs(a*d+b*c), 0.125f));
            //hmap2[y*w + x] = dmp * (std::sqrt(std::fabs(o)) * (a*d+b*c));
            //hmap2[y*w + x] = dmp * (gof_func(_o, (_a+_b+_c+_d+_e+_f+_g+_h)) - 0.5f);
            
            float _s = -_a*std::log(std::fabs(_a));
            _s       -= _b*std::log(std::fabs(_b));
            _s       -= _c*std::log(std::fabs(_c));
            _s       -= _d*std::log(std::fabs(_d));
            _s       -= _e*std::log(std::fabs(_e));
            _s       -= _f*std::log(std::fabs(_f));
            _s       -= _g*std::log(std::fabs(_g));
            _s       -= _h*std::log(std::fabs(_h));
            hmap2[y*w + x] = dmp * (_s / 4.0f);
            
            /*
            float _D = 1.0f;
            float _K = 0.079f;
            float _L = _o*-1.0f + (_a+_b+_c+_d)*0.2f + (_e+_f+_g+_h)*0.05f;
            float _N = _o + _D*_L - _o*(1.0f-_o)*(1.0f-_o) + _K*(1.0f-_o);
            hmap2[y*w + x] = dmp * (_N - 0.5f);
            */
            
            if (hmap2[y*w + x] >  40.0f) hmap2[y*w + x] -= 40.0f;
            if (hmap2[y*w + x] < -40.0f) hmap2[y*w + x] += 40.0f;
        }
    }
    
    std::memcpy(hmap, hmap2, w*h*sizeof(float));
}

void game_of_life_init (float* hmap, float* hmap2, int w, int h)
{
    uint32 rnd = 83;
    for (int x=1 ; x<w-1 ; ++x)
    {
        for (int y=1 ; y<h-1 ; ++y)
        {
            // xorshift
            rnd ^= rnd << 13;
            rnd ^= rnd >> 17;
            rnd ^= rnd << 5;
            
            hmap[y*w + x]  = (rnd % 111) ? 40.0f : 0.0f;
            hmap2[y*w + x] = (rnd % 111) ? 40.0f : 0.0f;
        }
    }
    for (int x=0 ; x<w ; ++x)
    {
        hmap[0*w + x]      = 0.f;
        hmap[(h-1)*w + x]  = 0.f;
        hmap2[0*w + x]     = 0.f;
        hmap2[(h-1)*w + x] = 0.f;
    }
    for (int y=0 ; y<h ; ++y)
    {
        hmap[y*w + 0]      = 0.f;
        hmap[y*w + (w-1)]  = 0.f;
        hmap2[y*w + 0]     = 0.f;
        hmap2[y*w + (w-1)] = 0.f;
    }
}

void reaction_diffusion (float* a, float* a2, float* b, float* b2, int w, int h, float hh)
{
    for (int x=1 ; x<w-1 ; ++x)
    {
        for (int y=1 ; y<h-1 ; ++y)
        {
            float dmp = 2.0f * hh;
            
            float _ao = (a[y*w + x] / dmp) + 0.5f;
            float _aa = (a[(y-1)*w + x] / dmp) + 0.5f;
            float _ab = (a[(y+1)*w + x] / dmp) + 0.5f;
            float _ac = (a[y*w + (x-1)] / dmp) + 0.5f;
            float _ad = (a[y*w + (x+1)] / dmp) + 0.5f;
            float _ae = (a[(y-1)*w + (x-1)] / dmp) + 0.5f;
            float _af = (a[(y-1)*w + (x+1)] / dmp) + 0.5f;
            float _ag = (a[(y+1)*w + (x-1)] / dmp) + 0.5f;
            float _ah = (a[(y+1)*w + (x+1)] / dmp) + 0.5f;
            
            float _bo = (b[y*w + x] / dmp) + 0.5f;
            float _ba = (b[(y-1)*w + x] / dmp) + 0.5f;
            float _bb = (b[(y+1)*w + x] / dmp) + 0.5f;
            float _bc = (b[y*w + (x-1)] / dmp) + 0.5f;
            float _bd = (b[y*w + (x+1)] / dmp) + 0.5f;
            float _be = (b[(y-1)*w + (x-1)] / dmp) + 0.5f;
            float _bf = (b[(y-1)*w + (x+1)] / dmp) + 0.5f;
            float _bg = (b[(y+1)*w + (x-1)] / dmp) + 0.5f;
            float _bh = (b[(y+1)*w + (x+1)] / dmp) + 0.5f;
            
            float _DA = 1.0f;
            float _DB = 0.5f;
            float _K  = 0.062f;
            float _F  = 0.0545f;
            float _LA = _ao*(-1.0f) + (_aa+_ab+_ac+_ad)*0.2f + (_ae+_af+_ag+_ah)*0.05f;
            float _LB = _bo*(-1.0f) + (_ba+_bb+_bc+_bd)*0.2f + (_be+_bf+_bg+_bh)*0.05f;
            float _NA = _ao + _DA*_LA - _ao*_bo*_bo + _F*(1.0f-_ao);
            float _NB = _bo + _DB*_LB + _ao*_bo*_bo - (_K+_F)*_bo;
            
            a2[y*w + x] = dmp * (_NA - 0.5f);
            b2[y*w + x] = dmp * (_NB - 0.5f);
            if (a2[y*w + x] >  hh) a2[y*w + x] =  hh;
            if (a2[y*w + x] < -hh) a2[y*w + x] = -hh;
            if (b2[y*w + x] >  hh) b2[y*w + x] =  hh;
            if (b2[y*w + x] < -hh) b2[y*w + x] = -hh;
        }
    }
    
    std::memcpy(a, a2, w*h*sizeof(float));
    std::memcpy(b, b2, w*h*sizeof(float));
}

void reaction_diffusion_init (float* a, float* a2, float* b, float* b2, int w, int h, float hh)
{
    //uint32 rnd = 83;
    for (int x=1 ; x<w-1 ; ++x)
    {
        for (int y=1 ; y<h-1 ; ++y)
        {
            // xorshift
            //rnd ^= rnd << 13;
            //rnd ^= rnd >> 17;
            //rnd ^= rnd << 5;
            bool seed = (x > 200 && x < 220) && (y > 200 && y < 220);
            
            a[y*w + x]  = (seed) ?   hh : -hh;
            b[y*w + x]  = (seed) ?  -hh :  hh;
            a2[y*w + x] = (seed) ?   hh : -hh;
            b2[y*w + x] = (seed) ?  -hh :  hh;
        }
    }
    /*
    for (int x=0 ; x<w ; ++x)
    {
        a[0*w + x]     =  40.f;
        a[(h-1)*w + x] =  40.f;
        b[0*w + x]     = -40.f;
        b[(h-1)*w + x] = -40.f;
        
        a2[0*w + x]     =  40.f;
        a2[(h-1)*w + x] =  40.f;
        b2[0*w + x]     = -40.f;
        b2[(h-1)*w + x] = -40.f;
    }
    for (int y=0 ; y<h ; ++y)
    {
        a[y*w + 0]     =  40.f;
        a[y*w + (w-1)] =  40.f;
        b[y*w + 0]     = -40.f;
        b[y*w + (w-1)] = -40.f;
        
        a2[y*w + 0]     =  40.f;
        a2[y*w + (w-1)] =  40.f;
        b2[y*w + 0]     = -40.f;
        b2[y*w + (w-1)] = -40.f;
    }
    */
}

}
