
#include "planet.hh"


planet::planet(const std::string& geomfile)
{
    geom_cpu = cr::bm1_to_meshux(geomfile);
    gsgl::MUXtoGPU(*geom_cpu, geom_gpu);
}
planet::~planet()
{
    delete geom_cpu;
    gsgl::clearGPU(geom_gpu);
}
