
#include "gpu_transfer.hh"


namespace gsgl {

void TXTtoGPU (const std::vector<float>& obj, cr::rrr_buffers& buf)
{
    buf.trin = (uint32)obj.size() / 4;
    buf.linn = 0;
    buf.tri_buf = 0;
    buf.lin_buf = 0;
    glGenBuffers(1, &(buf.tri_buf));
    glBindBuffer(GL_ARRAY_BUFFER, buf.tri_buf);
    glBufferData(GL_ARRAY_BUFFER, obj.size() * sizeof(float), obj.data(), GL_DYNAMIC_DRAW);
}

void MUXtoGPU (const cr::mesh_ux& obj, cr::rrr_buffers& buf)
{
    buf.trin = (uint32)obj.triangles.size() / cr::mesh_ux::tsize;
    buf.linn = (uint32)obj.lines.size()     / cr::mesh_ux::lsize;
    buf.pntn = (uint32)obj.points.size()    / cr::mesh_ux::psize;
    
    buf.tri_buf = 0;
    buf.lin_buf = 0;
    buf.pnt_buf = 0;
    
    if (buf.trin > 0)
    {
        glGenBuffers(1, &(buf.tri_buf));
        glBindBuffer(GL_ARRAY_BUFFER, buf.tri_buf);
        glBufferData(GL_ARRAY_BUFFER, obj.triangles.size() * sizeof(float), obj.triangles.data(), GL_STATIC_DRAW);
    }
    
    if (buf.linn > 0)
    {
        glGenBuffers(1, &(buf.lin_buf));
        glBindBuffer(GL_ARRAY_BUFFER, buf.lin_buf);
        glBufferData(GL_ARRAY_BUFFER, obj.lines.size() * sizeof(float), obj.lines.data(), GL_STATIC_DRAW);
    }
    
    if (buf.pntn > 0)
    {
        glGenBuffers(1, &(buf.pnt_buf));
        glBindBuffer(GL_ARRAY_BUFFER, buf.pnt_buf);
        glBufferData(GL_ARRAY_BUFFER, obj.points.size() * sizeof(float), obj.points.data(), GL_STATIC_DRAW);
    }
}

void MUXtoGPU (const cr::mesh_ux& triobj, const cr::mesh_ux& linobj, cr::rrr_buffers& buf)
{
    buf.trin = (uint32)triobj.triangles.size() / cr::mesh_ux::tsize;
    buf.linn = (uint32)linobj.lines.size()     / cr::mesh_ux::lsize;
    
    buf.tri_buf = 0;
    buf.lin_buf = 0;
    glGenBuffers(1, &(buf.tri_buf));
    glGenBuffers(1, &(buf.lin_buf));
    
    //glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, buf.tri_buf);
    glBufferData(GL_ARRAY_BUFFER, triobj.triangles.size() * sizeof(float), triobj.triangles.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, buf.lin_buf);
    glBufferData(GL_ARRAY_BUFFER, linobj.lines.size() * sizeof(float), linobj.lines.data(), GL_STATIC_DRAW);
}

void updateGPU (const cr::mesh_ux& obj, cr::rrr_buffers& buffers)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffers.tri_buf);
    glBufferData(GL_ARRAY_BUFFER, obj.triangles.size() * sizeof(float), obj.triangles.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, buffers.lin_buf);
    glBufferData(GL_ARRAY_BUFFER, obj.lines.size() * sizeof(float), obj.lines.data(), GL_STATIC_DRAW);
}

void updateGPU (const cr::mesh_ux& triobj, const cr::mesh_ux& linobj, cr::rrr_buffers& buf)
{
    glBindBuffer(GL_ARRAY_BUFFER, buf.tri_buf);
    //glBufferData(GL_ARRAY_BUFFER, triobj.triangles.size() * sizeof(float), 0, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, triobj.triangles.size() * sizeof(float), triobj.triangles.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, buf.lin_buf);
    //glBufferData(GL_ARRAY_BUFFER, linobj.lines.size() * sizeof(float), 0, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, linobj.lines.size() * sizeof(float), linobj.lines.data(), GL_STATIC_DRAW);
}

void clearGPU (cr::rrr_buffers& buffers)
{
    glDeleteBuffers(1, &(buffers.tri_buf));
    glDeleteBuffers(1, &(buffers.lin_buf));
    glDeleteBuffers(1, &(buffers.pnt_buf));
}


}
