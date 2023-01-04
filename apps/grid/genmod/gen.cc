
#include "gen.hh"

#include <cmath>

namespace ter {

void quantize (float* hmap, int w, int h, float q)
{
    for (int j=0 ; j<h ; ++j)
    {
        for (int i=0 ; i<w ; ++i)
        {
            //std::cout << i << "--" << j << std::endl;
            hmap[j*w+i] = std::floor(hmap[j*w+i] / q + 0.5f) * q;
        }
    }
}

}
