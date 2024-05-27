#include "grid_scene.hh"

#include <cmath>
#include <iostream>
#include <string>

#include <crcc/crcc.hh>

using namespace std;





grid_scene::grid_scene(cr::scripter& _conf) : scene(_conf)
{
    //bool found;
    
    sysconf.getvalue("folders.target", target_dir);
    //if (!found) return;
    target_dir = cr::spath(target_dir);
    
    cr::camera* cam0 = new cr::camera();
    cam0->near = 0.01f;
    cam0->far = 100.0f;
    cam0->focal = 4.0f;
    cam0->up   = cr::vec3(0.0f, 1.0f,  0.0f);
    cam0->pos  = cr::vec3(0.0f, 0.0f,  3.0f);
    cam0->look = cr::vec3(0.0f, 0.0f, -1.0f);
    cam0->oheight2 = 2.0f;
    cam0->pheight2 = 2.0f;
    cam0->aspect = 1.0f;
    
    cameras.push_back(cam0);
    
    
    
    cr::camera* cam1 = new cr::camera();
    cam1->near = 0.01f;
    cam1->far = 100.0f;
    cam1->focal = 4.0f;
    cam1->up   = cr::vec3(0.0f, 1.0f,  1.0f);
    cam1->up.normalize();
    cam1->pos  = cr::vec3(0.0f, 0.0f,  3.0f);
    cam1->look = cr::vec3(0.0f, 1.0f, -1.0f);
    cam1->look.normalize();
    cam1->oheight2 = 2.0f;
    cam1->pheight2 = 2.0f;
    cam1->aspect = 1.0f;
    
    cameras.push_back(cam1);
    
    
    rmode.camtype = 1;
    rmode.objtype = 1; objrender = 1;
    rmode.colourmode = 2;
    sflow = false;
            
    rrr_n = new gsgl::r_nshaded();
    rrr_v = new gsgl::r_vshaded();
    rrr_n->setup(&rmode);
    rrr_v->setup(&rmode);
    
    //render_lines.resize(9);
    
    
    std::string planet_in;
    sysconf.getvalue("data.planet_in", planet_in);
    //if (found)
    if (0)
    {
        //planet = new SimplePlanet(target_dir + planet_in);
        //px = py = 0;
        //planet->updateTerrainParts(px, py);
    }
    else
    {
        std::string s;
        bool f1 = sysconf.getvalue("data.planet_partsize", s);
        if (!f1) { std::cerr << "ERROR: [data] planet_partsize not found" << std::endl; return; }
        int pps = std::stoi(s);
        
        f1 = sysconf.getvalue("data.planet_sizex", s);
        if (!f1) { std::cerr << "ERROR: [data] planet_sizex not found" << std::endl; return; }
        int pw = std::stoi(s);
        
        f1 = sysconf.getvalue("data.planet_sizey", s);
        if (!f1) { std::cerr << "ERROR: [data] planet_sizey not found" << std::endl; return; }
        int ph = std::stoi(s);
        
        planet = new simple_planet(pps, pw, ph);
    }
}

grid_scene::~grid_scene()
{
    delete rrr_n;
    delete rrr_v;
    delete planet;
}


void grid_scene::step(float /*dt*/) {}

void grid_scene::save ()
{
    std::string d,n;
    bool f = sysconf.getvalue("data.planet_out_dir", d);
    f     &= sysconf.getvalue("data.planet_out_name", n);

    if (f)
    {
        planet->save(cr::spath(target_dir + d), n);
    }
    else
    {
        std::cerr << "ERROR: [data] planet_out_dir or _name not found" << std::endl;
    }
}

void grid_scene::mousescroll(float /*x*/, float y)
{
    cr::vec3 v(0.0f, 0.0f, -y*0.1f);
    for (unsigned int ci=0 ; ci<cameras.size() ; ++ci)
    {
        cameras[ci]->pos += v;
        cameras[ci]->oheight2 -= y*0.1f;
    }
}

void grid_scene::lookup(float a)
{
    if (used_cam == 0) return;
    cr::vec3 v = cr::cross(cameras[1]->look, cameras[1]->up);
    float S = std::sin(a/2.0f * cr::dtor);
    cr::quat q(std::cos(a/2.0f * cr::dtor), v.x*S, v.y*S, v.z*S);
    cameras[1]->look = cr::rot_q(q, cameras[1]->look);
    cameras[1]->up   = cr::rot_q(q, cameras[1]->up);
}

void grid_scene::rotate(float a)
{
    cr::quat q(std::cos(a/2.0f * cr::dtor), 0.0f, 0.0f, std::sin(a/2.0f * cr::dtor));
    
    for (unsigned int ci=0 ; ci<cameras.size() ; ++ci)
    {
        cameras[ci]->look = cr::rot_q(q, cameras[ci]->look);
        cameras[ci]->up   = cr::rot_q(q, cameras[ci]->up);
    }
}

void grid_scene::mousedrag(float /*xp*/, float /*yp*/, float x, float y, int /*btn*/)
{
    //cr::vec3 v((x)*0.01f, (y)*0.01f, 0.0f);
    for (unsigned int ci=0 ; ci<cameras.size() ; ++ci)
    {
        cr::vec3 left = cr::cross(cameras[ci]->look, cameras[ci]->up);
        cr::vec3 forw = cr::cross(cr::vec3(0.0f, 0.0f, 1.0f), left);
        if (cameras[ci]->up.z < 0) forw = -forw;
        cameras[ci]->pos += forw*(y)*0.01f;
        cameras[ci]->pos += left*(x)*0.01f;
    }
}

void grid_scene::init_render()
{
/*
    renderers[used_render]->initRender(screen_width, screen_height);
    if (used_render_2 >= 0)
    {
        renderers[used_render_2]->initRender(screen_width, screen_height);
    }
*/
}

void grid_scene::pre_render()
{
    //renderers[used_render]->preRender();
}

void grid_scene::render()
{
    //if (!(framebuf->is_complete())) { std::cout << "NC" << std::endl; }
    framebuf->use();
    
    rmode.objtype = objrender < 3 ? objrender : objrender-3;
    cr::mat4 im;
    
    if (rmode.objtype < 2)
    {
        rrr_n->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
        rrr_n->pre_render();
        
        for (int i=0 ; i<planet->pw*planet->ph ; ++i)
        {
            if (planet->parts[i] != 0)
            {
                im =  cr::move_mat(planet->parts[i]->pos);
                im *= cr::scale_mat(planet->parts[i]->scale);
                rrr_n->render(*cameras[used_cam], im, planet->parts[i]->gpuGrid);
                if (sflow) rrr_n->render(*cameras[used_cam], im, planet->parts[i]->gpuFlow);
            }
            else
            {
                cout << "ERROR: simple_planet: nullpointer" << endl;
                return;
            }
        }
    }
    else
    {
        rrr_v->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
        rrr_v->pre_render();
        for (int i=0 ; i<planet->pw*planet->ph ; ++i)
        {
            if (planet->parts[i] != 0)
            {
                im =  cr::move_mat(planet->parts[i]->pos);
                im *= cr::scale_mat(planet->parts[i]->scale);
                rrr_v->render(*cameras[used_cam], im, planet->parts[i]->gpuGrid);
                if (sflow) rrr_n->render(*cameras[used_cam], im, planet->parts[i]->gpuFlow);
            }
            else
            {
                cout << "ERROR: simple_planet: nullpointer" << endl;
                return;
            }
        }
    }
    
    if (objrender >= 3)
    {
        rmode.objtype = 3;
        rrr_v->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
        
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1, 1);
        for (int i=0 ; i<planet->pw*planet->ph ; ++i)
        {
            if (planet->parts[i] != 0)
            {
                im =  cr::move_mat(planet->parts[i]->pos);
                im *= cr::scale_mat(planet->parts[i]->scale);
                rrr_v->render(*cameras[used_cam], im, planet->parts[i]->gpuGrid);
            }
        }
        glDisable(GL_POLYGON_OFFSET_FILL);
    }
    
    framebuf->render();
    
    //std::cout << "END RENDER" << std::endl;
}


void grid_scene::keyaction(int key, int action, int mods)
{
    if (action == keys::RELEASE) return;
    switch (key)
    {
        case keys::I :
            ++(rmode.camtype); if (rmode.camtype >= cr::render_mode::cam_n) rmode.camtype=0;
            break;
                
        case keys::O :
            ++(objrender); if (objrender >= cr::render_mode::obj_n) objrender=0;
            break;
                
        case keys::P :
            break;
            
        case keys::K :
            ++(rmode.colourmode); if (rmode.colourmode >= cr::render_mode::col_n) rmode.colourmode=0;
            break;
        
        case keys::A :
            //mousedrag(0.0f, 0.0f, 10.0f, 0.0f, 0);
            rotate(2.0f);
            break;
            
        case keys::D :
            //mousedrag(0.0f, 0.0f, -10.0f, 0.0f, 0);
            rotate(-2.0f);
            break;
            
        case keys::W :
            lookup(2.0f);
            break;
            
        case keys::S :
            lookup(-2.0f);
            break;
            
        case keys::F :
            planet->filter();
            planet->stat();
            break;
            
        case keys::Z :
            //mods & keys::CONTROL_BIT ? planet->rdinit() : planet->rd();
            //planet->rdcalc();
            mods & keys::CONTROL_BIT ? planet->golinit() : planet->gol();
            planet->stat();
            break;
            
        case keys::G :
            planet->generate_ds(mods & keys::CONTROL_BIT, mods & keys::SHIFT_BIT);
            planet->stat();
            break;
            
        case keys::H :
            planet->generate_warp(mods & keys::CONTROL_BIT);
            //planet->generate_delaunay(mods & keys::CONTROL_BIT);
            planet->stat();
            break;
        
        case keys::C :
            planet->crack();
            planet->stat();
            break;
            
        case keys::Q :
            planet->quantize(2.0f);
            break;
        
        case keys::L :
            planet->level(0.0f);
            planet->edgefit(4);
            break;
            
        case keys::R :
            for (int i=0 ; i<10 ; ++i) {
                planet->erosion_step();
                //planet->erosion_2_step();
            }
            planet->stat();
            planet->updateGpu(false);
            break;
            
        case keys::T :
            for (int i=0 ; i<2 ; ++i) {
                planet->erosion_2_step();
            }
            planet->stat();
            planet->updateGpu(false);
            break;
            
        case keys::MINUS :
            mousescroll(0.0f, -0.2f);
            break;
            
        case keys::PLUS :
            mousescroll(0.0f, 0.2f);
            break;
            
        case keys::BUT_1 :
            planet->lessgap();
            break;
            
        case keys::BUT_2 :
            planet->moregap();
            break;
        
        case keys::BUT_3 :
            sflow = !sflow;
            break;
        
        case keys::BUT_4 :
            planet->ridge();
            break;
            
        case keys::BUT_5 :
            planet->generate_warp(false);
            planet->stat();
            break;
            
        case keys::BUT_6 :
            //planet->generate_delaunay(false);
            planet->domain_warp(2, 0.7f);
            //planet->gen_noise();
            //planet->fbm(10, 0.7f);
            planet->stat();
            break;
            
        case keys::BUT_7 :
            planet->s_noise();
            //planet->s_fbm(4, 0.7f);
            //planet->s_domain_warp(2, 0.5f);
            planet->stat();
            break;
            
        case keys::BUT_8 :
            planet->create_contour();
            break;
        
        case keys::BUT_9 :
            ++used_cam;
            if (used_cam >= cameras.size()) used_cam = 0;
            break;
        
        /*
        case keys::BUT_4 :
            planet->less();
            break;
            
        case keys::BUT_5 :
            planet->more();
            break;
        */
        
        case keys::ENTER :
            planet->reset();
            break;
        
        default:
            return;
    }
}

