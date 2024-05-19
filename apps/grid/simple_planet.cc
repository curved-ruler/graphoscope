
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
    unsigned int p  = size*size;
    gridgeom = new cr::mesh_ux(t, l, p);
    
    
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
    int pi = 0;
    cr::vec3 v0, v1, v2, v3, norm, zi;
    cr::material col;
    
    col.albedo.x = 0.8f; col.albedo.y = 0.1f; col.albedo.z = 0.0f;
    zi.x = 0.0f; zi.y = 0.0f; zi.z = 1.0f;
    
    //float zm2min = 10e5;
    //float zm2max = -10e5;
    
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
            
            col.albedo = planet->map->getc(px*size + x, py*size + y);
            
            gridgeom->set_tri(ti, v0, v1, v2, col, norm);
            gridgeom->set_tri(ti+1, v1, v3, v2, col, norm);
            ti += 2;
            
            gridgeom->set_lin(li, v0, v1, col);
            gridgeom->set_lin(li+1, v0, v2, col);
            li += 2;
            
            gridgeom->set_pnt(pi,   (v0+v1+v2)/3.0f, col);
            //gridgeom->set_pnt(pi+1, (v1+v2+v3)/3.0f, col);
            pi += 1;
        }
    }
    
    //std::cout << "ZM2: [ " << zm2min << " ,  " << zm2max << " ]" << std::endl; 
    
    gsgl::clearGPU(gpuGrid);
    //gsgl::updateGPU(*gridgeom, gpuGrid);
    gsgl::MUXtoGPU(*gridgeom, gpuGrid);
    
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

void terrain_part::create_contour (ter::terrain_data_square* map, int px, int py)
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
            
            //float zm = std::max(v0.z, std::max(v1.z, std::max(v2.z, v3.z)));
            //unsigned int ci = pal->get(zm);
            
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
                
                col.albedo = map->getc(px*size + x, py*size + y);
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
                
                col.albedo = map->getc(px*size + x, py*size + y);
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
    
    
    cr::height_pal* MAT = new cr::height_pal(6);
    MAT->cols[0]   =     {0.0f ,  0.0f ,  0.2f};
    MAT->limits[0] = -23.0f;
    MAT->cols[1]   =     {0.235f, 0.435f, 1.0f};
    MAT->limits[1] =  -4.0f;
    MAT->cols[2]   =     {0.592f, 0.423f, 0.235f};
    MAT->limits[2] =   0.0f;
    MAT->cols[3]   =     {0.156f, 0.361f, 0.058f};
    MAT->limits[3] =   7.0f;
    //MAT->cols[4]   =     {0.08f,  0.20f,  0.09f};
    MAT->cols[4]   =     {0.529f, 0.529f, 0.529f};
    MAT->limits[4] =  28.0f;
    MAT->cols[5]   =     {0.529f, 0.529f, 0.529f};
    
    pal = new cr::height_pal(*MAT, 5);
    
    cr::height_pal* MAT2 = new cr::height_pal(4);
    MAT2->cols[0] = {0.0f ,  0.0f ,  0.2f};
    MAT2->cols[1] = {0.235f, 0.435f, 1.0f};
    MAT2->cols[2] = {0.5f, 0.5f, 0.5f};
    MAT2->cols[3] = {0.7f, 0.7f, 0.7f};
    MAT2->limits[0] = -23.0f;
    MAT2->limits[1] =  -4.0f;
    MAT2->limits[2] =  10.0f;
    
    //pal2 = new cr::height_pal(*MAT2, 6);
    
    
    
    cr::height_pal* MAT3 = new cr::height_pal(4);
    MAT3->cols[0] = {0.08f ,  0.33f ,  0.06f};
    MAT3->cols[1] = {0.3f, 0.25f, 0.09f};
    MAT3->cols[2] = {0.0f, 0.80f, 1.00f};
    MAT3->cols[3] = {0.50f, 0.70f, 0.90f};
    MAT3->limits[0] = -4.5f;
    MAT3->limits[1] = 0.8f;
    MAT3->limits[2] = 10.0f;
    
    //pal2 = new cr::height_pal(*MAT3, 4);
    
    
    
    cr::height_pal* MAT4 = new cr::height_pal(3);
    MAT4->cols[0] = {0.0f , 0.0f , 0.2f};
    //MAT4->cols[1] = {0.235f, 0.435f, 0.8f};
    MAT4->cols[1] = {0.24f, 0.46f, 0.86f};
    //MAT4->cols[2] = {0.2f, 0.4f, 1.0f};
    MAT4->cols[2] = {0.33f, 0.50f, 1.0f};
    MAT4->limits[0] = -22.0f;
    MAT4->limits[1] =  16.0f;
    
    //pal2 = new cr::height_pal(*MAT4, 6);
    
    cr::height_pal* MAT5 = new cr::height_pal(3);
    MAT5->cols[0] = {0.0, 0.2, 0.5};
    MAT5->limits[0] = -15.0f;
    MAT5->cols[1] = {0.9, 0.75, 0.0};
    MAT5->limits[1] =  15.0f;
    MAT5->cols[2] = {0.0, 0.67, 0.2};
    
    pal2 = new cr::height_pal(*MAT5, 4);
    
    noisegen = new ter::noise_val();
    noisegen->xscale = 15.0f;
    noisegen->yscale = 15.0f;
    
    noiseg = new ter::noise_grad();
    noiseg->xscale = 15.0f;
    noiseg->yscale = 15.0f;
    
    noises = new ter::noise_s();
    noises->xscale = 15.0f;
    noises->yscale = 15.0f;
    
    delnoise_gen = nullptr;
    init_delnoise();
    
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
    delete noiseg;
    delete noises;
    delete delnoise_gen;
    
    delete pal;
    delete pal2;
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

void simple_planet::color ()
{
    cr::vec3 v0, v1, v2, v3, norm;
    
    //cr::fdice d1(120, -0.5f, 0.5f);
    
    float d1 = -0.05;
    
    for (int y=0 ; y<map->H ; ++y)
    {
        for (int x=0 ; x<map->W ; ++x)
        {
            int x1 = (x == map->W-1) ? 0 : x+1;
            int y1 = (y == map->H-1) ? 0 : y+1;
            
            v0.x = float(x);
            v0.y = float(y);
            v0.z = map->geth(x, y);
            v1.x = x + 1.0f;
            v1.y = float(y);
            v1.z = map->geth(x1, y);
            v2.x = float(x);
            v2.y = y + 1.0f;
            v2.z = map->geth(x, y1);
            v3.x = x + 1.0f;
            v3.y = y + 1.0f;
            v3.z = map->geth(x1, y1);
            
            norm = cr::cross( (v1-v0), (v2-v0) );
            norm.normalize();
            /*
            if (v0.z < 0.05)
            {
                float zm = std::max(v0.z, std::max(v1.z, std::max(v2.z, v3.z)));
                //unsigned int ci  = pal->get(zm);
                unsigned int ci2 = pal2->get(zm);
                map->col[y*W + x] = pal2->cols[ci2];
            }
            else
            {
            
                float zm2 = std::acos( cr::dot(norm, cr::vec3{0,0,1}) ) * 2.0f / cr::pi;
                
                //if (zm2 < zm2min) zm2min = zm2;
                //if (zm2 > zm2max) zm2max = zm2;
                
                //col.albedo = zm2 < 0.8f ? pal->cols[ci] : pal2->cols[ci2];
                //col.albedo = pal->cols[ci];
                
                //float hues[4] = {118.0f/360.0f, 70.0f/360.0f, 184.0f/360.0f, 147.0f/360.0f};
                float hues[4] = {118.0f/360.0f, 118.0f/360.0f, 184.0f/360.0f, 147.0f/360.0f};
                int huei = int(std::floor((3.0f*zm2 + 0.5f)));
                map->col[y*W + x] = cr::hsv2rgb(cr::vec3 { hues[huei], 0.9, 0.9 } );
            }
            */
            
            float zm = std::max(v0.z, std::max(v1.z, std::max(v2.z, v3.z)));
            //unsigned int ci  = pal->get(zm);
            unsigned int ci2 = pal2->get(zm);
            cr::vec3 rgb = pal2->cols[ci2];
            cr::vec3 hsv = cr::rgb2hsv(rgb);
            hsv.x += d1;
            map->col[y*W + x] = cr::hsv2rgb(cr::vec3(hsv.x, 0.9, 0.9));
            
            d1 += 0.001;
            if (d1 > 0.05) d1 = -0.05;
        }
    }
}

void simple_planet::init_delnoise()
{
    delnoise_gen = new ter::del_noise();
    delnoise_gen->xscale = 2.1f;
    delnoise_gen->yscale = 2.1f;
    delnoise_gen->xp = 0.0f;
    delnoise_gen->yp = 0.0f;
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
            output << ss.str() << ".terr" << std::endl;
            savepart(dir, ss.str(), i, j);
        }
    }
    output.close();
    std::cout << "Saved: " << (dir + name_base + ".txt") << std::endl;
}

void simple_planet::savepart (const std::string& dir, const std::string& name, int i, int j)
{
    std::ofstream output(dir + name + ".terr", std::ios::binary);
    
    unsigned int Wu  = partsize;
    unsigned int PWu = pw;
    
    float f3;
    
    output.write((char*)&partsize, sizeof(unsigned int));
    output.write((char*)&partsize, sizeof(unsigned int));
    
    for (unsigned int y=j*Wu ; y<=(j+1)*Wu ; ++y)
    {
        for (unsigned int x=i*Wu ; x<=(i+1)*Wu ; ++x)
        {
            f3 = map->height[y*(Wu*PWu+1) + x];
            
            //if (f3 < 0.0f) f3 = 0.0f;
            
            output.write((char*)&f3, sizeof(float));
            //output.write((char*)&c,  sizeof(uint32));
        }
    }
    
    for (unsigned int y=j*Wu ; y<=(j+1)*Wu ; ++y)
    {
        for (unsigned int x=i*Wu ; x<=(i+1)*Wu ; ++x)
        {
            cr::vec3 cc = map->col[y*(Wu*PWu+1) + x];
            uint32 c = (uint8(cc.x*255.0f)<<24) + (uint8(cc.y*255.0f)<<16) + (uint8(cc.z*255.0f)<<8);
            
            //output.write((char*)&f3, sizeof(float));
            output.write((char*)&c,  sizeof(uint32));
        }
    }
    
    output.close();
}

void simple_planet::create_contour ()
{
    contoured = true;
    
    for (int j=0 ; j<ph ; ++j)
    {
        for (int i=0 ; i<pw ; ++i)
        {
            parts[j*pw+i]->create_contour(map, i, j);
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
            map->height[0*W + px]     = corners;
            map->height[(W-1)*W + px] = corners;
        }
    }
    
    ter::ds_square_grid(map->height, partsize, pw, ph);
    edgefit(4);
    color();
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
    color();
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
    edgefit(4);
    color();
    updateGpu(slice);
}

void simple_planet::generate_delaunay (bool slice)
{
    contoured = false;
    
    if (!delnoise_gen) init_delnoise();
    
    for (int x=0 ; x<W ; ++x)
    {
        for (int y=0 ; y<H ; ++y)
        {
            map->height[y*W + x] = 1.5f * delnoise_gen->fbm(float(x), float(y));
        }
    }
    edgefit(5);
    color();
    updateGpu(slice);
}

void simple_planet::gen_noise ()
{
    contoured = false;
    
    for (int x=0 ; x<W ; ++x)
    {
        for (int y=0 ; y<H ; ++y)
        {
            map->height[y*W + x] = 3.0f * noiseg->base(float(x), float(y)).x;
        }
    }
    edgefit(5);
    color();
    updateGpu(false);
}

void simple_planet::fbm (int on, float g)
{
    for (int x=0 ; x<W ; ++x)
    {
        for (int y=0 ; y<H ; ++y)
        {
            map->height[y*W + x] = 40.0f * noiseg->fbm(float(x)/128.0f, float(y)/128.0f, on, g);
        }
    }
    edgefit(5);
    color();
    updateGpu(false);
}

void simple_planet::domain_warp (int on, float g)
{
    for (int x=0 ; x<W ; ++x)
    {
        for (int y=0 ; y<H ; ++y)
        {
            map->height[y*W + x] = 40.0f * noiseg->domain_warp(float(x)/128.0f, float(y)/128.0f, on, g);
        }
    }
    edgefit(5);
    color();
    updateGpu(false);
}

void simple_planet::s_noise ()
{
    contoured = false;
    
    for (int x=0 ; x<W ; ++x)
    {
        for (int y=0 ; y<H ; ++y)
        {
            map->height[y*W + x] = 3.0f * noises->noise( cr::vec2(float(x)/128.0f,
                                                                  float(y)/128.0f) );
        }
    }
    edgefit(5);
    color();
    updateGpu(false);
}

void simple_planet::s_fbm (int on, float g)
{
    contoured = false;
    
    for (int x=0 ; x<W ; ++x)
    {
        for (int y=0 ; y<H ; ++y)
        {
            map->height[y*W + x] = 10.0f * noises->fbm( cr::vec2(float(x)/64.0f - 64.0f,
                                                                 float(y)/64.0f - 64.0f),
                                                        on, g );
        }
    }
    edgefit(5);
    color();
    updateGpu(false);
}

void simple_planet::s_domain_warp (int on, float g)
{
    contoured = false;
    
    for (int x=0 ; x<W ; ++x)
    {
        for (int y=0 ; y<H ; ++y)
        {
            map->height[y*W + x] = 10.0f * noises->domain_warp( cr::vec2(float(x)/128.0f - 128.0f,
                                                                         float(y)/128.0f - 128.0f),
                                                                on, g );
        }
    }
    edgefit(5);
    color();
    updateGpu(false);
}

void simple_planet::erosion_step ()
{
    contoured = false;
    ter::erosion_hydraulic(map);
    color();
}
void simple_planet::erosion_2_step ()
{
    contoured = false;
    ter::erosion_talus(map->height, W, H, 0.1f);
    color();
}

void simple_planet::ridge ()
{
    for (int x=0 ; x<W ; ++x)
    {
        for (int y=0 ; y<H ; ++y)
        {
            map->height[y*W + x] = std::abs(map->height[y*W + x]) - (hmax/4.0);
        }
    }
    color();
    updateGpu(false);
}

void simple_planet::quantize(float q)
{
    ter::quantize(map->height, W, H, q);
    color();
    updateGpu(false);
}

void simple_planet::level(float l)
{
    ter::level(map->height, W, H, l);
    updateGpu(false);
}

void simple_planet::filter ()
{
    contoured = false;
    float k[9] = {
        0.0f,  -1.0f,  0.0f,
       -1.0f,   5.0f, -1.0f,
        0.0f,  -1.0f,  0.0f
    };
    ter::convolve(map->height, map_tmp->height, W, H, k, 3, 3, 1.0f);
    //ter::laplacian2(map->height, map_tmp->height, W, H);
    color();
    updateGpu(false);
}

void simple_planet::golinit ()
{
    contoured = false;
    ter::game_of_life_init(map->height, map_tmp->height, W, H);
    color();
    updateGpu(false);
}

void simple_planet::gol ()
{
    contoured = false;
    ter::game_of_life(map->height, map_tmp->height, W, H);
    color();
    updateGpu(false);
}

void simple_planet::rdinit ()
{
    contoured = false;
    //rdcalc2();
    ter::reaction_diffusion_init(map->sediment1, map_tmp->sediment1, map->sediment2, map_tmp->sediment2, W, H, 10.0f);
    color();
    updateGpu(false);
}

void simple_planet::rd ()
{
    contoured = false;
    for (int i=0 ; i<2000 ; ++i)
    {
        ter::reaction_diffusion(map->sediment1, map_tmp->sediment1, map->sediment2, map_tmp->sediment2, W, H, 10.0f);
    }
    color();
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
    color();
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
    
    for (int i=0 ; i<W ; ++i)
    {
        for (int ki=0 ; ki<k ;++ki)
        {
            a = 1.0f / (ki+2);
            b = 1.0f - a;
            h1 = map->height[ki*W       + i];
            h2 = map->height[(H-1-ki)*W + i];
            map->height[ki*W       + i] = a * h2 + b * h1;
            map->height[(H-1-ki)*W + i] = a * h1 + b * h2;
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
    
    //color();
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

