#pragma once

#include "../keys.hh"
#include "../obj3/camera.hh"

namespace cr {

class foot_cam
{
    private:
        camera* cam;
        //VecPlaced vp1, vp2;
        
        int move_f;
        int move_s;
        bool fast;
        
    public:
         foot_cam (camera* c);
        ~foot_cam ();
        
        //virtual void updateCam ();
        void look (float xdiff, float ydiff);
        void step (int key, int action, int mods);
        void tick (float dt);
};

}
