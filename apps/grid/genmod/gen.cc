
#include "gen.hh"

#include <cmath>

namespace ter {

void quantize (float* hmap, int w, int h, float q)
{
    for (int j=0 ; j<h ; ++j)
    {
        for (int i=0 ; i<w ; ++i)
        {
            hmap[j*w+i] = std::floor(hmap[j*w+i] / q + 0.5f) * q;
        }
    }
}

void level (float* hmap, int w, int h, float l)
{
    for (int j=0 ; j<h ; ++j)
    {
        for (int i=0 ; i<w ; ++i)
        {
            if (hmap[j*w+i] < l) hmap[j*w+i] = l;
        }
    }
}

}
