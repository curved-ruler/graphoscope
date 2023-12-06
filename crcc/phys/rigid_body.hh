#pragma once

#include "../math/mu.hh"
#include "../obj3/obj3.hh"



namespace cr {

class rigid_body
{
    public:
        vec3 pos;
        quat orient;
        
    protected:
        float damp;
        float adamp;
        float maxvel;
        float maxavel;
        
        float imass;
        float iinertia;
        vec3 massCenter;
        vec3 forces;
        vec3 torques;
        vec3 acceleration;
        vec3 angularacc;
        vec3 velocity;
        vec3 angularvel;
        
        // TODO general place (vec3) (used in transform also)
        vec3 xi, yi, zi;

    public:
        rigid_body();
        virtual ~rigid_body();
        
        void set_mass(float mass);
        void set_inertia(float i);
        //void setMassCenter(mu::vec3 mc);
        
        vec3 bodyref(vec3 space) const;
        
        void clear();
        void add_dampers();
        
        // unused, untested
        vec3 get_RollPitchYaw_Velocity() const;
        
        void add_force(vec3 f);
        vec3 add_torque(vec3 t, vec3 tpos);
        
        void move(float dt);
};

}
