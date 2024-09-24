
#pragma once

#include <vector>
#include "../base.hh"
#include "../math/mu.hh"
#include "material.hh"


namespace cr {


class mesh_ux
{
    public:
        std::vector<float> triangles;
        std::vector<float> lines;
        std::vector<float> points;
        
        uint32 tsize, lsize, psize;
        bool tnorm, lnorm, pnorm;
        
        
    public:
         mesh_ux (bool tn, bool ln, bool pn);
         mesh_ux (uint32 t, bool tn, uint32 l, bool ln, uint32 p, bool pn);
        ~mesh_ux ();
        
        void clear();
        
        void add_tri_n (vec3 v0, vec3 v1, vec3 v2, material m, vec3 norm);
        void add_lin_n (vec3 v0, vec3 v1, material m, vec3 norm);
        void add_pnt_n (vec3 v0, material m, vec3 norm);
        void add_tri (vec3 v0, vec3 v1, vec3 v2, material m);
        void add_lin (vec3 v0, vec3 v1, material m);
        void add_pnt (vec3 v0, material m);
        
        void set_tri_n (uint32 ti, vec3 v0, vec3 v1, vec3 v2, material m, vec3 norm);
        void set_lin_n (uint32 li, vec3 v0, vec3 v1, material m, vec3 norm);
        void set_pnt_n (uint32 pi, vec3 v0, material m, vec3 norm);
        void set_tri (uint32 ti, vec3 v0, vec3 v1, vec3 v2, material m);
        void set_lin (uint32 li, vec3 v0, vec3 v1, material m);
        void set_pnt (uint32 pi, vec3 v0, material m);
};


}
