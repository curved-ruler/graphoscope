
#pragma once


namespace cr {

class vehicle
{
    public:
        virtual void update_cam () = 0;
        virtual void control    (int key, int action) = 0;
        virtual void tick       (float dt) = 0;
};

}
