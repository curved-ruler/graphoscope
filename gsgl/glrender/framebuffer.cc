
#include "framebuffer.hh"

#include <string>
#include "../gsgl_debug.hh"
#include "mesh_attribs.hh"

namespace gsgl {

framebuffer::framebuffer(cr::render_mode* rm, GLuint _tex)
{
    settings = rm;
    tex = _tex;
    
    glGenFramebuffers(1, &fbo);

    bind();
    glBindTexture(GL_TEXTURE_2D, tex);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
    
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, settings->viewport[2] / settings->pixel_size, settings->viewport[3] / settings->pixel_size);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, settings->viewport[2], settings->viewport[3]);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    
    //std::cout << "E1" << std::endl;
    //GL_CHECK_ERROR();

    const float q[24] = { // vertex attributes for a quad that fills the entire screen
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    for (int i=0 ; i<24 ; ++i) quadVertices[i] = q[i];
    //glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    //glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*24, &quadVertices, GL_STATIC_DRAW);
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    cr::precompiler pc;
    std::string vs = pc.process(settings->shader_dir, "offscreen.glsl.v");
    std::string fs = pc.process(settings->shader_dir, "postproc_no.glsl.f");
    prog[0] = new renderprog(vs, fs);
    
    fs = pc.process(settings->shader_dir, "postproc_inv.glsl.f");
    prog[1] = new renderprog(vs, fs);
    
    fs = pc.process(settings->shader_dir, "postproc_enb.glsl.f");
    prog[2] = new renderprog(vs, fs);
}
framebuffer::~framebuffer()
{
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rbo);
    //glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    delete prog[0];
    delete prog[1];
    delete prog[2];
}

void framebuffer::bind ()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}
void framebuffer::unbind ()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
bool framebuffer::is_complete()
{
    bind();
    return (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

void framebuffer::use()
{
    bind();
}
void framebuffer::render()
{
    unbind();
    
    glUseProgram(prog[settings->proctype]->get_progid());
    glViewport(settings->viewport[0], settings->viewport[1], settings->viewport[2], settings->viewport[3]);
    
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), BUFFER_OFFSET(NULL));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), BUFFER_OFFSET(2 * sizeof(float)));

    glBindTexture(GL_TEXTURE_2D, tex);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}



}

