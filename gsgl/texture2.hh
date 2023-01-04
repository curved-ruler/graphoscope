
#pragma once

#include "gsgl_config.hh"


namespace gsgl {


class texture2
{
public:
    GLuint id;
    int w, h;
    GLenum format;
    int ch;
    
public:
    texture2(int _w, int _h, GLenum _f);
    ~texture2();
    
    void setdata(const float* raw_image);
    void getdata(float* raw);
};


}
