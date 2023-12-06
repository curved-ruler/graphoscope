#pragma once

#include <vector>
#include "../math/mu.hh"
#include "material.hh"


namespace cr {


class mesh_ix
{
    public:
        static const unsigned int vsize = 3; // 3
        static const unsigned int tsize = 3+1; // 3vert + 1mat ( +1 pointer to another tri whose normal will be used?? or quad/hexa structure only? )
        static const unsigned int lsize = 2+1; // 2vert + 1mat ( +2 pointer to triangle?? )
        
    public:
        std::vector<material> materials;
        std::vector<float>    vertices;
        std::vector<uint32>   triangles;
        std::vector<uint32>   lines;
        
        
    public:
         mesh_ix ();
         mesh_ix (unsigned int m, unsigned int v, unsigned int t, unsigned int l);
        ~mesh_ix ();
        
        void clear();
        
        void add_mat (material m);
        void add_ver (vec3 v);
        void add_tri (uint32 v0, uint32 v1, uint32 v2, uint32 m);
        void add_lin (uint32 v0, uint32 v1, uint32 m);
        
        void set_mat (unsigned int mi, material m);
        void set_ver (unsigned int vi, vec3 v);
        void set_tri (unsigned int ti, uint32 v0, uint32 v1, uint32 v2, uint32 m);
        void set_lin (unsigned int li, uint32 v0, uint32 v1, uint32 m);
};


}
