#pragma once

#include "../math/mu.hh"

namespace ter {

void ds_square_grid (float* hmap, int partsize, int pw, int ph);
void ds_square      (float* hmap, int hw, int hh, int dsw, int ulx, int uly, cr::fdice& dice);

}
