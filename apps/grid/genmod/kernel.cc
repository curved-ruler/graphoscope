#include "kernel.hh"

#include <cstring>

namespace ter {

void convolve (float* hmap, float* hmap2, int hw, int hh, float* kernel, int kw, int kh, float damp)
{
    int kw2 = kw / 2;
    int kh2 = kh / 2;
    for (int x=0 ; x<hw ; ++x)
    {
        for (int y=0 ; y<hh ; ++y)
        {
            float hnew = 0.0f;
            
            for (int i=0 ; i<kw ; ++i)
            {
                for (int j=0 ; j<kh ; ++j)
                {
                    if ((y-kh2+j >= 0) && (y-kh2+j < hh) && (x-kw2+i >= 0) && (x-kw2+i < hw))
                        hnew += kernel[j*kw + i] * hmap[(y-kh2+j)*hw + (x-kw2+i)];
                }
            }
            if ((y > 0) && (y < hh-1) && (x > 0) && (x < hw-1))
                hmap2[y*hw + x] = hnew * damp;
            else
                hmap2[y*hw + x] = 0.0f;
        }
    }
    
    std::memcpy(hmap, hmap2, hw*hh*sizeof(float));
}

void laplacian1 (float* hmap, float* hmap2, int hw, int hh)
{
    float k[9] = {
        0.0f,  0.5f, 0.0f,
        0.5f, -2.0f, 0.5f,
        0.0f,  0.5f, 0.0f
    };
    convolve(hmap, hmap2, hw, hh, k, 3, 3, 0.9f);
}
void laplacian2 (float* hmap, float* hmap2, int hw, int hh)
{
    float k[9] = {
        0.05f,  0.2f, 0.05f,
        0.2f,  -1.0f, 0.2f,
        0.05f,  0.2f, 0.05f
    };
    convolve(hmap, hmap2, hw, hh, k, 3, 3, 0.9f);
}

}
