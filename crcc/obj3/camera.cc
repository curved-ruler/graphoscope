
#include "camera.hh"


namespace cr {



void camera::setfov(float f)
{
    pheight2 = std::tan(f) * focal;
}
void camera::constrain()
{
    lu_constrain(look, up);
}

mat4 camera::view_mat() const
{
    vec3 u;
    u = cross(look, up);
    
    mat4 trot = {
         u.x,     u.y,     u.z,    0,
         up.x,    up.y,    up.z,   0,
        -look.x, -look.y, -look.z, 0,
         0,       0,       0,      1
    };
    
    /*
    mat4 ttr = {
        1, 0, 0, -c->pos.x,
        0, 1, 0, -c->pos.y,
        0, 0, 1, -c->pos.z,
        0, 0, 0,   1
    };
    */
    mat4 ttr = move_mat(-pos);
    
    return trot*ttr;
}

mat4 camera::ortho_mat() const
{
    float b = 1.0f / oheight2;
    float a = b / aspect;
    float e = (-near - far) / (far - near);
    float f = -2.0f / (far - near);
    
    mat4 tr = {
         a, 0, 0, 0,
         0, b, 0, 0,
         0, 0, f, e,
         0, 0, 0, 1
    };
    return tr;
}
void camera::persp_to_ortho()
{
    //float tanf = tan(fovy2);
    //oheight2 = tanf * focal;
    oheight2 = pheight2;
}
mat4 camera::persp_mat() const
{
    //float tanf = tan(fovy2); 
    float tanf = pheight2 / focal;
    float a = 1.0f / (tanf * aspect);
    float b = 1.0f / tanf;
    float e = (-near - far) / (far - near);
    float f = (-2.0f * near * far) / (far - near);
    
    mat4 tr = {
         a,  0,  0,  0,
         0,  b,  0,  0,
         0,  0,  e,  f,
         0,  0, -1,  0
    };
    return tr;
}

bool camera::persp_shift (float dist)
{
    if (near+dist < 0.0001f || focal+dist > 10000.0f) return false;
    
    focal += dist;
    near  += dist;
    far   += dist;
    pos   += look*dist;
    //fovy2  = std::atan(std::tan(fovy2) * focal / (focal+dist));
    
    return true;
}

}
