
#include "rigid_body.hh"

#include <cmath>
#include <iostream>

using namespace std;




namespace cr {

rigid_body::rigid_body()
{
    xi = vec3(1.0f, 0.0f, 0.0f);
    yi = vec3(0.0f, 1.0f, 0.0f);
    zi = vec3(0.0f, 0.0f, 1.0f);
    
    // to be override
    damp  = 0.0f;
    adamp = 0.0f;
    maxvel = 0.0f;
    maxavel = 0.0f;
    
    orient.set(1.0f, 0.0f, 0.0f, 0.0f);
}
rigid_body::~rigid_body() {}

void rigid_body::set_mass(float mass)
{
    imass = 1.0f / mass;
}
void rigid_body::set_inertia(float i)
{
    iinertia = 1.0f / i;
}
/*
void rigid_body::setMassCenter(vec3 mc) {
    massCenter = mc;
}
*/

void rigid_body::clear()
{
    forces.nill();
    torques.nill();
}

void rigid_body::add_dampers()
{
    // mass?
    //if (velocity.lenn() < 0.00000001f) {
        //velocity.nill();
    //} else {
        vec3 veldamp = velocity;
        veldamp.normalize();
        veldamp *= damp;
        add_force(veldamp);
    //}
    
    //if (angularvel.length() < 0.001f) {
        //angularvel.nill();
    //} else {
        vec3 wn = angularvel;
        wn.normalize();
        vec3 r = norm_ortho(wn);
        r.normalize();
        vec3 t = (cross(angularvel, r))*(adamp*2.0f);
        //cout << angularvel.str() << "       " << r.str() << endl;
        add_torque(t, r);
    //}
}

void rigid_body::add_force(vec3 f)
{
    forces += f;
}
vec3 rigid_body::add_torque(vec3 f, vec3 fpos)
{
    vec3 r = fpos - massCenter;
    vec3 t = cross(r, f);
    torques += t;
    return t;
}




vec3 rigid_body::bodyref(vec3 space) const
{
    vec3 look = rot_q(orient, xi);
    vec3 left = rot_q(orient, yi);
    vec3 up = cross(look, left);
    mat4 trot = {
        look.x, look.y, look.z, 0,
        left.x, left.y, left.z, 0,
         up.x,   up.y,   up.z,  0,
           0,      0,      0,   1
    };
    return trot*space;
}

vec3 rigid_body::get_RollPitchYaw_Velocity() const
{
    vec3 av = bodyref(angularvel);
    quat q = axistoquat(av);
    float r = std::atan2(2*(q.r*q.i + q.j*q.k), 1-2*(q.i*q.i + q.j*q.j));
    float p = std::asin (2*(q.r*q.j - q.i*q.k));
    float y = std::atan2(2*(q.r*q.k + q.i*q.j), 1-2*(q.j*q.j + q.k*q.k));
    return vec3(r, p, y);
}





void rigid_body::move(float dt)
{
    add_dampers();
    
    pos      += velocity*dt + acceleration*(dt*dt*0.5f);
    acceleration = forces*imass;
    velocity += acceleration*dt;
    velocity.maximize(maxvel);
    
    angularacc = torques*iinertia;
    //apitch = tpitch*iinertia;
    //aroll = troll*iinertia;
    /*
    if (angularacc.length() < 0.00001f) {
        angularvel.set(0.0f, 0.0f, 0.0f);
    }
    */
    quat w(0.0f, angularvel.x, angularvel.y, angularvel.z);
    // TODO: long equation
    orient += (w*orient)*(dt*0.5f);
    angularvel += angularacc*dt;
    angularvel.maximize(maxavel);
    
    //vpitch += apitch*dt;
    //vroll += aroll*dt;
}

}
