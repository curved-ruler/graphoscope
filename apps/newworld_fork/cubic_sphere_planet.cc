
#include "cubic_sphere_planet.hh"

#include <cmath>
#include <iostream>


terrain_part::terrain_part(cubic_sphere_planet* _planet, cspart_id _id) : planet(_planet), id(_id)
{
    int step = (id.lod >= 0) ? (1 << id.lod) : 1;
    int step_size = _planet->part_size / step;
    //std::cout << "step: " << step << ",   step_size: " << step_size << std::endl;
    
    geom = new cr::mesh_ux(2*(step)*(step), 2*(step)*(step), 0);
    
    /*
    int vertical1 = id.x0    + (1*(id.side & 1))
    int vertical2 = id.x0 +  + (1*(id.side & 2))
    
    int horizontal4 = id.y0  + (1*(id.side & 4))
    int horizontal8 = id.y0  + (1*(id.side & 8))
    */
    int x = id.x0;
    int y = id.y0;
    cr::vec3 a,b,c,d,n;
    cr::material col; col.albedo={ 1.0f, 0.62f, 0.0f };
    unsigned int l=0,t=0;
    for (int j=0 ; j<step ; ++j)
    {
        y = id.y0;
        for (int i=0 ; i<step ; ++i)
        {
            planet->xy_to_3d(a, id, x, y);
            planet->xy_to_3d(b, id, x+step_size, y);
            planet->xy_to_3d(c, id, x, y+step_size);
            planet->xy_to_3d(d, id, x+step_size, y+step_size);
            
            n = cross((b-a), (c-a)) * planet->base[static_cast<int>(id.c)].n;
            //n.normalize();
            geom->set_tri(t, c, b, a, col, n); ++t;
            geom->set_tri(t, c, d, b, col, n); ++t;
            
            geom->set_lin(l, a, b, col); ++l;
            geom->set_lin(l, a, c, col); ++l;
            
            y += step_size;
        }
        x += step_size;
    }
    
    gsgl::MUXtoGPU(*geom, gpuPointers);
}

terrain_part::~terrain_part()
{
    //delete[] data;
    delete geom;
    gsgl::clearGPU(gpuPointers);
}

void terrain_part::draw (const cr::camera& c, cr::renderer* rrr) const
{
    rrr->render(c, planet->model_mat(), gpuPointers);
}
void terrain_part::draw_cpu (const cr::camera& c, cr::renderer* rrr) const
{
    rrr->render(c, planet->model_mat(), *geom);
}

cubic_sphere_planet::cubic_sphere_planet (const cr::vec_placed& _place, int _part_size, int _part_n, int _full_lod, int _lod)
  : vec_placed(_place)
  , part_size(_part_size)
  , part_n   (_part_n)
  , full_lod (_full_lod)
  , lod(_lod)
{
    
    radius = cr::sqrt3 * (part_size * part_n) / 2.0f;
    
    const float a = (part_size * part_n) / 2.0f;
    
    //                                           p                     u                    v                 n
    base[static_cast<int>(comp::north)] = { cr::vec3(-a,  a,  a), cr::vec3( 0, -1, 0), cr::vec3( 1, 0,  0),  1.0f };
    base[static_cast<int>(comp::front)] = { cr::vec3( a,  a,  a), cr::vec3( 0, -1, 0), cr::vec3( 0, 0, -1),  1.0f };
    base[static_cast<int>(comp::east)]  = { cr::vec3( a, -a,  a), cr::vec3(-1,  0, 0), cr::vec3( 0, 0, -1),  1.0f };
    base[static_cast<int>(comp::south)] = { cr::vec3( a, -a, -a), cr::vec3( 0,  1, 0), cr::vec3(-1, 0,  0), -1.0f };
    base[static_cast<int>(comp::back)]  = { cr::vec3(-a, -a, -a), cr::vec3( 0,  1, 0), cr::vec3( 0, 0,  1), -1.0f };
    base[static_cast<int>(comp::west)]  = { cr::vec3(-a,  a, -a), cr::vec3( 1,  0, 0), cr::vec3( 0, 0,  1), -1.0f };
    
    load_terrain();
}

cubic_sphere_planet::~cubic_sphere_planet ()
{
    for (unsigned int i=0 ; i<terrain.size() ; ++i) delete terrain[i];
}

void cubic_sphere_planet::xy_to_3d(cr::vec3& a, const cspart_id& id, int x, int y)
{
    float alphax = (cr::pi / (2*part_size*part_n)) * x;
    float alphay = (cr::pi / (2*part_size*part_n)) * y;
    float betax = cr::pi / 4.0f - alphax;
    float betay = cr::pi / 4.0f - alphay;
    float lambdax = (1.0f - std::tan(betax)) * (part_size*part_n/2.0f);
    float lambday = (1.0f - std::tan(betay)) * (part_size*part_n/2.0f);
    a = base[static_cast<int>(id.c)].p + base[static_cast<int>(id.c)].u*lambdax + base[static_cast<int>(id.c)].v*lambday;
    a.setlength(radius);
}

void cubic_sphere_planet::load_terrain()
{
    if (lod < full_lod)
    {
        for (int i=0 ; i<part_n ; ++i)
        {
            for (int j=0 ; j<part_n ; ++j)
            {
                //for (int c = 0 ; c < 3 ; ++c)
                for (int c = 0 ; c < static_cast<int>(comp::count) ; ++c)
                {
                    cspart_id id;
                    id.c   = static_cast<comp>(c);
                    id.lod = lod;
                    id.x0  = j*part_size;
                    id.y0  = i*part_size;
                    //id.sides = ...
                    terrain_part* part = new terrain_part(this, id);
                    terrain.push_back(part);
                }
            }
        }
    }
}

void cubic_sphere_planet::draw (const cr::camera& cam, cr::renderer* rrr) const
{
    for (unsigned int i=0 ; i<terrain.size() ; ++i) terrain[i]->draw(cam, rrr);
}
void cubic_sphere_planet::draw_cpu (const cr::camera& cam, cr::renderer* rrr) const
{
    for (unsigned int i=0 ; i<terrain.size() ; ++i) terrain[i]->draw_cpu(cam, rrr);
}

