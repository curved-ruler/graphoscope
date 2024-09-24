
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
    buf.trin = (uint32)obj.triangles.size() / obj.tsize;
    buf.linn = (uint32)obj.lines.size()     / obj.lsize;
    buf.pntn = (uint32)obj.points.size()    / obj.psize;
    
    buf.tri_buf = 0;
    buf.lin_buf = 0;
    buf.pnt_buf = 0;
    
    buf.tnorm = obj.tnorm;
    buf.lnorm = obj.lnorm;
    buf.pnorm = obj.pnorm;
    
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
    buf.trin = (uint32)triobj.triangles.size() / triobj.tsize;
    buf.linn = (uint32)linobj.lines.size()     / linobj.lsize;
    buf.pntn = (uint32)triobj.points.size()    / triobj.psize;
    
    buf.tnorm = triobj.tnorm;
    buf.lnorm = linobj.lnorm;
    buf.pnorm = triobj.pnorm;
    
    buf.tri_buf = 0;
    buf.lin_buf = 0;
    glGenBuffers(1, &(buf.tri_buf));
    glGenBuffers(1, &(buf.lin_buf));
    
    //glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, buf.tri_buf);
    glBufferData(GL_ARRAY_BUFFER, triobj.triangles.size() * sizeof(float), triobj.triangles.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, buf.lin_buf);
    glBufferData(GL_ARRAY_BUFFER, linobj.lines.size() * sizeof(float), linobj.lines.data(), GL_STATIC_DRAW);
    
    if (buf.pntn > 0)
    {
        glGenBuffers(1, &(buf.pnt_buf));
        glBindBuffer(GL_ARRAY_BUFFER, buf.pnt_buf);
        glBufferData(GL_ARRAY_BUFFER, triobj.points.size() * sizeof(float), triobj.points.data(), GL_STATIC_DRAW);
    }
}

void updateGPU (const cr::mesh_ux& obj, cr::rrr_buffers& buffers)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffers.tri_buf);
    glBufferSubData(GL_ARRAY_BUFFER, 0, obj.triangles.size() * sizeof(float), obj.triangles.data());
    glBindBuffer(GL_ARRAY_BUFFER, buffers.lin_buf);
    glBufferSubData(GL_ARRAY_BUFFER, 0, obj.lines.size() * sizeof(float), obj.lines.data());
    glBindBuffer(GL_ARRAY_BUFFER, buffers.pnt_buf);
    glBufferSubData(GL_ARRAY_BUFFER, 0, obj.points.size() * sizeof(float), obj.points.data());
}

void updateGPU (const cr::mesh_ux& triobj, const cr::mesh_ux& linobj, cr::rrr_buffers& buf)
{
    glBindBuffer(GL_ARRAY_BUFFER, buf.tri_buf);
    glBufferSubData(GL_ARRAY_BUFFER, 0, triobj.triangles.size() * sizeof(float), triobj.triangles.data());
    glBindBuffer(GL_ARRAY_BUFFER, buf.lin_buf);
    glBufferSubData(GL_ARRAY_BUFFER, 0, linobj.lines.size() * sizeof(float), linobj.lines.data());
    glBindBuffer(GL_ARRAY_BUFFER, buf.pnt_buf);
    glBufferSubData(GL_ARRAY_BUFFER, 0, triobj.points.size() * sizeof(float), triobj.points.data());
}

void clearGPU (cr::rrr_buffers& buffers)
{
    glDeleteBuffers(1, &(buffers.tri_buf));
    glDeleteBuffers(1, &(buffers.lin_buf));
    glDeleteBuffers(1, &(buffers.pnt_buf));
}


}
