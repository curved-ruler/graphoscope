#pragma once

#include <string>

#include "../base.hh"

namespace cr {

enum cam_mode
{
    AXON  = 0,
    PERSP = 1,
    FISH  = 2,
    PP4   = 3,
    CM_MAX
};

enum obj_mode
{
    POINTS    = 0, // geometry shader triangle->points
    LINES     = 1,
    FLAT      = 2,
    FLAT_PTS  = 3, // flat + points
    FLAT_LINE = 4,
    OM_MAX
};

enum col_mode
{
    UNIFORM   = 0,
    UNIFORM_A = 1,
    RGB    = 2,
    RGB_A  = 3,
    CO_MAX
};

enum pproc_mode 
{
    PP_NOTHING,
    PP_INVERSE,
    PP_ENBOSS
};

struct rrr_buffers
{
    public:
        rrr_buffers();
        
        bool tnorm, lnorm, pnorm;
        
        uint32 trin;
        uint32 tri_buf;
        uint32 linn;
        uint32 lin_buf;
        uint32 pntn;
        uint32 pnt_buf;
};




struct render_mode
{
public:
    
    std::string shader_dir;
    
    uint8 camtype;
    static const uint8 cam_n = 4;
    uint8 objtype;
    static const uint8 obj_n = 6;
    uint8 proctype;
    static const uint8 pop_n = 3;
    
    
    float back[4];
    float col[4];
    
    int screen_w, screen_h;
    int viewport[4];
    int pixel_size;
    
    // 0 : white
    // 1 : white + alpha
    // 2 : col
    // 3 : col + alpha
    uint8 colourmode;
    static const uint8 col_n = 4;
    float alpha;
    
    float pointsize;
    int pointcnt; // assert <= 10
    float pointcoords[2*10];
    
    float pp4size;
    float pp6size;
    
    int thread_n;
    int frames;  // # of frames for full cpu render
    
public:
    render_mode();
    ~render_mode();
};




struct ui_render_mode
{
public:
    std::string shader_dir;
    std::string font_path;
    std::string font_name;
    
    int char_count;
    uint32* chars;
    
    int im_char_x;
    int im_char_y;
    int im_char_cols;
    int im_char_rows;
    unsigned char* im;
public:
    ui_render_mode();
    ~ui_render_mode();
    
    void find (uint32 c, int& x, int& y);
};

}
