
#pragma once

namespace ter {

class del_noise
{
    private:
        int prex, prey;
        float* pre_rnd;
        
    public:
        float xscale, yscale;
        float xp, yp;
        
        del_noise();
        ~del_noise();
        
        float value_noise (float x, float y);
        float base        (float x, float y);
        float fbm         (float x, float y);
        float domain_warp (float x, float y);
};

}
