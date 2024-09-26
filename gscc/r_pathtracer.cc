
#include "r_pathtracer.hh"

#include <vector>
#include <thread>
#include <algorithm>
#include <iostream>
#include <sstream>

namespace gscc {

r_pathtracer::r_pathtracer()
{
    screen = nullptr;
    depth  = nullptr;
    sw = sh = 0;
    //dice = nullptr;
    //actpixels = nullptr;
    max_depth = 1000000.0f;
    
    ipixel = 0;
    thread_k = 0;
    path_n = 0;
    
    rgen = new std::mt19937(238);
}
r_pathtracer::~r_pathtracer()
{
    delete rgen;
    delete[] screen;
    delete[] depth;
}

void r_pathtracer::setup(cr::render_mode* rm)
{
    settings = rm;
}

void r_pathtracer::init_render(unsigned int w, unsigned int h)
{
    if (w != sw || h != sh)
    {
        sw = w;
        sh = h;
        delete[] screen;
        delete[] depth;
        //delete dice;
        //dice = new cr::idice(237, 0, sw*sh-1);
        screen = new float[sw*sh*3];
        depth  = new float[sw*sh];
        
        perm.clear();
        perm.resize(sw*sh);
        for (unsigned int i=0 ; i<sw*sh ; ++i) perm[i] = i;
        std::shuffle(perm.begin(), perm.end(), *rgen);
        
        path_n = (sw*sh)/settings->frames;
        if ((sw*sh) % settings->frames != 0) path_n += 1;
        
        thread_k = path_n / settings->thread_n;
        if (path_n % settings->thread_n != 0) thread_k += 1;
        
        ipixel = 0;
    }
}

void r_pathtracer::pre_render()
{
    /*
    delete[] actpixels;
    path_n = settings->path_n;
    actpixels = new int[path_n];
    int x, y;
    for (int i=0 ; i<path_n ; ++i)
    {
        actpixels[i] = dice->next();
        x = actpixels[i] % (int)sw;
        y = actpixels[i] / (int)sw;
        
        screen[(y*sw + x)*3]     = 0.0f;
        screen[(y*sw + x)*3 + 1] = 0.0f;
        screen[(y*sw + x)*3 + 2] = 0.0f;
        depth[(y*sw + x)] = max_depth;
    }
    */
}

void r_pathtracer::render_pixel (int pix, const cr::camera& cam, const cr::mat4& modeltr, const cr::mesh_ux& mesh)
{
    for (int i=0 ; i<thread_k ; ++i)
    {
        unsigned int j = ipixel*path_n + thread_k*pix + i;
        if (j >= perm.size())
        {
            //std::stringstream ss("");
            //ss << "OVR pix: " << pix << " thread_k: " << thread_k << " ipixel: " << ipixel << " i: " << i << "\n";
            //std::cout << ss.str();
            return;
        }
        
        unsigned int x = perm[j] % sw;
        unsigned int y = perm[j] / sw;
        //if (x >= sw || y >= sh) continue;
        
        screen[(y*sw + x)*3]     = settings->back[0];
        screen[(y*sw + x)*3 + 1] = settings->back[1];
        screen[(y*sw + x)*3 + 2] = settings->back[2];
        depth[(y*sw + x)] = max_depth;
        
        float fx = ( (float)x  - (float)sw / 2.0f) / ((float)sh / 2.0f); // [-asp,asp] >
        float fy = ( (float)y  - (float)sh / 2.0f) / ((float)sh / 2.0f); // [ -1,  1]  v
        
        cr::line3 ray;
        ray.P = cam.pos;
        cr::vec3 vx = cr::cross(cam.look, cam.up) * (fx /* cam.pheight2 / 2.0f*/);
        cr::vec3 vy = cam.up * (fy /* cam.pheight2 / 2.0f*/);
        ray.I = cam.look + vx + vy;
        ray.I.normalize();
        
        size_t trin = mesh.triangles.size() / mesh.tsize;
        for (unsigned int trii = 0 ; trii<trin ; ++trii)
        {
            cr::vec3 a = modeltr * cr::vec3(mesh.triangles[trii*mesh.tsize],    mesh.triangles[trii*mesh.tsize+1],  mesh.triangles[trii*mesh.tsize+2]);
            cr::vec3 b = modeltr * cr::vec3(mesh.triangles[trii*mesh.tsize+9],  mesh.triangles[trii*mesh.tsize+10], mesh.triangles[trii*mesh.tsize+11]);
            cr::vec3 c = modeltr * cr::vec3(mesh.triangles[trii*mesh.tsize+18], mesh.triangles[trii*mesh.tsize+19], mesh.triangles[trii*mesh.tsize+20]);
            
            cr::vec3 nplane = cr::cross(b-a,c-a);
            nplane.normalize();
            if (cr::abs(cr::dot(nplane,ray.I)) == 0.0f) { continue; }
            
            //float t = (cr::dot(nplane,a) - cr::dot(nplane,ray.P)) / cr::dot(nplane,ray.I);
            float t = cr::dot(nplane,a-ray.P) / cr::dot(nplane,ray.I);
            //if (t < 0.0f || t > depth[(y*sw + x)]) continue;
            if (t > depth[(y*sw + x)]) continue;
            cr::vec3 q = ray.P + ray.I*t;
            
            if (cr::dot(cr::cross(b-a,q-a), cr::cross(b-a,c-a)) < 0.0f) continue;
            if (cr::dot(cr::cross(c-b,q-b), cr::cross(c-b,a-b)) < 0.0f) continue;
            if (cr::dot(cr::cross(a-c,q-c), cr::cross(a-c,b-c)) < 0.0f) continue;
            /*
            if (cr::dot(cr::cross(b-a,q-a), nplane) < 0.0f) continue;
            if (cr::dot(cr::cross(c-b,q-b), nplane) < 0.0f) continue;
            if (cr::dot(cr::cross(a-c,q-c), nplane) < 0.0f) continue;
            */
            //nplane = modeltr * nplane;
            //nplane.normalize();
            
            screen[(y*sw + x)*3]     = mesh.triangles[trii*mesh.tsize+3]*0.7f + mesh.triangles[trii*mesh.tsize+3]*cr::dot(nplane,cr::vec3(1,1,1).normal())*0.3f;
            screen[(y*sw + x)*3 + 1] = mesh.triangles[trii*mesh.tsize+4]*0.7f + mesh.triangles[trii*mesh.tsize+4]*cr::dot(nplane,cr::vec3(1,1,1).normal())*0.3f;
            screen[(y*sw + x)*3 + 2] = mesh.triangles[trii*mesh.tsize+5]*0.7f + mesh.triangles[trii*mesh.tsize+5]*cr::dot(nplane,cr::vec3(1,1,1).normal())*0.3f;
            depth[(y*sw + x)] = t;
        }
    }
}

void r_pathtracer::render (const cr::camera& cam, const cr::mat4& modeltr, const cr::mesh_ux& mesh)
{
    std::vector<std::thread> render_threads;
    
    for (int ri=0 ; ri<settings->thread_n ; ++ri)
    {
        render_threads.emplace_back(&r_pathtracer::render_pixel, this, ri, cam, modeltr, mesh);
        //render_pixel(ri, cam, modeltr, mesh);
    }
    
    ipixel += 1;
    if (ipixel >= uint32(settings->frames)) ipixel = 0;
    
    for (auto& t : render_threads)  t.join();
}

}
