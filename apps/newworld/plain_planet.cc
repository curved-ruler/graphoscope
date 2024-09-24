
#include "plain_planet.hh"


#include <cmath>
#include <sstream>
#include <fstream>
#include <iostream>

#include <crcc/crcc.hh>
#include <gsgl/gsgl.hh>




cell::cell (const std::string& file, float dx, float dy, float s)
{
    //std::cout << "TP: " << file.c_str() << std::endl;
    usedInNextIter = false;
    scale = s;
    pos.set(scale*dx, scale*dy, 0.0f);
    
    //geometry = cr::bm1_to_mb(file);
    //geometry = cr::bm1_to_mb_contour(file);
    geometry = cr::terr_to_meshux(file);
    
    gsgl::MUXtoGPU(*geometry, gpuPointers);
}

cell::~cell ()
{
    gsgl::clearGPU(gpuPointers);
    delete geometry;
}

void cell::draw (const cr::camera& c, cr::renderer* rrr) const
{
    cr::mat4 im =  cr::move_mat(pos);
             im *= cr::scale_mat(scale);
    rrr->render(c, im, gpuPointers);
}

void cell::drawcpu (const cr::camera& c, cr::renderer* rrr) const
{
    cr::mat4 im =  cr::move_mat(pos);
             im *= cr::scale_mat(scale);
    rrr->render(c, im, *geometry);
}

float cell::height (float xf, float yf, float tilesize)
{
    int ii = std::floor(xf);
    int jj = std::floor(yf);
    int i = (jj*tilesize + ii)*2;
    
    float xfi = xf - ii;
    float yfj = yf - jj;
    
    if (i >= 0 && (unsigned int)i < geometry->triangles.size()/27)
    {
        /*
        if (xfi + yfj < 0.5f)
        {
            float z1 = geometry->tris[i*27 + 11] - geometry->tris[i*27 + 2];
            float z2 = geometry->tris[i*27 + 20] - geometry->tris[i*27 + 2];
            return (geometry->tris[i*27 + 2] + xfi*z1 + yfj*z2) * scale;
        }
        else
        {
            float z1 = geometry->tris[(i+1)*27 + 20] - geometry->tris[(i+1)*27 + 11];
            float z2 = geometry->tris[(i+1)*27 + 2]  - geometry->tris[(i+1)*27 + 11];
            return (geometry->tris[(i+1)*27 + 11] + (1.0f-xfi)*z1 + (1.0f-yfj)*z2) * scale;
        }
        */
        
        return (
            geometry->triangles[i*27 + 2]  * (1.0f-xfi) * (yfj) +
            geometry->triangles[i*27 + 11] * (1.0f-xfi) * (1.0f-yfj) +
            geometry->triangles[i*27 + 20] * (xfi) * (1.0f-yfj) +
            geometry->triangles[(i+1)*27 + 11] * (xfi) * (yfj)
        ) * scale;
    }
    else
    {
        std::cout << "TP::height " << i << std::endl;
        return 0.0f;
    }
}



plain_planet::plain_planet (const std::string& mapfile)
{
    base_dir = cr::spath(cr::get_folder(mapfile));
    
    tilesize = 100.0f;
    scale = 1.0f / 100.0f;
    pw = 0;
    ph = 0;
    w = 3;
    h = 3;
    xo1 = (-w) / 2;
    yo1 = (-h) / 2;
    partFiles = 0;
    
    parts = new cell*[w*h];
    newparts = new cell*[w*h];
    //parts.reserve(w*h);
    //newparts.reserve(w*h);
    for (int i=0 ; i<w*h ; ++i) {
        parts[i] = 0;
        newparts[i] = 0;
    }
    
    std::string line;
    std::ifstream file(mapfile);
    if (!file) {
        std::cout << "ERROR: plain_planet: file not exists: " << mapfile << std::endl;
    }
    
    getline(file,line);
    std::istringstream buffer0(line);
    buffer0 >> tilesize;
    getline(file,line);
    std::istringstream buffer1(line);
    buffer1 >> pw;
    getline(file,line);
    std::istringstream buffer2(line);
    buffer2 >> ph;
    
    if (pw < 1 || ph < 1) {
        std::cout << "ERROR: plain_planet: " << mapfile << " wrong dimensions" << std::endl;
        file.close();
        return;
    }
    partFiles = new std::string[pw*ph];
    int i=0;
    while (getline(file,line)) {
        if (line.length() == 0 || line.at(0) == '#') continue;
        if (i >= pw*ph) break;
        partFiles[i] = line;
        i++;
    }
    if (i < pw*ph) {
        std::cout << "ERROR: plain_planet: " << mapfile << " has not enough parts." << std::endl;
        file.close();
        return;
    }
    file.close();
    
    initcells();
}

plain_planet::~plain_planet ()
{
    delete[] partFiles;
    for (int i=0 ; i<w*h ; ++i) {
        delete parts[i];
        parts[i] = 0;
    }
    delete[] parts;
    delete[] newparts;
}

void plain_planet::get_surface (cr::vec3 /*test_in*/, cr::vec3& /*out*/)
{
    
}

void plain_planet::modWindowAndPosition ()
{
    if (xo1 > pw || (xn1 + w - 1) < 0) {
        xo1 = cr::modi(xo1, pw);
        //xo2 = xo1 + w - 1;
    }
    
    if (yo1 > ph || (yn1 + h - 1) < 0) {
        yo1 = cr::modi(yo1, ph);
        //yo2 = yo1 + h - 1;
    }
}

void plain_planet::initcells ()
{
    int i2, j2;
    for (int j=0 ; j<h ; ++j) {
        for (int i=0 ; i<w ; ++i) {
            
            i2 = cr::modi(xo1+i, pw);
            j2 = cr::modi(yo1+j, ph);
            //cout << i2 << "***" << j2 << endl;
            parts[j*w+i] = new cell(base_dir + partFiles[j2*pw + i2],
                                           tilesize*(xo1+i), tilesize*(yo1+j),
                                           scale);
        }
    }
}

void plain_planet::updatecells (float x, float y)
{
    int sum = 0;
    
    //float d = tilesize*scale/2.0f;
    int xm = std::floor(x / (tilesize*scale));
    int ym = std::floor(y / (tilesize*scale));
    xn1 = xm - w/2; //xn2 = xn1 + w - 1;
    yn1 = ym - h/2; //yn2 = yn1 + h - 1; 
    
    int i2, j2;
    int io, jo;
    for (int j=0 ; j<h ; ++j)
    {
        for (int i=0 ; i<w ; ++i)
        {
            io = i + xn1 - xo1;
            jo = j + yn1 - yo1;
            
            if (io>=0 && io<w && jo>=0 && jo<h)
            {
                parts[jo*w + io]->usedInNextIter = true;
                newparts[j*w + i] = parts[jo*w + io];
            }
            else
            {
                i2 = cr::modi(xn1+i, pw);
                j2 = cr::modi(yn1+j, ph);
                //cout << i2 << "***" << j2 << endl;
                newparts[j*w+i] = new cell(base_dir + partFiles[j2*pw + i2],
                                                  tilesize*(xn1+i), tilesize*(yn1+j),
                                                  scale);
            }
        }
    }
    

    for (int i=0 ; i<w*h ; ++i)
    {
        if (!(parts[i]->usedInNextIter))
        {
            delete parts[i];
            parts[i] = 0;
        }
        else
        {
            parts[i]->usedInNextIter = false;
            ++sum;
        }
        parts[i] = newparts[i];
    }
    //parts = newparts;
    //cout << endl << endl;
    std::cout << "Parts kept: " << sum << std::endl;
    
    xo1 = xn1;
    yo1 = yn1;
}

void plain_planet::draw (const cr::camera& cam, cr::renderer* rrr) const
{
    for (int i=0 ; i<w*h ; ++i) {
        if (parts[i] != 0) {
            parts[i]->draw(cam, rrr);
        } else {
            std::cout << "ERROR: plain_planet: nullpointer" << std::endl;
            return;
        }
    }
}

void plain_planet::drawcpu (const cr::camera& cam, cr::renderer* rrr) const
{
    for (int i=0 ; i<w*h ; ++i) {
        if (parts[i] != 0) {
            parts[i]->drawcpu(cam, rrr);
        } else {
            std::cout << "ERROR: plain_planet: nullpointer" << std::endl;
            return;
        }
    }
}

bool plain_planet::newpos (cr::vec3 in, cr::vec3& newp)
{
    float xf = in.x / (tilesize*scale);
    float yf = in.y / (tilesize*scale);
    int xm = std::floor(xf);
    int ym = std::floor(yf);
    int ti = cr::modi(-xo1+xm, pw);
    int tj = cr::modi(-yo1+ym, ph);
    
    if (ti >= 0 && ti < w && tj >= 0 && tj < h)
    {
        newp.x = in.x;
        newp.y = in.y;
        newp.z = parts[tj*w + ti]->height((xf - (float)xm) * tilesize, (yf - (float)ym) * tilesize, tilesize);
        return true;
    }
    else
    {
        return false;
    }
}
