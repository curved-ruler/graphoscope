
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
    sysconf.getvalue("window.cam_handling", cam_handling, 0);
    sysconf.getvalue("window.alpha", rmode.alpha, 0.3f);
    
    float f = (cam_handling == 0) ? 500.0f * 0.9f : 1000.0f;
    cr::camera* cam0 = new cr::camera();
    cam0->near  = 1.0f;
    cam0->far   = f;
    cam0->focal = f / 2.0f;
    cam0->pos   = cr::vec3(-500.0f,  0.0f, 0.0f);
    cam0->up    = cr::vec3( 0.0f,  0.0f, 1.0f);
    cam0->look  = cr::vec3( 1.0f,  0.0f, 0.0f);
    cam0->setfov(25.0f);
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
    
    
    mtr.scale = 1.0f;
    mtr.pos  = {0.0f, 0.0f,  0.0f};
    mtr.look = {1.0f, 0.0f,  0.0f};
    mtr.up   = {0.0f, 0.0f,  1.0f};
    axis     = 0.0;
    rotation = 0.0;
    rotdir   = true;
    
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
    
    /*
    rmode.back[0] = 1.0f;
    rmode.back[1] = 1.0f;
    rmode.back[2] = 1.0f;
    sysconf.getarray("window.backgr", rmode.back, 3);
    */
    
    sysconf.getvalue("data.simn", simn, 1);
    sysconf.getvalue("data.simr", simr, 10.0f);
    
    planet = new uv_sphere_planet(sysconf, mtr, radius, cr::powi(2, lod), K);
    
    nextCamMode();
    nextObjMode();
    nextObjMode();
    nextColMode();
    nextColMode();
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
    if (cam_handling == 0)
    {
        if (y > 0.0f)
        {
            mtr.scale *= 1.25f;
        }
        else
        {
            mtr.scale *= 0.8f;
        }
    }
    else
    {
        //float p = std::max(std::fabs(cameras[0]->pos.x), 100.0f);
        if (y > 0.0f)
        {
            //cameras[0]->pos.x += 0.05f * p;
            cameras[0]->pos.x += 70.0f;
        }
        else
        {
            //cameras[0]->pos.x -= 0.05f * p;
            cameras[0]->pos.x -= 70.0f;
        }
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
    
    float scale = 0.2f * cameras[used_cam]->far / planet->radius;
    
    mtr2.identity();
    mtr2 *= cr::scale_mat(scale);
    mtr2 *= cr::roty_mat(axis);
    mtr2 *= cr::rotz_mat(rotation);
    
    //cameras[0]->aspect = (float)screen_width / (float)screen_height;
    
    renderers[used_render]->pre_render();
    
    /*
    renderers[1]->initRender(screen_width / rmode.pixel_size, screen_height / rmode.pixel_size);
    renderers[1]->render(*(cameras[used_cam]), mtr2 * mtr.modelTr(), worldCoordGpu);
    */
    
    renderers[used_render]->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
    planet->draw(*(cameras[used_cam]), mtr2 * mtr.model_mat(), renderers[used_render]);
    //renderers[used_render]->render(*(cameras[used_cam]), mtr2 * mtr.modelTr(), gpuModel);
    
    if (used_render_2 >= 0)
    {
        //rmode.pp6size = 0.9f;
        renderers[used_render_2]->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0f, 1.0f);
        
        //renderers[used_render_2]->render(*(cameras[used_cam]), mtr2 * mtr.modelTr(), gpuModel);
        planet->draw(*(cameras[used_cam]), mtr2 * mtr.model_mat(), renderers[used_render_2]);
        
        glDisable(GL_POLYGON_OFFSET_FILL);
    }

#ifdef DEBUG_CRACK
     renderers[1]->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
     renderers[1]->render(*(cameras[used_cam]), mtr2 * mtr.model_mat(), planet->debugBuff);
#endif

    framebuf->render();
}



void planetmaker_base::keyaction(int key, int action, int /*mods*/) {
    if (action == keys::RELEASE) return;
    switch (key)
    {
        case keys::I :
            nextCamMode();
            break;
            
        case keys::O :
            nextObjMode();
            break;
            
        case keys::P :
            // prostprocmode
            break;
            
        case keys::K :
            nextColMode();
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
            //planet->erosion_step();
            break;
        
        case keys::H :
            planet->rnd_octaves();
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
