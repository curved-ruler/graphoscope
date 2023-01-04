
#pragma once

#include <crcc/crcc.hh>
#include "renderprog.hh"


namespace gsgl {


class r_text
{
    private:
        cr::precompiler pc;
        renderprog* vf;
        GLuint tex;
        int status;
        cr::ui_render_mode* settings;
        
    public:
        r_text();
        ~r_text();
        
        void setup       (cr::ui_render_mode* rm);
        void init_render (const cr::uvec4& area);
        void pre_render  ();
        void render      (cr::rrr_buffers& buf, GLuint colbuf);
};


}
