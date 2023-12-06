
#include "foot_cam.hh"

#include <iostream>

namespace cr
{

foot_cam::foot_cam(camera* c)
{
    cam = c;
    move_f = 0;
    move_s = 0;
    fast = false;
}
foot_cam::~foot_cam() {}

void foot_cam::step(int key, int action, int /*mods*/)
{
    bool newval = !(action == keys::RELEASE);
    
    switch (key)
    {
        case keys::UP:
        case keys::W:
            if      (newval)      move_f = 1;
            else if (move_f == 1) move_f = 0;
            break;
            
        case keys::DOWN:
        case keys::S:
            if      (newval)       move_f = -1;
            else if (move_f == -1) move_f =  0;
            break;
            
        case keys::LEFT:
        case keys::A:
            if      (newval)      move_s = 1;
            else if (move_s == 1) move_s = 0;
            break;
            
        case keys::RIGHT:
        case keys::D:
            if      (newval)       move_s = -1;
            else if (move_s == -1) move_s =  0;
            break;
        
        case keys::SHIFT:
            fast = newval;
            break;
        
        default:
            return;
    }
}
void foot_cam::look(float xdiff, float ydiff)
{
    //std::cout << "X: " << xdiff << std::endl;
    float kd = 0.9f * dtor;
    vec3 zi(0.0f, 0.0f, 1.0f);
    vec3 left = cross(cam->up, cam->look);
    quat qx(kd*xdiff, zi);
    quat qy(kd*ydiff, left);
    
    vec3 nu = rot_q(qy, cam->up);
    vec3 nl = rot_q(qy, cam->look);
    
    if (nu.z > 0.001f)
    {
        cam->up = nu;
        cam->look = nl;
    }
    
    cam->up   = rot_q(qx, cam->up);
    cam->look = rot_q(qx, cam->look);
    
    cam->constrain();
}

void foot_cam::tick (float dt)
{
    float kd = (fast) ? 1.2f : 0.05f;
    
    if (move_f != 0)
    {
        vec3 d = vec3(cam->look.x, cam->look.y, 0.0f);
        d.normalize();
        cam->pos += d*(kd*(dt/1000.0f)*move_f);
    }
    if (move_s != 0)
    {
        vec3 d = cross(cam->up, cam->look);
        d.z = 0.0f;
        d.normalize();
        cam->pos += d*(kd*(dt/1000.0f)*move_s);
    }
}

}
