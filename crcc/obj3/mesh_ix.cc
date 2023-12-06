#include "mesh_ix.hh"


namespace cr {


mesh_ix::mesh_ix () {}
mesh_ix::mesh_ix (unsigned int m, unsigned int v, unsigned int t, unsigned int l)
{
    materials.resize(m);
    vertices.resize(v);
    triangles.resize(t);
    lines.resize(l);
}
mesh_ix::~mesh_ix ()
{
    clear();
}

void mesh_ix::clear ()
{
    materials.clear();
    vertices.clear();
    triangles.clear();
    lines.clear();
}

void mesh_ix::add_mat (material m)
{
    materials.push_back(m);
}
void mesh_ix::add_ver (vec3 v)
{
    vertices.push_back(v.x);
    vertices.push_back(v.y);
    vertices.push_back(v.z);
}
void mesh_ix::add_tri (uint32 v0, uint32 v1, uint32 v2, uint32 m)
{
    triangles.push_back(v0);
    triangles.push_back(v1);
    triangles.push_back(v2);
    triangles.push_back(m);
}
void mesh_ix::add_lin (uint32 v0, uint32 v1, uint32 m)
{
    triangles.push_back(v0);
    triangles.push_back(v1);
    triangles.push_back(m);
}

void mesh_ix::set_mat (unsigned int mi, material m)
{
    if (mi < materials.size())
    {
        materials[mi] = m;
    }
}
void mesh_ix::set_ver (unsigned int vi, vec3 v)
{
    if (vi < vertices.size()/vsize)
    {
        vertices[vi*vsize]     = v.x;
        vertices[vi*vsize + 1] = v.y;
        vertices[vi*vsize + 2] = v.z;
    }
}
void mesh_ix::set_tri (unsigned int ti, uint32 v0, uint32 v1, uint32 v2, uint32 m)
{
    if (ti < triangles.size()/tsize)
    {
        triangles[ti*tsize]     = v0;
        triangles[ti*tsize + 1] = v1;
        triangles[ti*tsize + 2] = v2;
        triangles[ti*tsize + 3] = m;
    }
}
void mesh_ix::set_lin (unsigned int li, uint32 v0, uint32 v1, uint32 m)
{
    if (li < lines.size()/lsize)
    {
        lines[li*lsize]     = v0;
        lines[li*lsize + 1] = v1;
        lines[li*lsize + 2] = m;
    }
}


}
