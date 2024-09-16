
#pragma once

namespace ter {

void game_of_life           (float* hmap, float* hmap2, int w, int h);
void game_of_life_init      (float* hmap, float* hmap2, int w, int h);
void reaction_diffusion     (float* a, float* a2, float* b, float* b2, int w, int h, float hh);
void reaction_diffusion_init(float* a, float* a2, float* b, float* b2, int w, int h, float hh);

}
