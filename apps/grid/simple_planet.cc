
#include "simple_planet.hh"


#include <cmath>
#include <cstring>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>



terrain_part::terrain_part(const std::string& file, int partsize, float dx, float dy, float s)
{
    usedInNextIter = false;
    size = partsize;
    scale = s;
    pos.set(scale*dx, scale*dy, 0.0f);
    flow_density = 4;
    flowz = 30.0f;
    
    cgeom = nullptr;
    gridgeom = cr::bm1_to_meshux(file);
    gsgl::MUXtoGPU(*gridgeom, gpuGrid);
}
terrain_part::terrain_part(int partsize, float dx, float dy, float s)
{
    usedInNextIter = false;
    size = partsize;
    scale = s;
    pos.set(scale*dx, scale*dy, 0.0f);
    flow_density = 4;
    flowz = 30.0f;
    
    cgeom = nullptr;
    
    init_grid();
}

terrain_part::~terrain_part()
{
    clear();
    delete gridgeom;
    delete flowgeom;
    if (cgeom) delete cgeom;
}

void terrain_part::clear()
{
    //r::clearGPU(gpuPointers);
    //r::clearGPU(gpuFlow);
}

void terrain_part::init_grid()
{
    unsigned int t  = 2*size*size;
    unsigned int l  = 2*size*size;
    gridgeom = new cr::mesh_ux(t, l, 0);
    
    
    unsigned int t2  = 0;
    for (int y=0 ; y<size ; ++y)
    {
        for (int x=0 ; x<size ; ++x)
        {
            if (((y % flow_density) == 0) && ((x % flow_density) == 0)) ++t2;
        }
    }
    flowgeom = new cr::mesh_ux(t2, t2, 0);

    gsgl::MUXtoGPU(*gridgeom, gpuGrid);
    gsgl::MUXtoGPU(*flowgeom, gpuFlow);
}


void terrain_part::setpos(float _x, float _y, float _z)
{
    pos.set(scale*_x, scale*_y, scale*_z);
}


void terrain_part::readpart(simple_planet* planet, int px, int py)
{
    //clear();
    
    int ti = 0, ti2 = 0;
    int li = 0, li2 = 0;
    cr::vec3 v0, v1, v2, v3, norm, zi;
    cr::material col;
    
    col.albedo.x = 0.8f; col.albedo.y = 0.1f; col.albedo.z = 0.0f;
    zi.x = 0.0f; zi.y = 0.0f; zi.z = 1.0f;
    
    for (int y=0 ; y<size ; ++y)
    {
        for (int x=0 ; x<size ; ++x)
        {
            if (((y % flow_density) == 0) && ((x % flow_density) == 0))
            {
                v0.x = float(x);
                v0.y = float(y);
                v0.z = flowz;
                v1.x = v0.x + planet->map->getfx(px*size + x, py*size + y)*5.0f;
                v1.y = v0.y + planet->map->getfy(px*size + x, py*size + y)*5.0f;
                v1.z = flowz;
                
                v2 = cr::cross(zi, (v1 - v0));
                v2.setlength(0.4f);
                v3 = -v2;
                v3 += v0;
                v2 += v0;
                
                flowgeom->set_tri(ti2, v1, v2, v3, col, zi);
                flowgeom->set_lin(li2, v0, v1, col);
                ++ti2;
                ++li2;
            }
            
            v0.x = float(x);
            v0.y = float(y);
            v0.z = planet->map->geth(px*size + x, py*size + y);
            v1.x = x + 1.0f;
            v1.y = float(y);
            v1.z = planet->map->geth(px*size + x + 1, py*size + y);
            v2.x = float(x);
            v2.y = y + 1.0f;
            v2.z = planet->map->geth(px*size + x, py*size + y + 1);
            v3.x = x + 1.0f;
            v3.y = y + 1.0f;
            v3.z = planet->map->geth(px*size + x + 1, py*size + y + 1);
            
            norm = cr::cross( (v1-v0), (v2-v0) );
            norm.normalize();
            
            float zm = std::max(v0.z, std::max(v1.z, std::max(v2.z, v3.z)));
            unsigned int ci = planet->pal->get(zm);
            
            //float zm2 = std::acos( cr::dot(norm, cr::vec3{0,0,1}) );
            //unsigned int ci2 = planet->pal2->get(zm2);
            
            /*
            if (v0.z < 0.0f) v0.z = 0.0f;
            if (v1.z < 0.0f) v1.z = 0.0f;
            if (v2.z < 0.0f) v2.z = 0.0f;
            if (v3.z < 0.0f) v3.z = 0.0f;
            */
            
            //col.albedo = cr::mix(planet->pal->cols[ci], planet->pal2->cols[ci2], 0.3f);
            col.albedo = planet->pal->cols[ci];
            gridgeom->set_tri(ti, v0, v1, v2, col, norm);
            gridgeom->set_tri(ti+1, v1, v3, v2, col, norm);
            ti += 2;
            
            gridgeom->set_lin(li, v0, v1, col);
            gridgeom->set_lin(li+1, v0, v2, col);
            li += 2;
        }
    }
    
    
    //gsgl::clearGPU(gpuGrid);
    gsgl::updateGPU(*gridgeom, gpuGrid);
    
    //std::cout << "GRID: " << gpuGrid.lin_buf << std::endl;
    
    //gsgl::clearGPU(gpuFlow);
    gsgl::updateGPU(*flowgeom, gpuFlow);
}

void terrain_part::readpart_plates(cr::height_pal* pal, ter::terrain_data_square* map, int px, int py)
{
    //clear();
    
    int ti = 0, ti2 = 0;
    int li = 0, li2 = 0;
    cr::vec3 v0, v1, v2, v3, norm, zi;
    cr::material col;
    
    col.albedo.x = 0.8f; col.albedo.y = 0.1f; col.albedo.z = 0.0f;
    zi.x = 0.0f; zi.y = 0.0f; zi.z = 1.0f;
    
    for (int y=0 ; y<size ; ++y)
    {
        for (int x=0 ; x<size ; ++x)
        {
            if (((y % flow_density) == 0) && ((x % flow_density) == 0))
            {
                v0.x = float(x);
                v0.y = float(y);
                v0.z = flowz;
                v1.x = v0.x + map->getfx(px*size + x, py*size + y)*5.0f;
                v1.y = v0.y + map->getfy(px*size + x, py*size + y)*5.0f;
                v1.z = flowz;
                
                v2 = cr::cross(zi, (v1 - v0));
                v2.setlength(0.4f);
                v3 = -v2;
                v3 += v0;
                v2 += v0;
                
                flowgeom->set_tri(ti2, v1, v2, v3, col, zi);
                flowgeom->set_lin(li2, v0, v1, col);
                ++ti2;
                ++li2;
            }
            
            v0.x = x - 0.5f;
            v0.y = y - 0.5f;
            v0.z = map->geth(px*size + x, py*size + y);
            
            //if (v0.z < 0.0f) v0.z = 0.0f;
            
            v1.x = x + 0.5f;
            v1.y = y - 0.5f;
            v1.z = v0.z;
            v2.x = x - 0.5f;
            v2.y = y + 0.5f;
            v2.z = v0.z;
            v3.x = x + 0.5f;
            v3.y = y + 0.5f;
            v3.z = v0.z;
            
            norm = cr::vec3(0.0f, 0.0f, 1.0f);
            //norm.normalize();
            
            //float zm = std::max(v0.z, std::max(v1.z, std::max(v2.z, v3.z)));
            unsigned int ci = pal->get(v0.z);
            
            col.albedo = pal->cols[ci];
            gridgeom->set_tri(ti, v0, v1, v2, col, norm);
            gridgeom->set_tri(ti+1, v1, v3, v2, col, norm);
            ti += 2;
            
            v0.x = float(x);
            v0.y = float(y);
            v0.z = map->geth(px*size + x, py*size + y);
            v1.x = x + 1.0f;
            v1.y = float(y);
            v1.z = map->geth(px*size + x + 1, py*size + y);
            v2.x = float(x);
            v2.y = y + 1.0f;
            v2.z = map->geth(px*size + x, py*size + y + 1);
            
            gridgeom->set_lin(li, v0, v1, col);
            gridgeom->set_lin(li+1, v0, v2, col);
            li += 2;
        }
    }
    
    
    //gsgl::clearGPU(gpuGrid);
    gsgl::updateGPU(*gridgeom, gpuGrid);
    
    //std::cout << "GRID: " << gpuGrid.lin_buf << std::endl;
    
    //gsgl::clearGPU(gpuFlow);
    gsgl::updateGPU(*flowgeom, gpuFlow);
}

void terrain_part::save_contour (const std::string& /*dir*/, const std::string& /*name_base*/)
{
    /*
    std::ofstream output(dir + name_base + ".clines", std::ios::binary);
    
    output.write((char*)&(cgeom->lsize), sizeof(unsigned int));
    unsigned int ls = unsigned int (cgeom->lines.size());
    output.write((char*)&(ls),      sizeof(unsigned int));
    
    output.write((char*)&(cgeom->lines[0]), sizeof(float) * ls * cgeom->lsize);
    
    output.close();
    */
}

void terrain_part::create_contour (cr::height_pal* pal, ter::terrain_data_square* map, int px, int py)
{
    if (cgeom) delete cgeom;
    
    cgeom = new cr::mesh_ux();
    
    cr::vec3 v0, v1, v2, v3, v4, v5, newnorm, norm;
    cr::material col;
    cr::vec3 zi(0.0f, 0.0f, 1.0f);
    
    float dz = 3.0f;
    
    for (float planez = -50.0f ; planez <= 50.1f ; planez += dz)
    {
    for (int y=0 ; y<size ; ++y)
    {
        for (int x=0 ; x<size ; ++x)
        {
            v0.x = float(x);
            v0.y = float(y);
            v0.z = map->geth(px*size + x, py*size + y);
            v1.x = x + 1.0f;
            v1.y = float(y);
            v1.z = map->geth(px*size + x + 1, py*size + y);
            v2.x = float(x);
            v2.y = y + 1.0f;
            v2.z = map->geth(px*size + x, py*size + y + 1);
            v3.x = x + 1.0f;
            v3.y = y + 1.0f;
            v3.z = map->geth(px*size + x + 1, py*size + y + 1);
            
            norm = cr::cross( (v1-v0), (v2-v0) );
            norm.normalize();
            
            //int z0 = cr::zero(planez-v0.z, 0.01f) ? 0 : ((planez > v0.z) ? -1 : 1 );
            //int z1 = cr::zero(planez-v1.z, 0.01f) ? 0 : ((planez > v1.z) ? -1 : 1 );
            //int z2 = cr::zero(planez-v2.z, 0.01f) ? 0 : ((planez > v2.z) ? -1 : 1 );
            //int z3 = cr::zero(planez-v3.z, 0.01f) ? 0 : ((planez > v3.z) ? -1 : 1 );
            
            int z0 = ((planez > v0.z) ? -1 : 1 );
            int z1 = ((planez > v1.z) ? -1 : 1 );
            int z2 = ((planez > v2.z) ? -1 : 1 );
            int z3 = ((planez > v3.z) ? -1 : 1 );
            
            float zm = std::max(v0.z, std::max(v1.z, std::max(v2.z, v3.z)));
            unsigned int ci = pal->get(zm);
            
            bool intersect = false;
            

            if ((z0 == 1 && z1 == -1) || (z0 == -1 && z1 == 1))
            {
                v4 = mix(v0, v1, cr::abs(planez-v0.z) / cr::abs(v0.z-v1.z));
                if (z2 == z0)
                {
                    v5 = mix(v1, v2, cr::abs(planez-v1.z) / cr::abs(v2.z-v1.z));
                }
                else
                {
                    v5 = mix(v0, v2, cr::abs(planez-v0.z) / cr::abs(v2.z-v0.z));
                }
                
                newnorm = cr::cross((v4-v5), zi);
                newnorm.normalize();
                
                intersect = true;
                
            }
            if ((z0 == 1 && z2 == -1) || (z0 == -1 && z2 == 1))
            {
                v4 = mix(v0, v2, cr::abs(planez-v0.z) / cr::abs(v0.z-v2.z));
                if (z1 == z0)
                {
                    v5 = mix(v1, v2, cr::abs(planez-v1.z) / cr::abs(v2.z-v1.z));
                }
                else
                {
                    v5 = mix(v0, v1, cr::abs(planez-v0.z) / cr::abs(v1.z-v0.z));
                }
                
                newnorm = cr::cross((v5-v4), zi);
                newnorm.normalize();
                
                intersect = true;
            }
            if ((z1 == 1 && z2 == -1) || (z1 == -1 && z2 == 1))
            {
                v4 = mix(v1, v2, cr::abs(planez-v1.z) / cr::abs(v1.z-v2.z));
                if (z0 == z1)
                {
                    v5 = mix(v0, v2, cr::abs(planez-v0.z) / cr::abs(v0.z-v2.z));
                }
                else
                {
                    v5 = mix(v0, v1, cr::abs(planez-v0.z) / cr::abs(v1.z-v0.z));
                }
                
                newnorm = cr::cross((v5-v4), zi);
                newnorm.normalize();
                
                intersect = true;
            }
            
            if (intersect)
            {
                if (!same_facing(norm, newnorm))
                {
                    newnorm = -newnorm;
                }
                
                cr::vec3 v4z(v4.x, v4.y, planez - dz);
                cr::vec3 v5z(v5.x, v5.y, planez - dz);
                
                col.albedo = pal->cols[ci];
                cgeom->add_tri(v5, v4,  v4z, col, newnorm);
                cgeom->add_tri(v5, v4z, v5z, col, newnorm);
                
                if (planez < 0.0f) {v4.z = 0.0f; v5.z=0.0f;}
                cgeom->add_lin(v4, v5, col);
            }
            
            //-------
            //-------
            //-------
            //-------
            //-------
            v0 = v3;
            z0 = z3;
            intersect = false;
            
            if ((z0 == 1 && z1 == -1) || (z0 == -1 && z1 == 1))
            {
                v4 = mix(v0, v1, cr::abs(planez-v0.z) / cr::abs(v0.z-v1.z));
                if (z2 == z0)
                {
                    v5 = mix(v1, v2, cr::abs(planez-v1.z) / cr::abs(v2.z-v1.z));
                }
                else
                {
                    v5 = mix(v0, v2, cr::abs(planez-v0.z) / cr::abs(v2.z-v0.z));
                }
                
                newnorm = cr::cross((v5-v4), zi);
                newnorm.normalize();
                
                intersect = true;
            }
            if ((z0 == 1 && z2 == -1) || (z0 == -1 && z2 == 1))
            {
                v4 = mix(v0, v2, cr::abs(planez-v0.z) / cr::abs(v0.z-v2.z));
                if (z1 == z0)
                {
                    v5 = mix(v1, v2, cr::abs(planez-v1.z) / cr::abs(v2.z-v1.z));
                }
                else
                {
                    v5 = mix(v0, v1, cr::abs(planez-v0.z) / cr::abs(v1.z-v0.z));
                }
                
                newnorm = cr::cross((v4-v5), zi);
                newnorm.normalize();
                
                intersect = true;
            }
            if ((z1 == 1 && z2 == -1) || (z1 == -1 && z2 == 1))
            {
                v4 = mix(v1, v2, cr::abs(planez-v1.z) / cr::abs(v1.z-v2.z));
                if (z0 == z1)
                {
                    v5 = mix(v0, v2, cr::abs(planez-v0.z) / cr::abs(v0.z-v2.z));
                }
                else
                {
                    v5 = mix(v0, v1, cr::abs(planez-v0.z) / cr::abs(v1.z-v0.z));
                }
                
                newnorm = cr::cross((v4-v5), zi);
                newnorm.normalize();
                
                intersect = true;
            }
            
            if (intersect)
            {
                if (!same_facing(norm, newnorm))
                {
                    newnorm = -newnorm;
                }
                
                cr::vec3 v4z(v4.x, v4.y, planez - dz);
                cr::vec3 v5z(v5.x, v5.y, planez - dz);
                
                col.albedo = pal->cols[ci];
                cgeom->add_tri(v5, v4,  v4z, col, newnorm);
                cgeom->add_tri(v5, v4z, v5z, col, newnorm);
                
                if (planez < 0.0f) {v4.z = 0.0f; v5.z=0.0f;}
                cgeom->add_lin(v4, v5, col);
            }
            
            
            
            //if (z0 == 0 && z1 == 0 && z2 != 0) {}
            //if (z0 == 0 && z1 != 0 && z2 == 0) {}
            //if (z0 != 0 && z1 == 0 && z2 == 0) {}
        }
    }
    }
    
    gsgl::MUXtoGPU(*cgeom, gpuGrid);
    //gsgl::MBtoGPU(grdgeom, cgeom, gpuGrid);
    //gsgl::hardUpdateGPU(gridgeom, cgeom, gpuGrid);
}

/*
simple_planet::simple_planet(const std::string& mapfile)
{
    base_dir = cr::spath(cr::get_folder(mapfile));
    
    tilesize = 100.0f;
    scale = 1.0f / 100.0f;
    gap = 0.0f;
    pw = 0;
    ph = 0;
    //w = 3;
    //h = 3;
    //xo1 = (-w) / 2;
    //yo1 = (-h) / 2;
    partFiles = 0;
    
    string line;
    ifstream file(mapfile);
    if (!file) {
        cout << "ERROR: simple_planet: file not exists: " << mapfile << endl;
    }
    
    getline(file,line);
    istringstream buffer0(line);
    buffer0 >> tilesize;
    scale = 1.0f / tilesize;
    getline(file,line);
    istringstream buffer1(line);
    buffer1 >> pw;
    getline(file,line);
    istringstream buffer2(line);
    buffer2 >> ph;
    
    if (pw < 1 || ph < 1) {
        cout << "ERROR: simple_planet: " << mapfile << " wrong dimensions" << endl;
        file.close();
        return;
    }
    partFiles = new string[pw*ph];
    int i=0;
    while (getline(file,line)) {
        if (line.length() == 0 || line.at(0) == '#') continue;
        if (i >= pw*ph) break;
        partFiles[i] = line;
        i++;
    }
    if (i < pw*ph) {
        cout << "ERROR: simple_planet: " << mapfile << " has not enough parts." << endl;
        file.close();
        return;
    }
    file.close();
    
    parts = new terrain_part*[pw*ph];
    //parts.reserve(w*h);
    //newparts.reserve(w*h);
    //for (int i=0 ; i<pw*ph ; ++i)
    //{
    //    parts[i] = 0;
    //}
    initterrain_parts();
}
*/
simple_planet::simple_planet(int p_partsize, int p_x, int p_y)
{
    contoured = false;
    
    tilesize = (float)p_partsize;
    scale = 1.0f / tilesize;
    gap = 0.0f;
    partsize = p_partsize;
    pw = p_x;
    ph = p_y;
    
    partFiles=0;
    parts=0;
    if (pw<1 || ph<1 || partsize < 1)
    {
        std::cerr << "ERROR: simple_planet dimensions" << std::endl;
        return;
    }
    
    //w = 3;
    //h = 3;
    //xo1 = (-w) / 2;
    //yo1 = (-h) / 2;
    
    //partFiles = new string[pw*ph];
    // TODO
    
    W = partsize*pw + 1;
    H = partsize*ph + 1;
    map     = new ter::terrain_data_square(W, H);
    map_tmp = new ter::terrain_data_square(W, H);
    idicew  = new cr::idice(120, 0, W-1);
    idiceh  = new cr::idice(123, 0, H-1);
    fd      = new cr::fdice(120, -1.0f, 1.0f);
    
    /*
    cr::height_pal* MAT = new cr::height_pal(6);
    MAT->cols[0] = {0.0f ,  0.0f ,  0.2f};
    MAT->cols[1] = {0.235f, 0.435f, 1.0f};
    MAT->cols[2] = {0.592f, 0.423f, 0.235f};
    MAT->cols[3] = {0.156f, 0.361f, 0.058f};
    MAT->cols[4] = {0.08f,  0.20f,  0.09f};
    MAT->cols[5] = {0.529f, 0.529f, 0.529f};
    MAT->limits[0] = -23.0f;
    MAT->limits[1] =  -4.0f;
    MAT->limits[2] =   0.0f;
    MAT->limits[3] =   7.0f;
    MAT->limits[4] =  40.0f;
    
    pal = new cr::height_pal(*MAT, 4);
    */
    
    /*
    cr::height_pal* MAT2 = new cr::height_pal(2);
    MAT2->cols[0] = {0.08f ,  0.33f ,  0.06f};
    //MAT2->cols[1] = {0.3f, 0.25f, 0.09f};
    MAT2->cols[1] = {0.60f, 0.80f, 1.00f};
    //MAT2->cols[3] = {0.50f, 0.70f, 0.90f};
    MAT2->limits[0] = 0.5f;
    //MAT2->limits[1] = 0.8f;
    //MAT2->limits[2] = 0.8f;
    
    pal2 = new cr::height_pal(*MAT2, 4);
    */
    
    
    cr::height_pal* MAT = new cr::height_pal(3);
    MAT->cols[0] = {0.0f , 0.0f , 0.2f};
    //MAT->cols[1] = {0.235f, 0.435f, 0.8f};
    MAT->cols[1] = {0.24f, 0.46f, 0.86f};
    //MAT->cols[2] = {0.2f, 0.4f, 1.0f};
    MAT->cols[2] = {0.33f, 0.50f, 1.0f};
    MAT->limits[0] = -22.0f;
    MAT->limits[1] =  16.0f;
    
    pal = new cr::height_pal(*MAT, 8);
    
    
    noisegen = new ter::noise();
    noisegen->xscale = 15.0f;
    noisegen->yscale = 15.0f;
    
    parts = new terrain_part*[pw*ph];
    for (int j=0 ; j<ph ; ++j)
    {
        for (int i=0 ; i<pw ; ++i)
        { 
            parts[j*pw+i] = new terrain_part(p_partsize,
                                            (tilesize+gap)*(i), (tilesize+gap)*(j),
                                            scale);
            //parts[j*pw+i]->readpart(pal, map, i, j);
        }
    }
    updateGpu(true);
}

simple_planet::~simple_planet()
{
    delete idicew;
    delete idiceh;
    delete fd;
    delete noisegen;
    delete pal;
    delete map;
    delete map_tmp;
    delete[] partFiles;
    if (parts != 0)
    {
        for (int i=0 ; i<pw*ph ; ++i)
        {
            delete parts[i];
            parts[i] = 0;
        }
        delete[] parts;
    }
}

void simple_planet::reset ()
{
    map->clear();
    map_tmp->clear();
    
    for (int j=0 ; j<ph ; ++j)
    {
        for (int i=0 ; i<pw ; ++i)
        { 
            parts[j*pw+i]->readpart(this, i, j);
        }
    }
    updateGpu(true);
}

/*
void simple_planet::modWindowAndPosition()
{
    if (xo1 > pw || (xn1 + w - 1) < 0)
    {
        xo1 = cr::modi(xo1, pw);
        //xo2 = xo1 + w - 1;
    }
    
    if (yo1 > ph || (yn1 + h - 1) < 0)
    {
        yo1 = cr::modi(yo1, ph);
        //yo2 = yo1 + h - 1;
    }
}
*/

/*
void simple_planet::initterrain_parts()
{
    int i2, j2;
    for (int j=0 ; j<h ; ++j) {
        for (int i=0 ; i<w ; ++i) {
            
            i2 = cr::modi(xo1+i, pw);
            j2 = cr::modi(yo1+j, ph);
            //cout << i2 << "***" << j2 << endl;
            parts[j*w+i] = new terrain_part(base_dir + partFiles[j2*pw + i2],
                                           (tilesize+gap)*(xo1+i), (tilesize+gap)*(yo1+j),
                                           scale);
        }
    }
}
*/
/*
void simple_planet::initterrain_parts()
{
    for (int j=0 ; j<ph ; ++j)
    {
        for (int i=0 ; i<pw ; ++i)
        { 
            parts[j*w+i] = new terrain_part(base_dir + partFiles[j*pw + i],
                                           (tilesize+gap)*(xo1+i), (tilesize+gap)*(yo1+j),
                                           scale);
        }
    }
}

void simple_planet::setdim (int _w, int _h)
{
    if (_w > 0 && _h > 0)
    {
        xo1 += (w-_w)/2;
        yo1 += (h-_h)/2;
        for (int i=0 ; i<w*h ; ++i) {
            delete parts[i];
        }
        delete[] parts;
        parts = 0;
        w = _w;
        h = _h;
        updateterrain_parts();
    }
}
*/
/*
void simple_planet::more ()
{
    if (w < 100 && h < 100)
    {
        setdim(w+1, h+1);
    }
}

void simple_planet::less ()
{
    if (w > 0 && h > 0)
    {
        setdim(w-1, h-1);
    }
}
*/

void simple_planet::save(const std::string& dir, const std::string& name_base)
{
    std::ofstream output(dir + name_base + ".txt");
    output << tilesize << std::endl;
    output << ph << std::endl;
    output << pw << std::endl;

    for (int j=0 ; j<ph ; ++j)
    {
        for (int i=0 ; i<pw ; ++i)
        {
            std::stringstream ss("");
            ss << name_base << "_" << j << "_" << i;
            output << ss.str() << ".bm1" << std::endl;
            savepart(dir, ss.str(), i, j);
        }
    }
    output.close();
    std::cout << "Saved: " << (dir + name_base + ".txt") << std::endl;
}

void simple_planet::savepart (const std::string& dir, const std::string& name, int i, int j)
{
    std::ofstream output(dir + name + ".bm1", std::ios::binary);
    
    unsigned int Wu  = partsize;
    unsigned int PWu = pw;
    unsigned int pyu = j;
    unsigned int pxu = i;
    
    unsigned int ui0, ui1, ui2;
    float f0, f1, f2;
    
    ui0 = 1;
    output.write((char*)&ui0, sizeof(unsigned int));
    ui0 = pal->colnum;
    output.write((char*)&ui0, sizeof(unsigned int));   // M
    ui0 = (Wu+1)*(Wu+1);
    output.write((char*)&ui0, sizeof(unsigned int)); // V
    ui0 = (Wu)*(Wu)*2;
    output.write((char*)&ui0, sizeof(unsigned int)); // T
    output.write((char*)&ui0, sizeof(unsigned int)); // L
    
    for (int ii = 0 ; ii<pal->colnum ; ++ii)
    {
        f0 = pal->cols[ii].x;
        f1 = pal->cols[ii].y;
        f2 = pal->cols[ii].z;
        output.write((char*)&f0, sizeof(float));
        output.write((char*)&f1, sizeof(float));
        output.write((char*)&f2, sizeof(float));
    }
    
    float yc = 0.0f;
    for (unsigned int y=pyu*Wu ; y<=(pyu+1)*Wu ; ++y)
    {
        float xc = 0.0f;
        for (unsigned int x=pxu*Wu ; x<=(pxu+1)*Wu ; ++x)
        {
            //f0 = xc*scale;
            //f1 = yc*scale;
            f0 = xc;
            f1 = yc;
            f2 = map->height[y*(Wu*PWu+1) + x];
            if (f2 < 0.0f) f2 = 0.0f;
            output.write((char*)&f0, sizeof(float));
            output.write((char*)&f1, sizeof(float));
            output.write((char*)&f2, sizeof(float));
            
            xc += 1.0f;
        }
        yc += 1.0f;
    }
    
    for (unsigned int y=0 ; y<(Wu) ; ++y)
    {
        for (unsigned int x=0 ; x<(Wu) ; ++x)
        {
            ui0 = (y+1)*(Wu+1)+x;
            ui1 =  y*(Wu+1)+x;
            ui2 =  y*(Wu+1)+x+1;
            output.write((char*)&ui0, sizeof(unsigned int));
            output.write((char*)&ui1, sizeof(unsigned int));
            output.write((char*)&ui2, sizeof(unsigned int));
            
            ui0 = y*(Wu+1)+x+1;
            ui1 = (y+1)*(Wu+1)+x+1;
            ui2 = (y+1)*(Wu+1)+x;
            output.write((char*)&ui0, sizeof(unsigned int));
            output.write((char*)&ui1, sizeof(unsigned int));
            output.write((char*)&ui2, sizeof(unsigned int));
        }
    }
    
    for (unsigned int y=0 ; y<(Wu) ; ++y)
    {
        for (unsigned int x=0 ; x<(Wu) ; ++x)
        {
            f0 = std::max(map->height[(pyu*Wu + y+1)*(Wu*PWu+1)+x+1 + pxu*Wu],
                 std::max(map->height[(pyu*Wu + y+1)*(Wu*PWu+1)+x + pxu*Wu],
                 std::max(map->height[(pyu*Wu + y)*(Wu*PWu+1)+x + pxu*Wu],
                          map->height[(pyu*Wu + y)*(Wu*PWu+1)+x+1 + pxu*Wu])));
            ui0 = pal->get(f0);
            output.write((char*)&ui0, sizeof(unsigned int));
            output.write((char*)&ui0, sizeof(unsigned int));
        }
    }
    
    
    
    for (unsigned int y=0 ; y<(Wu) ; ++y)
    {
        for (unsigned int x=0 ; x<(Wu) ; ++x)
        {
            ui0 = (y+1)*(Wu+1)+x;
            ui1 = y*(Wu+1)+x;
            output.write((char*)&ui0, sizeof(unsigned int));
            output.write((char*)&ui1, sizeof(unsigned int));
            
            ui0 = y*(Wu+1)+x;
            ui1 = y*(Wu+1)+x+1;
            output.write((char*)&ui0, sizeof(unsigned int));
            output.write((char*)&ui1, sizeof(unsigned int));
        }
    }
    
    for (unsigned int y=0 ; y<(Wu) ; ++y)
    {
        for (unsigned int x=0 ; x<(Wu) ; ++x)
        {
            f0 = std::max(map->height[(pyu*Wu + y+1)*(Wu*PWu+1)+x+1 + pxu*Wu],
                 std::max(map->height[(pyu*Wu + y+1)*(Wu*PWu+1)+x + pxu*Wu],
                 std::max(map->height[(pyu*Wu + y)*(Wu*PWu+1)+x + pxu*Wu],
                          map->height[(pyu*Wu + y)*(Wu*PWu+1)+x+1 + pxu*Wu])));
            ui0 = pal->get(f0);
            output.write((char*)&ui0, sizeof(unsigned int));
            output.write((char*)&ui0, sizeof(unsigned int));
        }
    }
    
    output.close();
    
    if (contoured)
    {
        parts[j*pw+i]->save_contour(dir, name);
    }
}

void simple_planet::create_contour ()
{
    contoured = true;
    
    for (int j=0 ; j<ph ; ++j)
    {
        for (int i=0 ; i<pw ; ++i)
        {
            parts[j*pw+i]->create_contour(pal, map, i, j);
        }
    }
}

void simple_planet::updateGpu (bool /*slice*/)
{
    for (int j=0 ; j<ph ; ++j)
    {
        for (int i=0 ; i<pw ; ++i)
        {
            parts[j*pw+i]->readpart(this, i, j);
            //parts[j*pw+i]->readpart_plates(pal, map, i, j);
        }
    }
}

void simple_planet::generate_ds (bool slice, bool dsmap)
{
    contoured = false;
    
    const float corners = -15.0f;
    
    if (dsmap)
    {
        int pyi = 0;
        for (int py=0 ; py<(H) ; py+=partsize)
        {
            int pxi = 0;
            for (int px=0 ; px<(W) ; px+=partsize)
            {
                map->height[py*W + px] = ((pxi+pyi) % 2 == 0) ? -20.0f : 28.0f;
                //map->height[py*W + px] = -15.0f + pyi*10.0f;
                ++pxi;
            }
            ++pyi;
        }

        for (int py=0 ; py<H ; ++py)
        {
            map->height[py*W + 0]   = corners;
            map->height[py*W + W-1] = corners;
        }
        for (int px=0 ; px<W ; ++px)
        {
            map->height[0*W + px]       = corners;
            map->height[(W-1)*W + px] = corners;
        }
    }
    
    ter::ds_square_grid(map->height, partsize, pw, ph);
    updateGpu(slice);
}

void simple_planet::crack ()
{
    contoured = false;
    
    for (int i=0 ; i<100 ; ++i)
    {
    cr::vec2 p1(float(idicew->next()), float(idiceh->next()));
    cr::vec2 p2(float(idicew->next()), float(idiceh->next()));
    cr::line2 line;
    line.set2p(p1, p2);
    cr::vec2 v;
    
    //ter::domain_warp_1(map->height, W, H);
    for (int x=0 ; x<W ; ++x)
    {
        for (int y=0 ; y<H ; ++y)
        {
            v.x = float(x); v.y = float(y);
            if (line.side(v) > 0.0f)
            {
                map->height[y*W + x] += 0.5f;
            }
            else
            {
                map->height[y*W + x] -= 0.5f;
            }
        }
    }
    }
    updateGpu(true);
}

void simple_planet::generate_warp (bool slice)
{
    contoured = false;
    
    //ter::domain_warp_1(map->height, W, H);
    for (int x=0 ; x<W ; ++x)
    {
        for (int y=0 ; y<H ; ++y)
        {
            //map->height[y*W + x] = noisegen->base(x, y);
            map->height[y*W + x] = 1.5f * noisegen->domain_warp(float(x), float(y));
        }
    }
    edgefit(32);
    updateGpu(slice);
}

void simple_planet::erosion_step ()
{
    contoured = false;
    ter::erosion_hydraulic(map);
}
void simple_planet::erosion_2_step ()
{
    contoured = false;
    ter::erosion_talus(map->height, W, H, 0.1f);
}

void simple_planet::quantize(float q)
{
    ter::quantize(map->height, W, H, q);
    updateGpu(false);
}

void simple_planet::filter ()
{
    contoured = false;
    ter::laplacian2(map->height, map_tmp->height, W, H);
    updateGpu(false);
}

void simple_planet::golinit ()
{
    contoured = false;
    ter::game_of_life_init(map->height, map_tmp->height, W, H);
    updateGpu(false);
}

void simple_planet::gol ()
{
    contoured = false;
    ter::game_of_life(map->height, map_tmp->height, W, H);
    updateGpu(false);
}

void simple_planet::rdinit ()
{
    contoured = false;
    //rdcalc2();
    ter::reaction_diffusion_init(map->sediment1, map_tmp->sediment1, map->sediment2, map_tmp->sediment2, W, H, 10.0f);
    updateGpu(false);
}

void simple_planet::rd ()
{
    contoured = false;
    for (int i=0 ; i<200 ; ++i)
    {
        ter::reaction_diffusion(map->sediment1, map_tmp->sediment1, map->sediment2, map_tmp->sediment2, W, H, 10.0f);
    }
    updateGpu(false);
}

void simple_planet::rdcalc ()
{
    contoured = false;
    for (int x=0 ; x<W ; ++x)
    {
        for (int y=0 ; y<H ; ++y)
        {
            //map->height[y*W + x] = map->sediment1[y*W + x] * map->sediment2[y*W + x] / 40.0f;
            map->height[y*W + x] = map->sediment1[y*W + x];
        }
    }
    updateGpu(false);
}
void simple_planet::rdcalc2 ()
{
    for (int x=0 ; x<W ; ++x)
    {
        for (int y=0 ; y<H ; ++y)
        {
            map->sediment1[y*W + x] =  map->height[y*W + x];
            map->sediment2[y*W + x] = -map->height[y*W + x];
            
            map_tmp->sediment1[y*W + x] =  map->height[y*W + x];
            map_tmp->sediment2[y*W + x] = -map->height[y*W + x];
        }
    }
}

void simple_planet::edgefit (int k)
{
    float a, b, h1, h2;
    float kf = 2.0f;
    for (int i=0 ; i<W ; ++i)
    {
        for (int ki=0 ; ki<k ;++ki)
        {
            a = 1.0f / kf;
            b = 1.0f - a;
            h1 = map->height[ki*W       + i];
            h2 = map->height[(H-1-ki)*W + i];
            map->height[ki*W       + i] = a * h2 + b * h1;
            map->height[(H-1-ki)*W + i] = a * h1 + b * h2;
            kf *= 2.0f;
        }
    }
    
    for (int i=0 ; i<H ; ++i)
    {
        for (int ki=0 ; ki<k ;++ki)
        {
            a = 1.0f / (ki+2);
            b = 1.0f - a;
            h1 = map->height[i*W + ki];
            h2 = map->height[i*W + H-1-ki];
            map->height[i*W + ki]     = a * h2 + b * h1;
            map->height[i*W + H-1-ki] = a * h1 + b * h2;
        }
    }
    
    updateGpu(false);
}

void simple_planet::stat ()
{
    hmin = map->height[0];
    hmax = map->height[0];
    
    for (int x=0 ; x<W ; ++x)
    {
        for (int y=0 ; y<H ; ++y)
        {
            if (map->height[y*W+x] > hmax) hmax = map->height[y*W+x];
            if (map->height[y*W+x] < hmin) hmin = map->height[y*W+x];
        }
    }
    
    std::cout << "hmin: " << std::fixed << std::setprecision(3) << std::setw(9) << hmin << ",   hmax: " << std::setw(9) << hmax << std::endl;
}

void simple_planet::moregap ()
{
    if (gap < 100.0f)
    {
        gap += 1.0f;
        for (int j=0 ; j<ph ; ++j)
        {
            for (int i=0 ; i<pw ; ++i)
            {
                parts[j*pw+i]->setpos((tilesize+gap)*(i), (tilesize+gap)*(j), 0.0f);
            }
        }
    }
}

void simple_planet::lessgap ()
{
    if (gap >= 0.999f)
    {
        gap -= 1.0f;
        for (int j=0 ; j<ph ; ++j)
        {
            for (int i=0 ; i<pw ; ++i)
            {
                parts[j*pw+i]->setpos((tilesize+gap)*(i), (tilesize+gap)*(j), 0.0f);
            }
        }
    }
}

/*
void simple_planet::updateterrain_parts ()
{
    if (parts != 0)
    {
        for (int i=0 ; i<w*h ; ++i) {
            delete parts[i];
        }
    }
    else
    {
        parts = new terrain_part*[w*h];
    }
    
    int i2, j2;
    for (int j=0 ; j<h ; ++j)
    {
        for (int i=0 ; i<w ; ++i)
        {
                i2 = cr::modi(xn1+i, pw);
                j2 = cr::modi(yn1+j, ph);
                //cout << i2 << "***" << j2 << endl;
                parts[j*w+i] = new terrain_part(base_dir + partFiles[j2*pw + i2],
                                                  (tilesize + gap)*(xo1+i), (tilesize + gap)*(yo1+j),
                                                  scale);
        }
    }
}

void simple_planet::updateterrain_parts (int x, int y)
{
    //float d = tilesize*scale/2.0f;
    int xm = cr::modi(x, pw);
    int ym = cr::modi(y, pw);
    xn1 = xm - w/2; //xn2 = xn1 + w - 1;
    yn1 = ym - h/2; //yn2 = yn1 + h - 1; 
    
    updateterrain_parts();
}
*/
