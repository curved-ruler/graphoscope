
#include "mesh_ux.hh"


namespace cr {


mesh_ux::mesh_ux (bool tn, bool ln, bool pn)
{
    tnorm = tn;
    tsize = tnorm ? 3 * (3+3+3) : 3 * (3+3);
    lnorm = ln;
    lsize = lnorm ? 2 * (3+3+3) : 2 * (3+3);
    pnorm = pn;
    psize = pnorm ? 1 * (3+3+3) : 1 * (3+3);
    
    clear();
}
mesh_ux::mesh_ux (uint32 t, bool tn, uint32 l, bool ln, uint32 p, bool pn)
{
    tnorm = tn;
    tsize = tnorm ? 3 * (3+3+3) : 3 * (3+3);
    lnorm = ln;
    lsize = lnorm ? 2 * (3+3+3) : 2 * (3+3);
    pnorm = pn;
    psize = pnorm ? 1 * (3+3+3) : 1 * (3+3);
    
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

void mesh_ux::add_tri_n (vec3 v0, vec3 v1, vec3 v2, material m, vec3 norm)
{
    if (!tnorm) { return; }
    
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
void mesh_ux::add_lin_n (vec3 v0, vec3 v1, material m, vec3 norm)
{
    if (!lnorm) { return; }
    
    lines.push_back(v0.x);
    lines.push_back(v0.y);
    lines.push_back(v0.z);
    lines.push_back(m.albedo.x);
    lines.push_back(m.albedo.y);
    lines.push_back(m.albedo.z);
    lines.push_back(norm.x);
    lines.push_back(norm.y);
    lines.push_back(norm.z);
    
    lines.push_back(v1.x);
    lines.push_back(v1.y);
    lines.push_back(v1.z);
    lines.push_back(m.albedo.x);
    lines.push_back(m.albedo.y);
    lines.push_back(m.albedo.z);
    lines.push_back(norm.x);
    lines.push_back(norm.y);
    lines.push_back(norm.z);
}

void mesh_ux::add_pnt_n (vec3 v0, material m, vec3 norm)
{
    if (!pnorm) { return; }
    
    points.push_back(v0.x);
    points.push_back(v0.y);
    points.push_back(v0.z);
    points.push_back(m.albedo.x);
    points.push_back(m.albedo.y);
    points.push_back(m.albedo.z);
    points.push_back(norm.x);
    points.push_back(norm.y);
    points.push_back(norm.z);
}
void mesh_ux::add_tri (vec3 v0, vec3 v1, vec3 v2, material m)
{
    if (tnorm) { return; }
    
    triangles.push_back(v0.x);
    triangles.push_back(v0.y);
    triangles.push_back(v0.z);
    triangles.push_back(m.albedo.x);
    triangles.push_back(m.albedo.y);
    triangles.push_back(m.albedo.z);
    
    triangles.push_back(v1.x);
    triangles.push_back(v1.y);
    triangles.push_back(v1.z);
    triangles.push_back(m.albedo.x);
    triangles.push_back(m.albedo.y);
    triangles.push_back(m.albedo.z);
    
    triangles.push_back(v2.x);
    triangles.push_back(v2.y);
    triangles.push_back(v2.z);
    triangles.push_back(m.albedo.x);
    triangles.push_back(m.albedo.y);
    triangles.push_back(m.albedo.z);
}

void mesh_ux::add_lin (vec3 v0, vec3 v1, material m)
{
    if (lnorm) { return; }
    
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
    if (pnorm) { return; }
    
    points.push_back(v0.x);
    points.push_back(v0.y);
    points.push_back(v0.z);
    points.push_back(m.albedo.x);
    points.push_back(m.albedo.y);
    points.push_back(m.albedo.z);
}

void mesh_ux::set_tri_n (uint32 ti, vec3 v0, vec3 v1, vec3 v2, material m, vec3 norm)
{
    if (!tnorm) { return; }
    
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
void mesh_ux::set_lin_n (uint32 li, vec3 v0, vec3 v1, material m, vec3 norm)
{
    if (!lnorm) { return; }
    
    if (li < lines.size() / lsize)
    {
        lines[li*lsize]      = v0.x;
        lines[li*lsize + 1]  = v0.y;
        lines[li*lsize + 2]  = v0.z;
        lines[li*lsize + 3]  = m.albedo.x;
        lines[li*lsize + 4]  = m.albedo.y;
        lines[li*lsize + 5]  = m.albedo.z;
        lines[li*lsize + 6]  = norm.x;
        lines[li*lsize + 7]  = norm.y;
        lines[li*lsize + 8]  = norm.z;
        
        lines[li*lsize + 9]  = v1.x;
        lines[li*lsize + 10] = v1.y;
        lines[li*lsize + 11] = v1.z;
        lines[li*lsize + 12] = m.albedo.x;
        lines[li*lsize + 13] = m.albedo.y;
        lines[li*lsize + 14] = m.albedo.z;
        lines[li*lsize + 15] = norm.x;
        lines[li*lsize + 16] = norm.y;
        lines[li*lsize + 17] = norm.z;
    }
}

void mesh_ux::set_pnt_n (uint32 vi, vec3 v0, material m, vec3 norm)
{
    if (!pnorm) { return; }
    
    if (vi < points.size() / psize)
    {
        points[vi*psize]      = v0.x;
        points[vi*psize + 1]  = v0.y;
        points[vi*psize + 2]  = v0.z;
        points[vi*psize + 3]  = m.albedo.x;
        points[vi*psize + 4]  = m.albedo.y;
        points[vi*psize + 5]  = m.albedo.z;
        points[vi*psize + 6]  = norm.x;
        points[vi*psize + 7]  = norm.y;
        points[vi*psize + 8]  = norm.z;
    }
}

void mesh_ux::set_tri (uint32 ti, vec3 v0, vec3 v1, vec3 v2, material m)
{
    if (tnorm) { return; }
    
    if (ti < triangles.size() / tsize)
    {
        triangles[ti*tsize]      = v0.x;
        triangles[ti*tsize + 1]  = v0.y;
        triangles[ti*tsize + 2]  = v0.z;
        triangles[ti*tsize + 3]  = m.albedo.x;
        triangles[ti*tsize + 4]  = m.albedo.y;
        triangles[ti*tsize + 5]  = m.albedo.z;
        
        triangles[ti*tsize + 6]  = v1.x;
        triangles[ti*tsize + 7]  = v1.y;
        triangles[ti*tsize + 8]  = v1.z;
        triangles[ti*tsize + 9]  = m.albedo.x;
        triangles[ti*tsize + 10] = m.albedo.y;
        triangles[ti*tsize + 11] = m.albedo.z;
        
        triangles[ti*tsize + 12] = v2.x;
        triangles[ti*tsize + 13] = v2.y;
        triangles[ti*tsize + 14] = v2.z;
        triangles[ti*tsize + 15] = m.albedo.x;
        triangles[ti*tsize + 16] = m.albedo.y;
        triangles[ti*tsize + 17] = m.albedo.z;
    }
}

void mesh_ux::set_lin (uint32 li, vec3 v0, vec3 v1, material m)
{
    if (lnorm) { return; }
    
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

void mesh_ux::set_pnt (uint32 vi, vec3 v0, material m)
{
    if (pnorm) { return; }
    
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
