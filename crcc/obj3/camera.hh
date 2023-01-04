#pragma once

#include "../math/mu.hh"


namespace cr {

class camera
{
    public:
        vec3 pos;
        vec3 look;
        vec3 up;
        
        float near;
        float far;
        float aspect;
        
        float oheight2;
        float pheight2;
        //float fovy2;
        float focal;
        
    public:
        mat4 view_mat  () const;
        mat4 ortho_mat () const;
        mat4 persp_mat () const;
        
        //bool check();
        void constrain();
        
        void setfov         (float f);
        void persp_to_ortho ();
        bool persp_shift    (float dist);
};

}
