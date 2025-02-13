
#include "universe.hh"

#include <cmath>
#include <climits>
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
    
    
    max_dt = 2.0f;
    
    cr::camera* cam = new cr::camera();
    cam->near = 0.1f;
    cam->far = 1000.0f;
    cam->focal = 70.0f;
    cam->setfov(25.0f * cr::dtor);
    cam->persp_to_ortho();
    cam->aspect = 1.0f;
    //cam->pos  = cr::vec3(0.0f, 0.0f, 0.015f);
    cam->pos  = cr::vec3(0.0f, 0.0f, 0.0f);
    cam->look = cr::vec3(1.0f, 0.0f, 0.0f);
    cam->up   = cr::vec3(0.0f, 0.0f, 1.0f);
    
    cameras.push_back(cam);
    
    rrr_n = new gsgl::r_nshaded();
    rrr_v = new gsgl::r_vshaded();
    rrr_n->setup(&rmode);
    rrr_v->setup(&rmode);
    
    
    
    shipCpu = cr::bm1_to_meshux(target_dir + std::string("mesh/spaceship_a.bm1"));
    gsgl::MUXtoGPU(*shipCpu, shipGpu);
    pos   = {  0, 0, 0 };
    look  = {  1, 0, 0 };
    up    = {  0, 0, 1 };
    scale = 1.0f;
    
    up2    = {  0, 0, 1 };
    look2  = {  1, 0, 0 };
    up2a   = 0.0f;
    look2a = 0.0f;
    
    
    p1 = new planet(target_dir + std::string("mesh/planet_001.bm1"));
    p2 = new planet(target_dir + std::string("mesh/planet_002.bm1"));
    
    p1->pos  = { 50, -10, 10 };
    p1->look = { 1, 0, 0 };
    p1->up   = { 0, 1, 0 };
    p1->scale = 0.2f;
    
    p2->pos  = { 70, -15, 0 };
    p2->look = { 0, 1, 0 };
    p2->up   = { 1, 0, 0 };
    p2->scale = 1.2f;
    
    
    
    int N = 2000;
    float stars[N*6];
    uint32 x = 120;
    uint32 y = 200;
    for (int i=0 ; i<N ; ++i)
    {
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        y ^= y << 13;
        y ^= y >> 17;
        y ^= y << 5;
        float xx = float(x) / float(UINT_MAX);
        xx = std::acos(2.0f*xx-1.0f);
        float yy = float(y) / float(UINT_MAX);
        yy = 2.0f*cr::pi*yy;
        stars[i*6]     = std::sin(xx) * std::cos(yy);
        stars[i*6 + 1] = std::sin(xx) * std::sin(yy);
        stars[i*6 + 2] = std::cos(xx);
        stars[i*6 + 3] = 1.0f;
        stars[i*6 + 4] = 1.0f;
        stars[i*6 + 5] = 0.0f;
    }
    glGenBuffers(1, &(starsGpu.pnt_buf));
    glBindBuffer(GL_ARRAY_BUFFER, starsGpu.pnt_buf);
    glBufferData(GL_ARRAY_BUFFER, N*6 * sizeof(float), stars, GL_STATIC_DRAW);
    starsGpu.pntn = N;
    
    rmode.back[0] = 0.0f;
    rmode.back[1] = 0.0f;
    rmode.back[2] = 0.0f;
    
    //glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
}

universe::~universe ()
{
    delete shipCpu;
    gsgl::clearGPU(shipGpu);
    delete p1;
    delete p2;
}


void universe::step (float /*dt*/)
{
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
    rmode.camtype = 1;
    rmode.objtype = 1;
    rmode.colourmode = 0;
    rmode.col[0] = 1.0f;
    rmode.col[1] = 1.0f;
    rmode.col[2] = 1.0f;
    
    cr::mat4 mx = cr::rotx_mat(15.5f);
    cr::mat4 m0;
    
    //if (!(framebuf->is_complete())) { std::cout << "NC" << std::endl; }
    framebuf->use();
    
    //std::cout << "R1: " << used_render << "  R2: " << used_render_2 << "  C: " << used_cam << std::endl;
    //rmode.pp6size = 1.2f;
    
    rmode.objtype = 0;
    rrr_n->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
    rrr_n->pre_render();
    glDisable(GL_DEPTH_TEST);
    rrr_n->render(*(cameras[0]), m0, starsGpu);
    glEnable(GL_DEPTH_TEST);
    
    rmode.objtype = 1;
    m0  = cr::move_mat(cameras[0]->pos);
    //rrr_n->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
    rrr_n->render(*(cameras[0]), m0, shipGpu);
    
    m0  = cr::move_mat(p1->pos);
    m0 *= cr::scale_mat(p1->scale);
    rrr_n->render(*(cameras[0]), m0, p1->geom_gpu);
    m0  = cr::move_mat(p2->pos);
    m0 *= cr::scale_mat(p2->scale);
    rrr_n->render(*(cameras[0]), m0 * mx, p2->geom_gpu);
    
    rmode.objtype = 2;
    rrr_v->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);
    rmode.col[0] = 0.0f;
    rmode.col[1] = 0.0f;
    rmode.col[2] = 1.0f;
    m0  = cr::move_mat(p1->pos);
    m0 *= cr::scale_mat(p1->scale);
    rrr_v->render(*(cameras[0]), m0, p1->geom_gpu);
    m0  = cr::move_mat(p2->pos);
    m0 *= cr::scale_mat(p2->scale);
    rrr_v->render(*(cameras[0]), m0 * mx, p2->geom_gpu);
    glDisable(GL_POLYGON_OFFSET_FILL);

    framebuf->render();

    //std::cout << "END RENDER" << std::endl;
}


void universe::save () {}

void universe::keyaction (int /*key*/, int /*action*/, int /*mods*/)
{
}

void universe::mousemove (float /*xpos*/, float /*ypos*/, float xdiff, float ydiff)
{
    if (xdiff > -50.0f && xdiff < 50.0f && ydiff > -50.0f && ydiff < 50.0f)
    {
        float kd = 0.4f*cr::dtor;
        
        //float neg = cr::dot(look, look2);
        //neg = neg > 0.0f ? -1.0f : 1.0f;
        up2a   += kd*xdiff;
        look2a -= kd*ydiff;
        cr::vec3 left = cr::cross(look2, up2);
        float ls = left.length();
        /*
        if (ls < 0.01)
        {
            left = cr::cross(up, up2);
            ls = left.length();
        }
        */
        left /= ls;
        a = cr::rot_mat( up2a, up2);
        b = cr::rot_mat( look2a, left);
        
        cameras[0]->up   = a*b*up2;
        cameras[0]->look = a*b*look2;
        
        //cameras[0]->constrain();
        
        /*
        if (std::abs(up2a) > 90.0f*cr::dtor || std::abs(look2a) > 90.0f*cr::dtor)
        {
            up2    = cameras[0]->up;
            look2  = cameras[0]->look;
            up2a   = 0.0f;
            look2a = 0.0f;
        }
        */
        
        //up2 = cameras[0]->up;
        //look2 = cameras[0]->look;
        
        
        /*
        cr::vec3 left = cr::cross(up2, cameras[0]->look);
        cr::quat qx(kd*xdiff, up2);
        cr::quat qy(kd*ydiff, left);
        
        cameras[0]->up   = cr::rot_q(qx, cameras[0]->up);
        cameras[0]->look = cr::rot_q(qx, cameras[0]->look);
        
        cameras[0]->up   = cr::rot_q(qy, cameras[0]->up);
        cameras[0]->look = cr::rot_q(qy, cameras[0]->look);
        */
    }
}
