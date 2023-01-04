#include "universe.hh"

#include <cmath>
#include <iostream>
#include <string>


universe::universe (cr::scripter& conf) : scene(conf)
{
    bool found;
    
    found = conf.getvalue("folders.target", target_dir);
    if (!found)
    {
        std::cerr << "ERROR: Target dir not found" << std::endl;
        return;
    }
    target_dir = cr::spath(target_dir);
    std::cout << "Target: " << target_dir << std::endl;
    
    conf.getvalue("folders.font_dir", font_dir);
    font_dir = cr::spath(font_dir);
    
    conf.getvalue("data.aeroplane_mesh", v_s, "mesh/vehicles/plane01x.bm1");
    std::cout << "V mesh: " << v_s << std::endl;
    
    //conf.getvalue("data", "planet", p_s, "mesh/terrain/tcol4/Planet_checkers.txt");
    //std::cout << "Planet: " << p_s << std::endl;

    conf.getvalue("data.vehicle", used_vehicle, 1);
    std::cout << "Vechic: " << used_vehicle << std::endl;
    if (used_vehicle < 0 || used_vehicle > 2)
    {
        std::cerr << "ERROR: Used vehicle: " << used_vehicle << std::endl;
        used_vehicle = 1;
    }
    
    
    max_dt = 2.0f;
    
    // aeroplane
    cr::camera* cam = new cr::camera();
    cam->near = 0.001f;
    cam->far = 100.0f;
    cam->focal = 1.0f;
    cam->setfov(25.0f);
    cam->persp_to_ortho();
    cam->aspect = 1.0f;
    //cam->pos  = cr::vec3(0.0f, 0.0f, 0.015f);
    cam->pos  = cr::vec3(0.0f, 0.0f, 0.0f);
    cam->look = cr::vec3(1.0f, 0.0f, 0.0f);
    cam->up   = cr::vec3(0.0f, 0.0f, 1.0f);
    
    // foot
    cr::camera* cam_f = new cr::camera();
    
    // spectator
    cr::camera* cam_s = new cr::camera();
    cam_s->near = 0.1f;
    cam_s->far = 100.0f;
    cam_s->focal = 1.0f;
    cam_s->setfov(25.0f);
    cam_s->persp_to_ortho();
    cam_s->aspect = 1.0f;
    cam_s->pos  = cr::vec3(0.0f, 0.0f, 0.0f);
    cam_s->look = cr::vec3(1.0f, 0.0f, 0.0f);
    cam_s->up   = cr::vec3(0.0f, 0.0f, 1.0f);
    
    cameras.push_back(cam_f);
    cameras.push_back(cam);
    cameras.push_back(cam_s);
    
    
    conf.getvalue("render.engine", renderengine, "gsgl");
    conf.getvalue("render.path_n", rmode.path_n, 100);
    rei = (renderengine =="gsgl") ? 1 :0;
    renderer_pt = new gscc::r_pathtracer();
    renderer_pt->setup(&rmode);
    
    cr::renderer* renderer0 = new gsgl::r_matrix_point();
    cr::renderer* renderer1 = new gsgl::r_matrix_line();
    cr::renderer* renderer2 = new gsgl::r_matrix_flat();
    cr::renderer* renderer3 = new gsgl::r_fish1_point();
    cr::renderer* renderer4 = new gsgl::r_fish1_line();
    cr::renderer* renderer5 = new gsgl::r_fish1_flat();
    cr::renderer* renderer6 = new gsgl::r_pp4_point();
    cr::renderer* renderer7 = new gsgl::r_pp4_line();
    cr::renderer* renderer8 = new gsgl::r_pp4_flat();
    renderer0->setup(&rmode);
    renderer1->setup(&rmode);
    renderer2->setup(&rmode);
    renderer3->setup(&rmode);
    renderer4->setup(&rmode);
    renderer5->setup(&rmode);
    renderer6->setup(&rmode);
    renderer7->setup(&rmode);
    renderer8->setup(&rmode);
    renderers.push_back(renderer0);
    renderers.push_back(renderer1);
    renderers.push_back(renderer2);
    renderers.push_back(renderer3);
    renderers.push_back(renderer4);
    renderers.push_back(renderer5);
    renderers.push_back(renderer6);
    renderers.push_back(renderer7);
    renderers.push_back(renderer8);
    
    showplane = true;
    used_cam = used_vehicle;
    
    foot_cam = new cr::foot_cam(cameras[0]);
    
    plane = new cr::aeroplane(cameras[1], "");
    plane->pos = cr::vec3(0.0f, 0.0f, 7.0f);
    if (used_vehicle == 1) plane->update_cam();
    vpos1 = plane->pos;
    planeCpu = cr::bm1_to_meshux(target_dir + cr::localize_path(v_s));
    gsgl::MUXtoGPU(*planeCpu, planeGpu);
    
    spec_cam = new cr::spectator_cam(cameras[2]);
    
    cr::vec_placed p1place;
    p1place.pos  = { 0, 0, 0 };
    p1place.look = { 0, 0, 1};
    p1place.up   = { 0, 1, 0};
    p1place.scale = 1.0f;
    cubic_sphere_planet* p1 = new cubic_sphere_planet(p1place, 8, 1, 100, 3);
    /*
    cr::VecPlaced p2place;
    p1place.pos  = { 1000, 200, 300 };
    p1place.look = { 0, 1, 0};
    p1place.up   = { 1, 0, 0};
    p1place.scale = 1.f;
    cubic_sphere_planet* p2 = new cubic_sphere_planet(p2place, 8, 4, 100, 3);
    */
    
    planets.push_back(p1);
    //planets.push_back(p2);
    
    //glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
    
    std::string font;
    conf.getvalue("ui.textconf", font);
    cr::scripter fontconf(target_dir + font_dir + font);
    uie = new gsui::uiengine(fontconf);
    
    nextCamMode();
    nextObjMode();
}

universe::~universe ()
{
    delete plane;
    delete planeCpu;
    gsgl::clearGPU(planeGpu);
    delete spec_cam;
    delete foot_cam;
    
    delete renderer_pt;
    
    delete uie;
    
    for (unsigned int i=0 ; i<planets.size() ; ++i) delete planets[i];
}


void universe::step (float dt)
{
    //cout << "dt: " << dt << endl;
    if (dt > max_dt) dt = max_dt;
    
    if (used_vehicle == 0)
    {
        //foot->tick(dt);
        //cr::vec3 np;
        //planet->newpos(cameras[0]->pos, np);
        //cameras[0]->pos = cr::vec3(np.x, np.y, np.z + 0.015f);
    }
    else if (used_vehicle == 1)
    {
        plane->tick(dt);
    }
    else if (used_vehicle == 2)
    {
        spec_cam->tick(dt);
    }
    
    /*
    if (used_vehicle == 1 && distxy(vpos1, plane->pos) > distUpdatePlanet)
    {
        std::cout << plane->pos.str() << " - " << vpos1.str() << " = " << distxy(vpos1, plane->pos) << std::endl;
        vpos1 = plane->pos;
        planet->updateTerrainParts(vpos1.x, vpos1.y);
    }
    */
}

void universe::init_render ()
{
    //renderers[used_render]->initRender(screen_width, screen_height);
}

void universe::pre_render ()
{
    //renderers[used_render]->preRender();
}

void universe::render ()
{
    //if (!(framebuf->is_complete())) { std::cout << "NC" << std::endl; }
    framebuf->use();

    //std::cout << "R1: " << used_render << "  R2: " << used_render_2 << "  C: " << used_cam << std::endl;
    //rmode.pp6size = 1.2f;
    
    if (rei)
    {
        renderers[used_render]->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
        renderers[used_render]->pre_render();
        
        if (showplane) renderers[used_render]->render(*(cameras[used_cam]), plane->model_mat(), planeGpu);
        
        for (unsigned int i=0 ; i<planets.size() ; ++i)
        {
            planets[i]->draw(*(cameras[used_cam]), renderers[used_render]);
        }
        
        if (used_render_2 >= 0)
        {
            //rmode.colourmode = x;
            
            renderers[used_render_2]->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(1.0f, 1.0f);
            if (showplane) renderers[used_render_2]->render(*(cameras[used_cam]), plane->model_mat(), planeGpu);
            
            for (unsigned int i=0 ; i<planets.size() ; ++i)
            {
                //int c = rmode.colourmode;
                //rmode.colourmode = 1;
                planets[i]->draw(*(cameras[used_cam]), renderers[used_render_2]);
                //rmode.colourmode = c;
            }
            
            glDisable(GL_POLYGON_OFFSET_FILL);
        }
        
        //uie->render();
    }
    else
    {
        renderer_pt->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
        renderer_pt->pre_render();
        
        if (showplane) renderer_pt->render(*(cameras[used_cam]), plane->model_mat(), *planeCpu);
        
        for (unsigned int i=0 ; i<planets.size() ; ++i)
        {
            planets[i]->draw_cpu(*(cameras[used_cam]), renderer_pt);
        }
        
        screen_tex->setdata(renderer_pt->screen);
    }

    framebuf->render();

    //std::cout << "END RENDER" << std::endl;
}



void universe::perspsh (float d)
{
    cameras[used_cam]->persp_shift(d);
}

void universe::save () {}

void universe::keyaction (int key, int action, int mods)
{
    switch (key)
    {
        case keys::I :
            if (action == keys::PRESS) nextCamMode();
            break;
        
        case keys::O :
            if (action == keys::PRESS) nextObjMode();
            break;
        
        case keys::P :
            // prostprocmode
            break;
        
        case keys::K :
            if (action == keys::PRESS) nextColMode();
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
        
        default:/*
            if      (used_vehicle == 0) foot_cam->step(key, action, mods);
            else if (used_vehicle == 1) plane->control(key, action);
            else if (used_vehicle == 2) spec_cam->step(key, action, mods);
            */
            plane->control(key, action);
            spec_cam->step(key, action, mods);
    }
}

void universe::mousedrag (float /*xpos*/, float /*ypos*/, float xdiff, float ydiff, int /*dragbutton*/)
{
    if (used_vehicle == 0)
    {
        if (xdiff > -1000.0f && xdiff < 1000.0f && ydiff > -1000.0f && ydiff < 1000.0f)
        {
            foot_cam->look(-xdiff * 0.4f, -ydiff * 0.4f);
        }
    }
    else if (used_vehicle == 2)
    {
        if (xdiff > -1000.0f && xdiff < 1000.0f && ydiff > -1000.0f && ydiff < 1000.0f)
        {
            spec_cam->look(-xdiff * 0.4f, -ydiff * 0.4f);
        }
    }
}
void universe::mousemove (float /*xpos*/, float /*ypos*/, float xdiff, float ydiff)
{
    if (used_vehicle == 0)
    {
        if (xdiff > -1000.0f && xdiff < 1000.0f && ydiff > -1000.0f && ydiff < 1000.0f)
        {
            foot_cam->look(xdiff * 0.4f, ydiff * 0.4f);
        }
    }
    else if (used_vehicle == 2)
    {
        if (xdiff > -1000.0f && xdiff < 1000.0f && ydiff > -1000.0f && ydiff < 1000.0f)
        {
            spec_cam->look(xdiff * 0.4f, ydiff * 0.4f);
        }
    }
}
