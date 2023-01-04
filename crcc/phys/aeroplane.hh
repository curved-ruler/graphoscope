#pragma once

#include "../keys.hh"
#include "vehicle.hh"
#include "rigid_body.hh"
#include "../obj3/camera.hh"


namespace cr {


class aeroplane : public vehicle, public rigid_body
{
    private:
        camera* cam;
        
        // TODO bitmask
        bool forw;
        bool backw;
        bool tup;
        bool tdown;
        bool tLroll;
        bool tRroll;
        bool tLyaw;
        bool tRyaw;
        
        float thrust;
        
    public:
        aeroplane  (camera* c, const std::string& vfile);
        virtual ~aeroplane ();
        
        virtual void update_cam ();
        virtual void control    (int key, int action);
        virtual void tick       (float dt);
};

}
