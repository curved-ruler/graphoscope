
#pragma once

#include <crcc/crcc.hh>
#include "renderprog.hh"


namespace gsgl {


class r_vshaded
{
    private:
        cr::precompiler pc;
        renderprog* vf;
        int status;
        cr::render_mode* settings;
        
    public:
        r_vshaded();
        ~r_vshaded();
        
        void setup       (cr::render_mode* settings);
        void init_render (unsigned int w, unsigned int h);
        void pre_render  ();
        void render      (const cr::camera& cam, const cr::mat4& modeltr, const cr::rrr_buffers& bufs);
};


}
