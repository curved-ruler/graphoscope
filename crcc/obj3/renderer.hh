
#pragma once

#include "../math/mu.hh"
#include "../obj3/mesh_ux.hh"
#include "camera.hh"
#include "render_mode.hh"

namespace cr {

class renderer
{
    public:
        virtual ~renderer();
        
        virtual void setup       (render_mode* settings) = 0;
        virtual void init_render (unsigned int w, unsigned int h) = 0;
        virtual void pre_render  () = 0;
        virtual void render      (const camera& cam, const mat4& modeltr, const rrr_buffers& bufs);
        virtual void render      (const camera& cam, const mat4& modeltr, const cr::mesh_ux& mesh);
};

}
