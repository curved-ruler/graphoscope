
#include "r_text.hh"

#include "mesh_attribs.hh"
#include "../gsgl_debug.hh"

namespace gsgl {


r_text::r_text  ()
{
    vf = nullptr;
}
r_text::~r_text ()
{
    delete vf;
    glDeleteTextures(1, &tex);
}

void r_text::setup (cr::ui_render_mode* rm)
{
    settings = rm;
    
    std::string vs = pc.process(settings->shader_dir, "text.glsl.v");
    std::string fs = pc.process(settings->shader_dir, "text.glsl.f");
    vf = new renderprog(vs, fs);
    
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    int x = settings->im_char_cols * settings->im_char_x;
    int y = settings->im_char_rows * settings->im_char_y;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, x, y, 0, GL_RED, GL_UNSIGNED_BYTE, settings->im);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void r_text::init_render (const cr::uvec4& area)
{
    GL_CHECK_ERROR();
    glUseProgram(vf->get_progid());
    
    glDisable(GL_DEPTH_TEST);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glViewport(area.x, area.y, area.z, area.w);
    vf->set_uniform_float("viewportx", area.z - area.x);
    vf->set_uniform_float("viewporty", area.w - area.y);
    GL_CHECK_ERROR();
}
void r_text::pre_render () {}

void r_text::render (cr::rrr_buffers& buf, GLuint colbuf)
{
    GL_CHECK_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, buf.tri_buf);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), BUFFER_OFFSET(NULL) );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), BUFFER_OFFSET(2 * sizeof(float)) );
    glEnableVertexAttribArray(1);
    GL_CHECK_ERROR();
    
    glBindBuffer(GL_ARRAY_BUFFER, colbuf);
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, 1*sizeof(uint32), BUFFER_OFFSET(NULL) );
    glEnableVertexAttribArray(2);
    GL_CHECK_ERROR();
    
    glBindTexture(GL_TEXTURE_2D, tex);
    glDrawArrays(GL_TRIANGLES, 0, buf.trin * 3);
    
    //glDeleteVertexArrays(1, &vao);
}


}
