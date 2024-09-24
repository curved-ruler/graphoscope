
#include "mesh_converter.hh"

#include "../math/mu.hh"
#include "../gen/bitutils.hh"
#include "material.hh"

#include <cstdint>
#include <fstream>
#include <iostream>


namespace cr {

mesh_ux* bm1_to_meshux (const std::string& filename)
{
    //cout << "BM1: " << filename << endl;
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file)
    {
        std::cout << "ERROR: parse_bm1: file does not exists: " << filename << std::endl;
        return nullptr;
    }
    
    unsigned int c,v,t,l;
    
    file.read((char*)&c, sizeof(unsigned int)); //1
    //std::cout << c << std::endl;
    
    file.read((char*)&c, sizeof(unsigned int));
    //std::cout << c << std::endl;
    file.read((char*)&v, sizeof(unsigned int));
    //std::cout << v << std::endl;
    file.read((char*)&t, sizeof(unsigned int));
    //std::cout << t << std::endl;
    file.read((char*)&l, sizeof(unsigned int));
    //std::cout << l << std::endl;
    
    mesh_ux* retmesh = new mesh_ux(t, true, l, false, 0, false);
    
    // cols
    float* cols = new float[c*3];
    file.read((char*)cols, sizeof(float) * c*3);
    // verts
    float* verts = new float[v*3];
    file.read((char*)verts, sizeof(float) * v*3);
    // triangles
    unsigned int* tris = new unsigned int[t*3];
    unsigned int* tc = new unsigned int[t];
    file.read((char*)tris, sizeof(unsigned int) * t*3);
    file.read((char*)tc, sizeof(unsigned int) * t);
    // lines
    unsigned int* lins = new unsigned int[l*2];
    unsigned int* lc = new unsigned int[l];
    file.read((char*)lins, sizeof(unsigned int) * l*2);
    file.read((char*)lc, sizeof(unsigned int) * l);
    
    material mat;
    
    for (unsigned int i=0 ; i<t ; ++i)
    {
        mat.albedo = vec3{ cols[tc[i]*3], cols[tc[i]*3 + 1], cols[tc[i]*3 + 2] };
        
        vec3 v0 = vec3{ verts[tris[i*3]*3],     verts[tris[i*3]*3 + 1],     verts[tris[i*3]*3 + 2] };
        vec3 v1 = vec3{ verts[tris[i*3 + 1]*3], verts[tris[i*3 + 1]*3 + 1], verts[tris[i*3 + 1]*3 + 2] };
        vec3 v2 = vec3{ verts[tris[i*3 + 2]*3], verts[tris[i*3 + 2]*3 + 1], verts[tris[i*3 + 2]*3 + 2]  };
        
        vec3 norm = cross((v1-v0), (v2-v0));
        norm.normalize();
        
        retmesh->set_tri_n(i, v0, v1, v2, mat, norm);
    }
    
    for (unsigned int i=0 ; i<l ; ++i)
    {
        mat.albedo = vec3{ cols[lc[i]*3], cols[lc[i]*3 + 1], cols[lc[i]*3 + 2] };
        
        retmesh->set_lin(i,
                         vec3{ verts[lins[i*2]*3],     verts[lins[i*2]*3 + 1],     verts[lins[i*2]*3 + 2] },
                         vec3{ verts[lins[i*2 + 1]*3], verts[lins[i*2 + 1]*3 + 1], verts[lins[i*2 + 1]*3 + 2] },
                         mat);
    }
    
    file.close();
    
    delete[] cols;
    delete[] verts;
    delete[] tris;
    delete[] tc;
    delete[] lins;
    delete[] lc;
    
    return retmesh;
}

mesh_ix* bm1_to_meshix (const std::string& filename)
{
    //cout << "BM1: " << filename << endl;
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file)
    {
        std::cout << "ERROR: parse_bm1: file does not exists: " << filename << std::endl;
        return nullptr;
    }
    
    unsigned int c,v,t,l;
    
    file.read((char*)&c, sizeof(unsigned int)); //1
    //std::cout << c << std::endl;
    
    file.read((char*)&c, sizeof(unsigned int));
    //std::cout << c << std::endl;
    file.read((char*)&v, sizeof(unsigned int));
    //std::cout << v << std::endl;
    file.read((char*)&t, sizeof(unsigned int));
    //std::cout << t << std::endl;
    file.read((char*)&l, sizeof(unsigned int));
    //std::cout << l << std::endl;
    
    mesh_ix* retmesh = new mesh_ix(c, v, t, l);
    
    // cols
    float* cols = new float[c*3];
    file.read((char*)cols, sizeof(float) * c*3);
    // verts
    float* verts = new float[v*3];
    file.read((char*)verts, sizeof(float) * v*3);
    // triangles
    unsigned int* tris = new unsigned int[t*3];
    unsigned int* tc = new unsigned int[t];
    file.read((char*)tris, sizeof(unsigned int) * t*3);
    file.read((char*)tc, sizeof(unsigned int) * t);
    // lines
    unsigned int* lins = new unsigned int[l*2];
    unsigned int* lc = new unsigned int[l];
    file.read((char*)lins, sizeof(unsigned int) * l*2);
    file.read((char*)lc, sizeof(unsigned int) * l);
    
    material mat;
    for (unsigned int i=0 ; i<c ; ++i)
    {
        mat.albedo = vec3{ cols[i*3], cols[i*3 + 1], cols[i*3 + 2] };
        retmesh->set_mat(i, mat);
    }
    
    for (unsigned int i=0 ; i<v ; ++i)
    {
        retmesh->set_ver(i, vec3{ verts[i*3], verts[i*3 + 1], verts[i*3 + 2] });
    }
    
    for (unsigned int i=0 ; i<t ; ++i)
    {
        retmesh->set_tri(i, tris[i*3], tris[i*3 + 1], tris[i*3 + 2], tc[i]);
    }
    
    for (unsigned int i=0 ; i<l ; ++i)
    {
        retmesh->set_lin(i, lins[i*2], lins[i*2 + 1], lc[i]);
    }
    
    file.close();
    
    delete[] cols;
    delete[] verts;
    delete[] tris;
    delete[] tc;
    delete[] lins;
    delete[] lc;
    
    return retmesh;
}



mesh_ux* terr_to_meshux (const std::string& filename)
{
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file)
    {
        std::cout << "ERROR: 'terr' file does not exists: " << filename << std::endl;
        return nullptr;
    }
    
    uint32 nx;
    uint32 ny;
    file.read((char*)&nx, sizeof(uint32));
    file.read((char*)&ny, sizeof(uint32));
    
    uint32 t = (nx)*(ny)*2;
    //uint32 l = (nx)*(ny)*2 + (nx+ny);
    uint32 l = (nx)*(ny)*2;
    uint32 p = nx*ny;
    mesh_ux* retmesh = new mesh_ux(t, true, l, true, p, true);
    
    float  Z[(nx+1)*(ny+1)];
    uint32 C[(nx+1)*(ny+1)];
    
    file.read((char*)Z, sizeof(float)  * (nx+1)*(ny+1));
    file.read((char*)C, sizeof(uint32) * (nx+1)*(ny+1));
    
    file.close();
    
    float z1,z2,z3,z4;
    material mat;
    //float scale = 1.0f / 20.0f;
    float scale = 1.0f;
    
    for (uint32 y=0 ; y<ny ; ++y)
    for (uint32 x=0 ; x<nx ; ++x)
    {
        z1 = Z[y*(nx+1)+x];
        z2 = Z[(y+1)*(nx+1)+x];
        z3 = Z[(y)*(nx+1)+(x+1)];
        z4 = Z[(y+1)*(nx+1)+(x+1)];
        
        z1 = z1 * scale;
        z2 = z2 * scale;
        z3 = z3 * scale;
        z4 = z4 * scale;
        vec3 v0 = vec3{ float(x),   float(y),   z1 };
        vec3 v1 = vec3{ float(x),   float(y+1), z2 };
        vec3 v2 = vec3{ float(x+1), float(y),   z3 };
        vec3 v3 = vec3{ float(x+1), float(y+1), z4 };
        vec3 norm = cross((v1-v0), (v2-v0));
        norm.normalize();
        
        
        uint32 ci = C[y*(nx+1)+x];
        uint8 rr,gg,bb,aa;
        get_bytes(ci, rr, gg, bb, aa);
        mat.albedo.x = float(rr)/255.0f;
        mat.albedo.y = float(gg)/255.0f;
        mat.albedo.z = float(bb)/255.0f;
        
        retmesh->set_tri_n((y*(nx)+x)*2,  v1,v0,v2,mat,norm);
        
        retmesh->set_tri_n((y*(nx)+x)*2 + 1,  v2,v3,v1,mat,norm);
        
        retmesh->set_lin_n((y*(nx)+x)*2,  v0,v1,mat,norm);
        
        retmesh->set_lin_n((y*(nx)+x)*2 + 1,  v0,v2,mat,norm);
        
        retmesh->set_pnt_n((y*(nx)+x),
                         vec3{ float(x) + 0.5f, float(y) + 0.5f, (z1+z2+z3+z4)/4.0f },
                         mat, norm);
    }
    
    return retmesh;
    
    //delete[] z;
    //delete[] c;
}

}
