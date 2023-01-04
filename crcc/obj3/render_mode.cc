#include "render_mode.hh"

#include <cmath>
#include "../math/mconst.hh"

namespace cr {

render_mode::render_mode()
{
    pp4size = 1.2f;
    pp6size = 1.2f;
    
    
    back[0] = 0.07f;
    back[1] = 0.27f;
    back[2] = 0.27f;
    back[3] = 1.0f;
    col[0] = 0.9f;
    col[1] = 0.9f;
    col[2] = 0.9f;
    col[3] = 0.3f;
    
    colourmode = 0;
    
    pointcnt = 6;
    pointsize = 2.0f;
    
    /* (Moire1)
    pointcoords[6] = 0.333333f;  pointcoords[7] = 0.0f;
    pointcoords[8] = 0.0f;       pointcoords[9] = 0.333333f;
    pointcoords[10] = 0.666666f; pointcoords[11] = 0.0f;
    pointcoords[12] = 0.0f;      pointcoords[13] = 0.666666f;
    */
    /*
    pointcoords[0] = 0.16666f;  pointcoords[1] = 0.16666f;
    pointcoords[2] = 0.5f;      pointcoords[3] = 0.16666f;
    pointcoords[4] = 0.83333f;  pointcoords[5] = 0.16666f;
    
    pointcoords[6] = 0.16666f;  pointcoords[7] = 0.5f;
    pointcoords[8] = 0.5f;      pointcoords[9] = 0.5f;
    
    pointcoords[10] = 0.16666f; pointcoords[11] = 0.83333f;
    */
    
    float a = 1.0f / (5.0f + 1.0f/std::tan(22.5f*cr::dtor));
    
    pointcoords[0]  = a;    pointcoords[1]  = a;
    pointcoords[2]  = 3*a;  pointcoords[3]  = a;
    pointcoords[4]  = 5*a;  pointcoords[5]  = a;
    
    pointcoords[6]  = a;    pointcoords[7]  = 3*a;
    pointcoords[8]  = 3*a;  pointcoords[9]  = 3*a;
    
    pointcoords[10] = a;    pointcoords[11] = 5*a;
    
    
    // unused
    pointcoords[12] = 0.0f; pointcoords[13] = 0.0f;
    pointcoords[14] = 0.0f; pointcoords[15] = 0.0f;
    pointcoords[16] = 0.0f; pointcoords[17] = 0.0f;
    pointcoords[18] = 0.0f; pointcoords[19] = 0.0f;
    
    path_n = 100;
}

render_mode::~render_mode() {}

ui_render_mode::ui_render_mode ()
{
    chars = nullptr;
    im = nullptr;
}
ui_render_mode::~ui_render_mode ()
{
    delete[] chars;
    free(im);
}
void ui_render_mode::find (uint32 c, int& x, int& y)
{
    for (int j=1 ; j<char_count ; ++j)
    {
        if (chars[j] == c)
        {
            x = j % im_char_cols;
            y = j / im_char_cols;
            return;
        }
    }
    x = 0;
    y = 0;
}

}
