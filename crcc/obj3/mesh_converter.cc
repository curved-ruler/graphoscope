
#include "mesh_converter.hh"

#include "../math/mu.hh"
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
    
    mesh_ux* retmesh = new mesh_ux(t, l, 0);
    
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
        
        retmesh->set_tri(i,
                         vec3{ verts[tris[i*3]*3],     verts[tris[i*3]*3 + 1],     verts[tris[i*3]*3 + 2] },
                         vec3{ verts[tris[i*3 + 1]*3], verts[tris[i*3 + 1]*3 + 1], verts[tris[i*3 + 1]*3 + 2] },
                         vec3{ verts[tris[i*3 + 2]*3], verts[tris[i*3 + 2]*3 + 1], verts[tris[i*3 + 2]*3 + 2]  },
                         mat);
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
        retmesh->set_ver(i, vec3{ verts[i*3], verts[i*3 + 1], verts[i*3 + 1] });
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

}
