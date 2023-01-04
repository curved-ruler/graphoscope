
#pragma once

#include <vector>

#include <crcc/crcc.hh>
#include "widget.hh"

namespace gs {



class uiengine
{
    public:
        cr::ui_render_mode settings;
        std::vector<widget*> widgets;
        
    public:
        uiengine(cr::scripter& conf);
        ~uiengine();
};

}
