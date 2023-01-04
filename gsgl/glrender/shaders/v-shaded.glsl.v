
#version 430 core

#include "func/pp6.glsl"
#include "func/shading.glsl"


layout(location = 0) in vec3 position;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec3 in_norm;


uniform int  proj;
uniform mat4 p;
uniform mat4 vm;
uniform float rad, aspect, nn, ff;

uniform vec3 light;
uniform vec3 vpos;

uniform int colm;
uniform vec3 base_color;

out vec3 color;



void main ()
{
    switch (proj)
    {
        case 0:
        case 1:  gl_Position = p * vm * vec4(position,1.0); break;
        case 2:  gl_Position = transform6pp(vec4(position,1.0), vm, rad, aspect, nn, ff); break;
        default: gl_Position = transform4pp(vec4(position,1.0), vm, rad, aspect, nn, ff); break;
    }
    vec4 ppos   = vm * vec4(position,1.0);
    
    vec3 c;
    if (colm == 1) c = in_color;
    else           c = base_color;
    
    vec3 col_spec = shade_specular(c, mat3(vm)*in_norm, light, normalize(vpos-ppos.xyz));
    vec3 col_diff = shade_diffuse (c, mat3(vm)*in_norm, light);
    
    // gamma
    //color = pow(color_lin, vec3(1.0/2.2));
    
    color = 0*vec3(0.7, 1.0, 1.0) + 0.7*c + 0.3*col_diff + 0*col_spec;
}
