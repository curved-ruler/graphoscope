
#include "nw_scene.hh"

#include <cmath>
#include <iostream>
#include <string>

#include <crcc/crcc.hh>




nw_scene::nw_scene (cr::scripter& _conf) : scene(_conf)
{
    //bool found;
    
    sysconf.getvalue("folders.target", target_dir);
    
    target_dir = cr::spath(target_dir);
    std::cout << "Target: " << target_dir << std::endl;
    
    sysconf.getvalue("data.aeroplane_mesh", v_s);
    std::cout << "V mesh: " << v_s << std::endl;
    
    sysconf.getvalue("data.planet", p_s);
    std::cout << "Planet: " << p_s << std::endl;

    std::string used_vehicle_str;
    sysconf.getvalue("data.vehicle", used_vehicle_str);
    //used_vehicle = std::stoi(used_vehicle_str);
    used_vehicle = 0;
    
    distUpdatePlanet = 0.8f;
    maxDt = 2.0f;
    
    cr::camera* cam = new cr::camera();
    cam->near  = 0.001f;
    cam->far   = 10.0f;
    cam->focal = 1.0f;
    cam->setfov(25.0f*cr::dtor);
    cam->persp_to_ortho();
    cam->aspect = 1.0f;
    cam->pos  = cr::vec3(0.0f, 0.0f, 0.015f);
    cam->look = cr::vec3(1.0f, 0.0f, 0.0f);
    cam->up   = cr::vec3(0.0f, 0.0f, 1.0f);
    cameras.push_back(cam);
    
    cr::camera* camf = new cr::camera();
    camf->near  = 0.001f;
    camf->far   = 10.0f;
    camf->focal = 1.0f;
    camf->setfov(25.0f*cr::dtor);
    camf->persp_to_ortho();
    camf->aspect = 1.0f;
    camf->pos  = cr::vec3(0.0f, 0.0f, 0.015f);
    camf->look = cr::vec3(1.0f, 0.0f, 0.0f);
    camf->up   = cr::vec3(0.0f, 0.0f, 1.0f);
    cameras.push_back(camf);
    
    
    rmode.camtype = 1;
    rmode.objtype = 1; objrender = 1;
    rmode.colourmode = 2;
            
    cr::renderer* rrr_n = new gsgl::r_nshaded();
    cr::renderer* rrr_v = new gsgl::r_vshaded();
    rrr_n->setup(&rmode);
    rrr_v->setup(&rmode);
    renderers.push_back(rrr_n);
    renderers.push_back(rrr_v);
    
    
    showplane = true;
    
    plane = new cr::aeroplane(cameras[1], "");
    foot  = new cr::foot_cam(cameras[0]);
    
    plane->pos = cr::vec3(0.0f, 0.0f, 0.1f);
    //plane->updateCam();
    vpos1 = plane->pos;
    cr::mesh_ux* planegeom = cr::bm1_to_meshux(target_dir + cr::localize_path(v_s));
    gsgl::MUXtoGPU(*planegeom, planegpu);
    delete planegeom;
    
    planet = new plain_planet(target_dir + cr::localize_path(p_s));
    
    //glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
    std::cout << "cam[u]" << std::endl;
    std::cout << "  near  " << cameras[used_cam]->near << std::endl;
    std::cout << "  far   " << cameras[used_cam]->far << std::endl;
    std::cout << "  focal " << cameras[used_cam]->focal << std::endl;
    std::cout << "  ph2   " << cameras[used_cam]->pheight2 << std::endl;
    std::cout << "  oh2   " << cameras[used_cam]->oheight2 << std::endl;
    std::cout << "  look  " << cameras[used_cam]->look.str() << std::endl;
    std::cout << "  up    " << cameras[used_cam]->up.str() << std::endl;
}

nw_scene::~nw_scene ()
{
    delete plane;
    gsgl::clearGPU(planegpu);
    delete planet;
}

float distxy (cr::vec3 a, cr::vec3 b)
{
    return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}


void nw_scene::step (float dt)
{
    //cout << "dt: " << dt << endl;
    if (dt > maxDt) dt = maxDt;
    
    plane->tick(dt);
    
    foot->tick(dt);
    cr::vec3 np;
    planet->newpos(cameras[0]->pos, np);
    
    if (!used_vehicle) cameras[0]->pos = cr::vec3(np.x, np.y, np.z + 0.015f);
    
    
    if (used_vehicle && distxy(vpos1, plane->pos) > distUpdatePlanet)
    {
        std::cout << plane->pos.str() << " - " << vpos1.str() << " = " << distxy(vpos1, plane->pos) << std::endl;
        vpos1 = plane->pos;
        planet->updatecells(vpos1.x, vpos1.y);
    }
}

void nw_scene::init_render ()
{
    //renderers[used_render]->initRender(screen_width, screen_height);
}

void nw_scene::pre_render ()
{
    //renderers[used_render]->preRender();
}

void nw_scene::render ()
{
    //if (!(framebuf->is_complete())) { std::cout << "NC" << std::endl; }
    framebuf->use();

    //std::cout << "R1: " << used_render << "  R2: " << used_render_2 << "  C: " << used_cam << std::endl;
    
    //rmode.pp6size = 1.2f;
    
    //unsigned int x = rmode.colourmode;
    //if (used_render_2 >=0)
    //{
    //    rmode.colourmode = 2;
    //}
    
    rmode.objtype = (objrender < 3) ? objrender : objrender-3;
    used_render   = (rmode.objtype < 2) ? 0 : 1;
    used_render_2 = (objrender < 3) ? -1 : 1;
    
    cr::mat4 pltr = cr::move_mat(plane->pos) * plane->orient.rot_mat() * cr::scale_mat(0.1f, 0.1f, 0.1f);
    
    renderers[used_render]->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
    renderers[used_render]->pre_render();
    if (showplane) renderers[used_render]->render(*(cameras[used_cam]), pltr, planegpu);
    planet->draw(*(cameras[used_cam]), renderers[used_render]);
    
    if (used_render_2 >= 0)
    {
        rmode.objtype = 2;
        //rmode.colourmode = x;
        //rmode.pp6size = 0.9f;
        renderers[used_render_2]->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0f, 1.0f);
        if (showplane) renderers[used_render_2]->render(*(cameras[used_cam]), pltr, planegpu);
        planet->draw(*(cameras[used_cam]), renderers[used_render_2]);
        glDisable(GL_POLYGON_OFFSET_FILL);
    }

    framebuf->render();

    //std::cout << "END RENDER" << std::endl;
}



void nw_scene::perspsh (float d)
{
    cameras[used_cam]->persp_shift(d);
}

void nw_scene::save () {}

void nw_scene::keyaction (int key, int action, int mods)
{
    switch (key)
    {
        case keys::I :
            if (action == keys::PRESS)
            {
                ++(rmode.camtype); if (rmode.camtype >= cr::render_mode::cam_n) rmode.camtype=0;
            }
            break;
        
        case keys::O :
            if (action == keys::PRESS)
            {
                ++(objrender); if (objrender >= cr::render_mode::obj_n) objrender=0;
            }
            break;
        
        case keys::P :
            // prostprocmode
            break;
        
        case keys::K :
            if (action == keys::PRESS)
            {
                ++(rmode.colourmode); if (rmode.colourmode >= cr::render_mode::col_n) rmode.colourmode=0;
            }
            break;
            
        case keys::BUT_1 :
            if (action == keys::PRESS) showplane = !showplane;
            break;
        
        case keys::BUT_2 :
            if (action == keys::PRESS) perspsh(-0.02f);
            break;
        
        case keys::BUT_3 :
            if (action == keys::PRESS) perspsh(0.02f);
            break;
        
        default:
            if (used_vehicle)
            {
                plane->control(key, action);
            }
            else
            {
                foot->step(key, action, mods);
            }
    }
}

void nw_scene::mousedrag   (float /*xpos*/, float /*ypos*/, float xdiff, float ydiff, int /*dragbutton*/)
{
    /*
    if (!used_vehicle)
    {
        if (xdiff > -1000.0f && xdiff < 1000.0f && ydiff > -1000.0f && ydiff < 1000.0f)
        {
            foot->look(-xdiff * 0.4f, -ydiff * 0.4f);
        }
    }
    */
}
void nw_scene::mousemove   (float /*xpos*/, float /*ypos*/, float xdiff, float ydiff)
{
    if (!used_vehicle)
    {
        if (std::abs(xdiff) < 100.0f && std::abs(ydiff) < 100.0f)
        {
            foot->look(xdiff * 0.4f, ydiff * 0.4f);
        }
    }
    //std::cout << xdiff << " - " << ydiff << std::endl;
}
