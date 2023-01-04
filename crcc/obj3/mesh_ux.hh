#pragma once

#include <vector>
#include "../math/mu.hh"
#include "material.hh"


namespace cr {


class mesh_ux
{
    public:
        static const unsigned int tsize = 3 * (3+3+3); // 3 * (3 pos + 3 norm + 3 col)
        static const unsigned int lsize = 2 * (3+3);   // 2 * (3 pos + 3 col)
        static const unsigned int psize = 1 * (3+3);   // 1 * (3 pos + 3 col)
        
    public:
        std::vector<float> triangles;
        std::vector<float> lines;
        std::vector<float> points;
        
        
    public:
         mesh_ux ();
         mesh_ux (unsigned int t, unsigned int l, unsigned int p);
        ~mesh_ux ();
        
        void clear();
        
        void add_tri (vec3 v0, vec3 v1, vec3 v2, material m, vec3 norm);
        void add_tri (vec3 v0, vec3 v1, vec3 v2, material m);
        void add_lin (vec3 v0, vec3 v1, material m);
        void add_pnt (vec3 v0, material m);
        
        void set_tri (unsigned int ti, vec3 v0, vec3 v1, vec3 v2, material m, vec3 norm);
        void set_tri (unsigned int ti, vec3 v0, vec3 v1, vec3 v2, material m);
        void set_lin (unsigned int li, vec3 v0, vec3 v1, material m);
        void set_pnt (unsigned int pi, vec3 v0, material m);
};


}
