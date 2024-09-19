
#include "planetmaker_base.hh"

#include <cmath>
#include <iostream>
#include <string>

#include <crcc/crcc.hh>



planetmaker_base::planetmaker_base(cr::scripter& _conf) : scene(_conf)
{
    bool found;
    
    found = sysconf.getvalue("folders.target", target_dir);
    if (!found) { std::cerr << "ERROR: conf: folders.target_dir not found" << std::endl; }
    target_dir = cr::spath(target_dir);
    
    int K;
    sysconf.getvalue("data.lod", lod, 2);
    sysconf.getvalue("data.k", K, 1);
    sysconf.getvalue("data.radius", radius, 100.0f);
    sysconf.getvalue("window.alpha", rmode.alpha, 0.3f);
    
    
    cr::camera* cam0 = new cr::camera();
    cam0->near  = 1.0f;
    cam0->far   = 500.0f;
    cam0->focal = 500.0f * 0.9f;
    cam0->pos   = cr::vec3(-500.0f,  0.0f, 0.0f);
    cam0->up    = cr::vec3( 0.0f,  0.0f, 1.0f);
    cam0->look  = cr::vec3( 1.0f,  0.0f, 0.0f);
    cam0->setfov(25.0f * cr::dtor);
    cam0->persp_to_ortho();
    cam0->aspect = 1.0f;
    
    cameras.push_back(cam0);
    
    /*
    cr::camera* cam1 = new cr::camera();
    cam1->near = 0.01f;
    cam1->far = 100.0f;
    cam1->focal = 4.0f;
    cam1->up   = cr::vec3(0.0f, 1.0f,  1.0f);
    cam1->up.normalize();
    cam1->pos  = cr::vec3(0.0f, 0.0f,  3.0f);
    cam1->look = cr::vec3(0.0f, 1.0f, -1.0f);
    cam1->look.normalize();
    cam1->setfov(25.0f);
    cam1->persp_to_ortho();
    cam1->aspect = 1.0f;
    
    cameras.push_back(cam1);
    */
    
    
    rmode.camtype = 1;
    rmode.objtype = 1; objrender = 1;
    rmode.colourmode = 2;
    rmode.proctype = 0;
    
    rmode.pointsize = 20.0f;
    
    cr::renderer* rrr_n = new gsgl::r_nshaded();
    cr::renderer* rrr_v = new gsgl::r_vshaded();
    rrr_n->setup(&rmode);
    rrr_v->setup(&rmode);
    renderers.push_back(rrr_n);
    renderers.push_back(rrr_v);
    
    /*
    cr::SimpleMeshBuilder mb;
    float da = cr::pi / (patch_lod+2);
    float th = da/2.0f;
    float fi = 0;
    cr::vec3 v0, v1, v2, v3;
    for (int i_th=0 ; i_th<=patch_lod ; ++i_th)
    {
        for (int i_fi=0 ; i_fi<2*(patch_lod+2) ; ++i_fi)
        {
            v0 = cr::sphere_cartesian(patch_r, th, fi);
            v1 = cr::sphere_cartesian(patch_r, th, fi+da);
            v2 = cr::sphere_cartesian(patch_r, th+da, fi);
            v3 = cr::sphere_cartesian(patch_r, th+da, fi+da);
            mb.AddTriangle(v0, v2, v1, cr::vec3{1, 1, 1});
            mb.AddTriangle(v1, v2, v3, cr::vec3{1, 1, 1});
            
            fi += da;
        }
        th += da;
    }
    modelgeom = mb.CreateMeshB();
    r::MBtoGPU(modelgeom, gpuModel);
    */
    
    worldCoord = new cr::mesh_ux(0,3,0);
    worldCoord->set_lin(0, cr::vec3{0,0,0}, cr::vec3{1,0,0}, cr::material { cr::vec3{1,0,0} });
    worldCoord->set_lin(1, cr::vec3{0,0,0}, cr::vec3{0,1,0}, cr::material { cr::vec3{0,1,0} });
    worldCoord->set_lin(2, cr::vec3{0,0,0}, cr::vec3{0,0,1}, cr::material { cr::vec3{0,0,1} });
    gsgl::MUXtoGPU(*worldCoord, worldCoordGpu);
    
    sysconf.getvalue("crack.simn", simn, 1);
    sysconf.getvalue("crack.simr", simr, 10.0f);
    
    planet = new uv_sphere_planet(sysconf, radius, cr::powi(2, lod), K);
    planet->scale = 1.0f;
    planet->pos  = {0.0f, 0.0f,  0.0f};
    planet->look = {1.0f, 0.0f,  0.0f};
    planet->up   = {0.0f, 0.0f,  1.0f};
    axis     = 0.0;
    rotation = 0.0;
    rotdir   = true;
}

planetmaker_base::~planetmaker_base()
{
    //std::cout << "DelB:   " << (void*)glDeleteBuffers << std::endl;
    //std::cout << "DelFrB: " << (void*)glDeleteFramebuffers << std::endl;
    //std::cout << "DelRB:  " << (void*)glDeleteRenderbuffers << std::endl;
    delete worldCoord;
    gsgl::clearGPU(worldCoordGpu);
}



void planetmaker_base::step (float /*dt*/) {}
void planetmaker_base::save () {}

void planetmaker_base::mousescroll (float /*x*/, float y)
{
    if (y > 0.0f)
    {
        planet->scale *= 1.25f;
    }
    else
    {
        planet->scale *= 0.8f;
    }
}

void planetmaker_base::mousedrag(float /*xpos*/, float /*ypos*/, float xdiff, float ydiff, int /*btn*/)
{
    if (rotdir)
    {
        rotation -= xdiff;
    }
    else
    {
        rotation += xdiff;
    }
    axis -= ydiff;
    
    // Ensure [0,360]
    axis = axis - std::floor(axis/360.0f)*360.0f;
    rotation = rotation - std::floor(rotation/360.0f)*360.0f;
}
void planetmaker_base::mousebutton (int /*key*/, int action, int /*mods*/)
{
    if (action == keys::PRESS) startrot();
}
void planetmaker_base::startrot()
{
    if ((axis > 90.0f) && (axis < 270.0f)) {
        rotdir = false;
    } else {
        rotdir = true;
    }
}

void planetmaker_base::init_render()
{
    //renderers[used_render]->initRender(screen_width, screen_height);
}

void planetmaker_base::pre_render()
{
    //renderers[used_render]->preRender();
}

void planetmaker_base::render()
{
    framebuf->use();
    
    rmode.objtype = (objrender < 3) ? objrender : objrender-3;
    used_render   = (rmode.objtype < 2) ? 0 : 1;
    used_render_2 = (objrender < 3) ? -1 : 1;
    
    float scale = 0.2f * cameras[used_cam]->far / planet->radius;
    
    mtr.identity();
    mtr *= cr::scale_mat(scale);
    mtr *= cr::roty_mat(axis * cr::dtor);
    mtr *= cr::rotz_mat(rotation * cr::dtor);
    
    renderers[used_render]->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
    renderers[used_render]->pre_render();
    planet->draw(*(cameras[used_cam]), mtr * cr::scale_mat(planet->scale), renderers[used_render]);
    
    if (used_render_2 >= 0)
    {
        rmode.objtype = 2;
        //rmode.colourmode = x;
        //rmode.pp6size = 0.9f;
        renderers[used_render_2]->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0f, 1.0f);
        planet->draw(*(cameras[used_cam]), mtr * cr::scale_mat(planet->scale), renderers[used_render_2]);
        glDisable(GL_POLYGON_OFFSET_FILL);
    }

#ifdef DEBUG_CRACK
     renderers[1]->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
     renderers[1]->render(*(cameras[used_cam]), mtr * cr::scale_mat(planet->scale), planet->debugBuff);
#endif
    
    framebuf->render();
}



void planetmaker_base::keyaction(int key, int action, int /*mods*/) {
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
            ++(rmode.proctype); if (rmode.proctype >= cr::render_mode::pop_n) rmode.proctype=0;
            break;
            
        case keys::K :
            ++(rmode.colourmode); if (rmode.colourmode >= cr::render_mode::col_n) rmode.colourmode=0;
            break;
        /*
        case keys::A :
            ++px;
            planet->updateTerrainParts(px, py);
            break;
            
        case keys::D :
            --px;
            planet->updateTerrainParts(px, py);
            break;
            
        case keys::W :
            --py;
            planet->updateTerrainParts(px, py);
            break;
            
        case keys::S :
            ++py;
            planet->updateTerrainParts(px, py);
            break;
        */
        
        case keys::A :
            //move(10.0f, 0.0f);
            mousedrag(0.0f, 0.0f, -5.0f, 0.0f, 0);
            break;
            
        case keys::D :
            //move(-10.0f, 0.0f);
            mousedrag(0.0f, 0.0f, 5.0f, 0.0f, 0);
            break;
            
        case keys::W :
            //move(0.0f, -10.0f);
            mousedrag(0.0f, 0.0f, 0.0f, 5.0f, 0);
            break;
            
        case keys::S :
            //move(0.0f, 10.0f);
            mousedrag(0.0f, 0.0f, 0.0f, -5.0f, 0);
            break;
            
        case keys::G :
            planet->ds();
            break;
            
        case keys::C :
            planet->crack(simn, simr);
            break;
            
        case keys::R :
            planet->del_noise();
            break;
        
        case keys::H :
            planet->noise1();
            break;
            
        case keys::V :
            (cameras[0]->far > 600.0f) ? cameras[0]->far = 500.0f : cameras[0]->far = 1000.0f;
            break;
            
        case keys::MINUS :
            mousescroll(0.0f, -0.2f);
            break;
            
        case keys::PLUS :
            mousescroll(0.0f, 0.2f);
            break;
            
        case keys::BUT_1 :
            //planet->lessgap();
            break;
            
        case keys::BUT_2 :
            //planet->moregap();
            break;
        
        case keys::BUT_4 :
            //planet->switchflow();
            break;
        
        /*
        case keys::BUT_9 :
            ++used_cam;
            if (used_cam > 1) used_cam = 0;
            break;
        */
        
        /*
        case keys::BUT_4 :
            planet->less();
            break;
            
        case keys::BUT_5 :
            planet->more();
            break;
        */
        default:
            return;
    }
}
