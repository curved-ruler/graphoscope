
#include "textarea.hh"

#include <iostream>

namespace gs {

textarea::textarea()
{
    settings = nullptr;
    rcsize = 2*3*(2+2);
}
textarea::~textarea()
{
    
}
void textarea::setup(cr::ui_render_mode* s)
{
    settings  = s;
    startposx = 0;
    startposy = 0;
}
void textarea::setdims (float _pixsize, float _xsep, float _ysep, int _w, int _h, bool _wrap)
{
    pixsize  = _pixsize;
    charw    = pixsize * settings->im_char_x;
    charh    = pixsize * settings->im_char_y;
    xsep     = _xsep;
    ysep     = _ysep;
    w        = _w;
    h        = _h;
    wordwrap = _wrap;
    
    xnc = int(std::floor(w / (charw + xsep)));
    ync = int(std::floor(h / (charh + ysep)));
    
    renderdata.clear();
    renderdata.resize(xnc*ync*rcsize);
    for (int i=0 ; i<ync ; ++i)
    {
        for (int j=0 ; j<xnc ; ++j)
        {
            renderdata[(i*xnc+j)*rcsize +  0] = (j)*(charw + xsep);
            renderdata[(i*xnc+j)*rcsize +  1] = (i)*(charh + ysep);
            
            renderdata[(i*xnc+j)*rcsize +  4] = (j)*(charw + xsep);
            renderdata[(i*xnc+j)*rcsize +  5] = (i)*(charh + ysep) + charh;
            
            renderdata[(i*xnc+j)*rcsize +  8] = (j)*(charw + xsep) + charw;
            renderdata[(i*xnc+j)*rcsize +  9] = (i)*(charh + ysep);
            
            renderdata[(i*xnc+j)*rcsize + 12] = (j)*(charw + xsep) + charw;
            renderdata[(i*xnc+j)*rcsize + 13] = (i)*(charh + ysep) + charh;
            
            renderdata[(i*xnc+j)*rcsize + 16] = (j)*(charw + xsep) + charw;
            renderdata[(i*xnc+j)*rcsize + 17] = (i)*(charh + ysep);
            
            renderdata[(i*xnc+j)*rcsize + 20] = (j)*(charw + xsep);
            renderdata[(i*xnc+j)*rcsize + 21] = (i)*(charh + ysep) + charh;
        }
    }
    
    colors.clear();
    colors.resize(xnc*ync*6);
    uint32 x = 120;
    for (int i=0 ; i<xnc*ync ; ++i)
    {
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        colors[i*6+0] = x;
        colors[i*6+1] = x;
        colors[i*6+2] = x;
        colors[i*6+3] = x;
        colors[i*6+4] = x;
        colors[i*6+5] = x;
    }
}

void textarea::settext (const std::string& txt)
{
    startposx = 0;
    startposy = 0;
    text = txt;
    calc_linedata();
    
    render();
}
void textarea::calc_linedata()
{
    size_t pos = 0;
    uint32 ci = 0;
    while (pos < text.size())
    {
        ci = cr::next_unicode(text, pos);
        if (ci == chars::eol) {  linedata.linestarts.push_back(pos); }
    }
}

void textarea::scrolly (int y)
{
    size_t n = linedata.linestarts.size();
    
    if (y < 0 && size_t(-y) > startposy)
    {
        startposy = 0;
        return;
    }
    if (y > 0 && y+startposy >= n)
    {
        startposy = n > 0 ? n-1 : 0;
        return;
    }
    
    startposy += y;
}

void textarea::render()
{
    int cx;
    int cy;
    uint32 ci = 0;
    size_t n = text.size();
    size_t pos = linedata.linestarts[startposy];
    
    bool newline;
    for (int i=0 ; i<ync ; ++i)
    {
        newline = false;
        
        for (int j=0 ; j<xnc ; ++j)
        {
            if (pos < n && !newline)
            {
                ci = cr::next_unicode(text, pos);
                if (ci == chars::eol) { newline = true; cx = 1; cy = 0; }
                else                  { settings->find(ci, cx, cy); }
            }
            else
            {
                cx = 1; cy = 0;
            }
            renderdata[(i*xnc+j)*rcsize +  2] = float(cx) / settings->im_char_cols;
            renderdata[(i*xnc+j)*rcsize +  3] = float(cy) / settings->im_char_rows;
            
            renderdata[(i*xnc+j)*rcsize +  6] = float(cx)   / settings->im_char_cols;
            renderdata[(i*xnc+j)*rcsize +  7] = float(cy+1) / settings->im_char_rows;
            
            renderdata[(i*xnc+j)*rcsize + 10] = float(cx+1) / settings->im_char_cols;
            renderdata[(i*xnc+j)*rcsize + 11] = float(cy)   / settings->im_char_rows;
            
            renderdata[(i*xnc+j)*rcsize + 14] = float(cx+1) / settings->im_char_cols;
            renderdata[(i*xnc+j)*rcsize + 15] = float(cy+1) / settings->im_char_rows;
            
            renderdata[(i*xnc+j)*rcsize + 18] = float(cx+1) / settings->im_char_cols;
            renderdata[(i*xnc+j)*rcsize + 19] = float(cy)   / settings->im_char_rows;
            
            renderdata[(i*xnc+j)*rcsize + 22] = float(cx)   / settings->im_char_cols;
            renderdata[(i*xnc+j)*rcsize + 23] = float(cy+1) / settings->im_char_rows;
        }
    }
}

}
