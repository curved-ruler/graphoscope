
#include "r_nshaded.hh"

#include <iostream>
#include "mesh_attribs.hh"


namespace gsgl {

r_nshaded::r_nshaded()
{
    vf = nullptr;
}
r_nshaded::~r_nshaded()
{
    delete vf;
}

void r_nshaded::setup (cr::render_mode* rm)
{
    settings = rm;
    
    std::string vs = pc.process(settings->shader_dir,  "n-shaded.glsl.v");
    std::string fs = pc.process(settings->shader_dir, "vn-shaded.glsl.f");
    
    vf = new renderprog(vs, fs);
    status = vf->get_state();
}

void r_nshaded::init_render(unsigned int w, unsigned int h)
{
    glUseProgram(vf->get_progid());
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    
    glViewport(0, 0, w, h);
}

void r_nshaded::pre_render()
{
    GLfloat depth = 1.0f;
    glClearBufferfv(GL_COLOR, 0, settings->back);
    glClearBufferfv(GL_DEPTH, 0, &depth);
}

void r_nshaded::render(const cr::camera& cam, const cr::mat4& modeltr, const cr::rrr_buffers& bufs)
{
#ifdef C_GSGL_DEBUG
    if (status != 1)
    {
        //std::cout << "NR" << std::endl;
        return;
    }
#endif
    
    if (settings->colourmode & 1)
    {
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
    }
    else
    {
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }
    
    //glBindVertexArray(mesh->vao);
    
    switch (settings->objtype)
    {
        case 0:  glBindBuffer(GL_ARRAY_BUFFER, bufs.pnt_buf); break;
        case 1:  glBindBuffer(GL_ARRAY_BUFFER, bufs.lin_buf); break;
        default: glBindBuffer(GL_ARRAY_BUFFER, bufs.tri_buf); break;
    }
    
    glEnableVertexAttribArray(obj::POS_BUF_ID);
    glEnableVertexAttribArray(obj::COL_BUF_ID);
    glVertexAttribPointer(obj::POS_BUF_ID, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), BUFFER_OFFSET(NULL));
    glVertexAttribPointer(obj::COL_BUF_ID, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), BUFFER_OFFSET(3 * sizeof(float)));
    
    cr::mat4 view = cam.view_mat();
    cr::mat4 proj = (settings->camtype == 0) ? cam.ortho_mat() : cam.persp_mat();
    bool succ = true;
    succ &= vf->set_uniform_m4   ("p",      proj);
    succ &= vf->set_uniform_m4   ("vm",     view * modeltr);
    succ &= vf->set_uniform_float("aspect", cam.aspect);
    succ &= vf->set_uniform_float("nn",     cam.near);
    succ &= vf->set_uniform_float("ff",     cam.far);
    succ &= vf->set_uniform_float("rad",    settings->pp6size);
    succ &= vf->set_uniform_int  ("proj",   settings->camtype);
    succ &= vf->set_uniform_int  ("colm",   (int)((settings->colourmode & 2) >> 1));
    succ &= vf->set_uniform_v3   ("base_color", cr::vec3{ settings->col[0], settings->col[1], settings->col[2] });
    if (settings->colourmode & 1) succ &= vf->set_uniform_float("alpha", settings->col[3]);
    else                          succ &= vf->set_uniform_float("alpha", 1.0);
    
#ifdef C_GSGL_DEBUG
    if (!succ)
    {
        status = -10;
        return;
    }
#endif
    
    switch (settings->objtype)
    {
        case 0:  glDrawArrays(GL_POINTS,    0, bufs.pntn * 1); break;
        case 1:  glDrawArrays(GL_LINES,     0, bufs.linn * 2); break;
        default: glDrawArrays(GL_TRIANGLES, 0, bufs.trin * 3); break;
    }
    
    glDisableVertexAttribArray(obj::POS_BUF_ID);
    glDisableVertexAttribArray(obj::COL_BUF_ID);
}


}
