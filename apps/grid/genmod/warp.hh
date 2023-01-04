#pragma once

#include <crcc/crcc.hh>

namespace ter {

class noise
{
    private:
        int prex, prey;
        float* pre_rnd;
        
    public:
        float xscale, yscale;
        
        noise();
        ~noise();
        
        float value_noise (float x, float y);
        float base        (float x, float y);
        float fbm         (float x, float y);
        float domain_warp (float x, float y);
        
        //float fill(float* hmap, int hw, int hh);
};

}
