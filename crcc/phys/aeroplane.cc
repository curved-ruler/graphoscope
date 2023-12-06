
#include "aeroplane.hh"

#include <iostream>

namespace cr {



aeroplane::aeroplane (camera* c, const std::string& /*vfile*/)
{
    cam = c;
    pos = vec3{ cam->pos.x, cam->pos.y - 0.5f, cam->pos.x - 0.3f } ;
    //orient = lu_transf(cam->look ,cam->up);
    //campos.set(0.0f, -0.5f, -0.3f);
    //geometry = new r::McDrawable(vfile);
    
    massCenter.set(0.0f, 0.0f, 0.0f);
    set_mass(1000.0f);
    set_inertia(100.0f);
    
    forw   = false;
    backw  = false;
    tup    = false; //dup = false;
    tdown  = false; //ddown  = false;
    tLroll = false; //dleft  = false;
    tRroll = false; //dright = false;
    tLyaw  = false;
    tRyaw  = false;
    
    thrust = true;
    
    damp = -0.01f; // - (F / vmax)
    adamp = -1.0f;
    
    maxvel = 0.03f;
    maxavel = 0.5f;
}
aeroplane::~aeroplane ()
{
    //delete geometry;
}

void aeroplane::update_cam ()
{
    vec3 up    = rot_q(orient, zi);
    vec3 look  = rot_q(orient, xi);
    vec3 right = cross(look, up);
    //up.normalize();
    //look.normalize();
    up /= 5.0f;
    look /= 5.0f;
    vec3 dir = -look + up*0.5f;
    
    cam->pos  = pos + dir * (cam->focal);
    cam->look = pos - cam->pos;
    cam->up   = -cross(right, cam->look); // ??? why the "-"
    cam->constrain();
}

void aeroplane::control (int key, int action)
{
    bool newval = !(action == keys::RELEASE);
    
    if (key == keys::UP)   { forw  = newval; }
    if (key == keys::DOWN) { backw = newval; }
    
    if (key == keys::S) { tup = newval; }
    if (key == keys::W) { tdown = newval; }
    
    if (key == keys::A) { tLroll = newval; }
    if (key == keys::D) { tRroll = newval; }
    if (key == keys::LEFT) { tLroll = newval; }
    if (key == keys::RIGHT) { tRroll = newval; }
    
    if (key == keys::Q) { tLyaw = newval; }
    if (key == keys::E) { tRyaw = newval; }
}

void aeroplane::tick (float dt)
{
    clear();
    
    vec3 look = rot_q(orient, xi);
    vec3 up = rot_q(orient, zi);
    vec3 right = cross(look, up);
    float vrot = velocity.length() * 30.0f;
    if (vrot > 0.7f) vrot = 0.7f;
    
    if (forw) {
        vec3 f = look*0.1f;
        add_force(f);
    }
    if (backw) {
        vec3 f = look*(-0.1f);
        add_force(f);
    }
    
    
    if (tup) {
        vec3 t = up*(0.2f*vrot);
        add_torque(t, look);
    }
    if (tdown) {
        vec3 t = up*(-0.2f*vrot);
        add_torque(t, look);
    }
    if (tLroll) {
        vec3 t = up*(0.2f*vrot);
        add_torque(t, right);
    }
    if (tRroll) {
        vec3 t = up*(0.2f*vrot);
        add_torque(t, -right);
    }
    if (tLyaw) {
        vec3 t = right*(0.2f*vrot);
        add_torque(t, -look);
    }
    if (tRyaw) {
        vec3 t = right*(0.2f*vrot);
        add_torque(t, look);
    }
    
    /*
    vec3 eulerAngVel = getRollPitchYawVelocity();
    if (dleft) {
        if (eulerAngVel.x > 0.0f) { //AAAAA
            //std::cout << eulerAngVel.str() << std::endl;
            vec3 t = up*10.5f*eulerAngVel.x;
            roll(t, -right);
        } else {
            vec3 t = up*10.5f*eulerAngVel.x;
            roll(t, right);
        }
    }
    
    if (dright) { //DDDDD
        if (eulerAngVel.x > 0.0f) {
            vec3 t = up*10.5f*eulerAngVel.x;
            roll(t, right);
        } else {
            vec3 t = up*10.5f*eulerAngVel.x;
            roll(t, -right);
        }
    }
    */
    
    if (thrust)
    {
        float len = velocity.length();
        if (len > cr::eps) {
            vec3 vn = velocity;
            vn /= len;
            float cosangle = dot(vn, look);
            vec3 ll = (cosangle > 0.0f) ? look : -look;
            vn += (ll - vn) * 0.2f;
            vn.setlength(len);
            velocity = vn;
        }
    }
    
    move(dt);
    orient.normalize();
    update_cam();
}

}
