
#include "spectator_cam.hh"

#include <iostream>

namespace cr
{

spectator_cam::spectator_cam(camera* c)
{
    cam = c;
    forw   = 0;
    strafe = 0;
    up     = 0;
    rot    = 0;
    fast = false;
}
spectator_cam::~spectator_cam() {}

void spectator_cam::step(int key, int action, int /*mods*/)
{
    bool newval = !(action == keys::RELEASE);
    
    switch (key)
    {
        case keys::UP:
        case keys::W:
            if      (newval)     forw =  1;
            else if (forw ==  1) forw =  0;
            break;
            
        case keys::DOWN:
        case keys::S:
            if      (newval)     forw = -1;
            else if (forw == -1) forw =  0;
            break;
            
        case keys::LEFT:
        case keys::A:
            if      (newval)       strafe =  1;
            else if (strafe ==  1) strafe =  0;
            break;
            
        case keys::RIGHT:
        case keys::D:
            if      (newval)       strafe = -1;
            else if (strafe == -1) strafe =  0;
            break;
        
        case keys::R:
            if      (newval)   up =  1;
            else if (up ==  1) up =  0;
            break;
        case keys::F:
            if      (newval)   up = -1;
            else if (up == -1) up =  0;
            break;
        
        case keys::Q:
            if      (newval)    rot = -1;
            else if (rot == -1) rot =  0;
            break;
        case keys::E:
            if      (newval)    rot =  1;
            else if (rot ==  1) rot =  0;
            break;
        
        case keys::SHIFT:
            fast = newval;
            break;
        
        default:
            return;
    }
}
void spectator_cam::look(float xdiff, float ydiff)
{
    //std::cout << "X: " << xdiff << std::endl;
    float kd = 0.9f;
    vec3 left = cross(cam->up, cam->look);
    quat qx(kd*xdiff, cam->up);
    quat qy(kd*ydiff, left);
    
    cam->up   = rot_q(qy, cam->up);
    cam->look = rot_q(qy, cam->look);
    
    cam->look = rot_q(qx, cam->look);
    
    cam->constrain();
}

void spectator_cam::tick (float dt)
{
    float kd = (fast) ? 1.5f : 0.5f;
    
    //if (forw != 0)
    cam->pos += cam->look * (kd*(dt/8.0f)*forw);
    
    //if (strafe != 0)
    vec3 left = cross(cam->up, cam->look);
    cam->pos += left * (kd*(dt/8.0f)*strafe);
    
    //if (up != 0)
    cam->pos += cam->up * (kd*(dt/8.0f)*up);
    
    quat q(dt*rot, cam->look);
    cam->up = rot_q(q, cam->up);
    cam->constrain();
}

}
