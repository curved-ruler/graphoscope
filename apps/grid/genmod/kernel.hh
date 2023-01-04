#pragma once

namespace ter {

void kernel     (float* hmap, float* hmap2, int hw, int hh, float* kernel, int kw, int kk);
void laplacian1 (float* hmap, float* hmap2, int hw, int hh);
void laplacian2 (float* hmap, float* hmap2, int hw, int hh);
    
}
