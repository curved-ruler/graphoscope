
#pragma once

#include <crcc/crcc.hh>
#include "../gsgl_config.hh"
#include "renderprog.hh"

namespace gsgl {

class framebuffer
{
    private:
        cr::render_mode* settings;
        
        GLuint fbo;
        GLuint tex;
        GLuint rbo;
        GLuint quadVAO, quadVBO;
        float quadVertices[24];
        renderprog* prog;
        
    public:
        framebuffer(cr::render_mode* rm, GLuint _texture);
        ~framebuffer();
        
        void bind();
        void unbind();
        bool is_complete();
        
        void use();
        void render();
};

}

