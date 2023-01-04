
#include "texture2.hh"

#include <cstring>


namespace gsgl {


texture2::texture2(int _w, int _h, GLenum _f) : w(_w), h(_h), format(_f)
{
    ch = format == GL_RGB ? 3 : format == GL_RGBA ? 4 : 1;
    
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}
texture2::~texture2()
{
    glDeleteTextures(1, &id);
}

void texture2::setdata(const float* raw_image)
{
    //std::memcpy(data, raw_image, w*h*ch*sizeof(float));
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_FLOAT, raw_image);
}
void texture2::getdata(float* raw)
{
    glGetTextureImage(id, 0, format, GL_FLOAT, w*h*ch, raw);
}


}
