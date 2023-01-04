
#pragma once

#include <string>

#include <crcc/crcc.hh>
#include <gsgl/gsgl.hh>


class planet
{
    public:
        cr::mesh_ux*    geom_cpu;
        cr::rrr_buffers geom_gpu;
        
        cr::vec3 pos;
        cr::vec3 look;
        cr::vec3 up;
        float scale;
    
    public:
         planet(const std::string& geomfile);
        ~planet();
};
