
#include "mesh_ux.hh"


namespace cr {


mesh_ux::mesh_ux () {}
mesh_ux::mesh_ux (unsigned int t, unsigned int l, unsigned int p)
{
    triangles.resize(t*tsize);
    lines.resize(l*lsize);
    points.resize(p*psize);
}
mesh_ux::~mesh_ux ()
{
    clear();
}

void mesh_ux::clear ()
{
    triangles.clear();
    lines.clear();
    points.clear();
}

void mesh_ux::add_tri (vec3 v0, vec3 v1, vec3 v2, material m, vec3 norm)
{
    triangles.push_back(v0.x);
    triangles.push_back(v0.y);
    triangles.push_back(v0.z);
    triangles.push_back(m.albedo.x);
    triangles.push_back(m.albedo.y);
    triangles.push_back(m.albedo.z);
    triangles.push_back(norm.x);
    triangles.push_back(norm.y);
    triangles.push_back(norm.z);
    
    triangles.push_back(v1.x);
    triangles.push_back(v1.y);
    triangles.push_back(v1.z);
    triangles.push_back(m.albedo.x);
    triangles.push_back(m.albedo.y);
    triangles.push_back(m.albedo.z);
    triangles.push_back(norm.x);
    triangles.push_back(norm.y);
    triangles.push_back(norm.z);
    
    triangles.push_back(v2.x);
    triangles.push_back(v2.y);
    triangles.push_back(v2.z);
    triangles.push_back(m.albedo.x);
    triangles.push_back(m.albedo.y);
    triangles.push_back(m.albedo.z);
    triangles.push_back(norm.x);
    triangles.push_back(norm.y);
    triangles.push_back(norm.z);
}
void mesh_ux::add_tri (vec3 v0, vec3 v1, vec3 v2, material m)
{
    vec3 norm = cross((v1-v0), (v2-v0));
    norm.normalize();
    
    triangles.push_back(v0.x);
    triangles.push_back(v0.y);
    triangles.push_back(v0.z);
    triangles.push_back(m.albedo.x);
    triangles.push_back(m.albedo.y);
    triangles.push_back(m.albedo.z);
    triangles.push_back(norm.x);
    triangles.push_back(norm.y);
    triangles.push_back(norm.z);
    
    triangles.push_back(v1.x);
    triangles.push_back(v1.y);
    triangles.push_back(v1.z);
    triangles.push_back(m.albedo.x);
    triangles.push_back(m.albedo.y);
    triangles.push_back(m.albedo.z);
    triangles.push_back(norm.x);
    triangles.push_back(norm.y);
    triangles.push_back(norm.z);
    
    triangles.push_back(v2.x);
    triangles.push_back(v2.y);
    triangles.push_back(v2.z);
    triangles.push_back(m.albedo.x);
    triangles.push_back(m.albedo.y);
    triangles.push_back(m.albedo.z);
    triangles.push_back(norm.x);
    triangles.push_back(norm.y);
    triangles.push_back(norm.z);
}

void mesh_ux::add_lin (vec3 v0, vec3 v1, material m)
{
    lines.push_back(v0.x);
    lines.push_back(v0.y);
    lines.push_back(v0.z);
    lines.push_back(m.albedo.x);
    lines.push_back(m.albedo.y);
    lines.push_back(m.albedo.z);
    
    lines.push_back(v1.x);
    lines.push_back(v1.y);
    lines.push_back(v1.z);
    lines.push_back(m.albedo.x);
    lines.push_back(m.albedo.y);
    lines.push_back(m.albedo.z);
}

void mesh_ux::add_pnt (vec3 v0, material m)
{
    points.push_back(v0.x);
    points.push_back(v0.y);
    points.push_back(v0.z);
    points.push_back(m.albedo.x);
    points.push_back(m.albedo.y);
    points.push_back(m.albedo.z);
}

void mesh_ux::set_tri (unsigned int ti, vec3 v0, vec3 v1, vec3 v2, material m, vec3 norm)
{
    if (ti < triangles.size() / tsize)
    {
        triangles[ti*tsize]      = v0.x;
        triangles[ti*tsize + 1]  = v0.y;
        triangles[ti*tsize + 2]  = v0.z;
        triangles[ti*tsize + 3]  = m.albedo.x;
        triangles[ti*tsize + 4]  = m.albedo.y;
        triangles[ti*tsize + 5]  = m.albedo.z;
        triangles[ti*tsize + 6]  = norm.x;
        triangles[ti*tsize + 7]  = norm.y;
        triangles[ti*tsize + 8]  = norm.z;
        
        triangles[ti*tsize + 9]  = v1.x;
        triangles[ti*tsize + 10] = v1.y;
        triangles[ti*tsize + 11] = v1.z;
        triangles[ti*tsize + 12] = m.albedo.x;
        triangles[ti*tsize + 13] = m.albedo.y;
        triangles[ti*tsize + 14] = m.albedo.z;
        triangles[ti*tsize + 15] = norm.x;
        triangles[ti*tsize + 16] = norm.y;
        triangles[ti*tsize + 17] = norm.z;
        
        triangles[ti*tsize + 18] = v2.x;
        triangles[ti*tsize + 19] = v2.y;
        triangles[ti*tsize + 20] = v2.z;
        triangles[ti*tsize + 21] = m.albedo.x;
        triangles[ti*tsize + 22] = m.albedo.y;
        triangles[ti*tsize + 23] = m.albedo.z;
        triangles[ti*tsize + 24] = norm.x;
        triangles[ti*tsize + 25] = norm.y;
        triangles[ti*tsize + 26] = norm.z;
    }
}
void mesh_ux::set_tri (unsigned int ti, vec3 v0, vec3 v1, vec3 v2, material m)
{
    if (ti < triangles.size() / tsize)
    {
        vec3 norm = cross((v1-v0), (v2-v0));
        norm.normalize();
        
        triangles[ti*tsize]      = v0.x;
        triangles[ti*tsize + 1]  = v0.y;
        triangles[ti*tsize + 2]  = v0.z;
        triangles[ti*tsize + 3]  = m.albedo.x;
        triangles[ti*tsize + 4]  = m.albedo.y;
        triangles[ti*tsize + 5]  = m.albedo.z;
        triangles[ti*tsize + 6]  = norm.x;
        triangles[ti*tsize + 7]  = norm.y;
        triangles[ti*tsize + 8]  = norm.z;
        
        triangles[ti*tsize + 9]  = v1.x;
        triangles[ti*tsize + 10] = v1.y;
        triangles[ti*tsize + 11] = v1.z;
        triangles[ti*tsize + 12] = m.albedo.x;
        triangles[ti*tsize + 13] = m.albedo.y;
        triangles[ti*tsize + 14] = m.albedo.z;
        triangles[ti*tsize + 15] = norm.x;
        triangles[ti*tsize + 16] = norm.y;
        triangles[ti*tsize + 17] = norm.z;
        
        triangles[ti*tsize + 18] = v2.x;
        triangles[ti*tsize + 19] = v2.y;
        triangles[ti*tsize + 20] = v2.z;
        triangles[ti*tsize + 21] = m.albedo.x;
        triangles[ti*tsize + 22] = m.albedo.y;
        triangles[ti*tsize + 23] = m.albedo.z;
        triangles[ti*tsize + 24] = norm.x;
        triangles[ti*tsize + 25] = norm.y;
        triangles[ti*tsize + 26] = norm.z;
    }
}

void mesh_ux::set_lin (unsigned int li, vec3 v0, vec3 v1, material m)
{
    if (li < lines.size() / lsize)
    {
        lines[li*lsize]      = v0.x;
        lines[li*lsize + 1]  = v0.y;
        lines[li*lsize + 2]  = v0.z;
        lines[li*lsize + 3]  = m.albedo.x;
        lines[li*lsize + 4]  = m.albedo.y;
        lines[li*lsize + 5]  = m.albedo.z;
        
        lines[li*lsize + 6]  = v1.x;
        lines[li*lsize + 7]  = v1.y;
        lines[li*lsize + 8]  = v1.z;
        lines[li*lsize + 9]  = m.albedo.x;
        lines[li*lsize + 10] = m.albedo.y;
        lines[li*lsize + 11] = m.albedo.z;
    }
}

void mesh_ux::set_pnt (unsigned int vi, vec3 v0, material m)
{
    if (vi < points.size() / psize)
    {
        points[vi*psize]      = v0.x;
        points[vi*psize + 1]  = v0.y;
        points[vi*psize + 2]  = v0.z;
        points[vi*psize + 3]  = m.albedo.x;
        points[vi*psize + 4]  = m.albedo.y;
        points[vi*psize + 5]  = m.albedo.z;
    }
}

}
