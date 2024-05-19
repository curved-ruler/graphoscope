
#pragma once

#include <crcc/crcc.hh>

namespace ter {

class noise_s
{
    public:
        float xscale, yscale;
        
        noise_s();
        ~noise_s();
        
        float noise       (cr::vec2 x);
        float fbm         (cr::vec2 x, int on, float hh);
        float domain_warp (cr::vec2 x, int on, float hh);
        
        //float fill(float* hmap, int hw, int hh);
};

class noise_val
{
    private:
        int prex, prey;
        float* pre_rnd;
        
    public:
        float xscale, yscale;
        
        noise_val();
        ~noise_val();
        
        float value_noise (float x, float y);
        float base        (float x, float y);
        float fbm         (float x, float y);
        float domain_warp (float x, float y);
        
        //float fill(float* hmap, int hw, int hh);
};

class noise_grad
{
    public:
        float xscale, yscale;
        
        noise_grad();
        ~noise_grad();
        
        cr::vec2 hash (cr::vec2 p);
        
        cr::vec3 grad_noise (cr::vec2 x);
        cr::vec3 base     (float x, float y);
        float fbm         (float x, float y, int on, float hh);
        float domain_warp (float x, float y, int on, float hh);
        
        //float fill(float* hmap, int hw, int hh);
};

}
