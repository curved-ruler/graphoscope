#pragma once

#include "../keys.hh"
#include "../obj3/camera.hh"

namespace cr {

class spectator_cam
{
    private:
        camera* cam;
        
        int forw;
        int strafe;
        int up;
        int rot;
        bool fast;
        
    public:
         spectator_cam (camera* c);
        ~spectator_cam ();
        
        //virtual void updateCam ();
        void look (float xdiff, float ydiff);
        void step (int key, int action, int mods);
        void tick (float dt);
};

}
