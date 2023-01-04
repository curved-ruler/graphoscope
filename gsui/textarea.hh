
#pragma once

#include <string>
#include <vector>

#include <crcc/crcc.hh>

#include "widget.hh"

namespace gs {

struct textarea_linedata
{
    std::vector<size_t> linestarts;
};

class textarea : public widget
{
    private:
        cr::ui_render_mode* settings;
        
        bool  wordwrap;
        float pixsize;
        float charw, charh;
        float xsep, ysep;
        int   w, h;
        int   xnc, ync;
        
        size_t startposx, startposy;
        textarea_linedata linedata;
        void calc_linedata();
        
        std::vector<size_t> cursors;
    
    public:
        std::string text;
        
        int rcsize;
        std::vector<float>  renderdata;
        std::vector<uint32> colors;
    
    public:
        textarea ();
        virtual ~textarea ();
        
        void setup (cr::ui_render_mode* s);
        void setdims (float _pixsize, float _xsep, float _ysep, int _w, int _h, bool _wrap);
        
        void settext (const std::string& txt);
        void paste   (const std::string& txt);
        
        void scrolly (int y);
        
        void render ();
};

}
