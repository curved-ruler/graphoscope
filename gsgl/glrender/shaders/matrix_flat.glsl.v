#version 430 core


layout(location = 0) in vec3 position;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec3 in_norm;

uniform vec3 base_color;
uniform mat4 viewmodel;
uniform mat4 proj;
uniform vec3 light;
uniform vec3 vpos;
uniform int colm;

out vec3 color;


#include "func/shading.glsl"

void main ()
{
    gl_Position = proj * viewmodel * vec4(position,1.0);
    vec4 ppos   = viewmodel * vec4(position,1.0);
    
    vec3 c;
    if (colm == 1) c = in_color;
    else           c = base_color;
    
    //vec3 col_spec = shade_specular(vec3(1.0,1.0,1.0), mat3(viewmodel)*in_norm, light, normalize(vpos-ppos.xyz));
    //vec3 col_diff = shade_diffuse (vec3(1.0,1.0,1.0), mat3(viewmodel)*in_norm, light);

    vec3 col_spec = shade_specular(c, mat3(viewmodel)*in_norm, light, normalize(vpos-ppos.xyz));
    vec3 col_diff = shade_diffuse (c, mat3(viewmodel)*in_norm, light);
    
    // gamma
    //color = pow(color_lin, vec3(1.0/2.2));
    
    color = 0*vec3(0.7, 1.0, 1.0) + 0.7*c + 0.3*col_diff + 0*col_spec;
}
