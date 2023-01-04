
#version 430 core

#include "func/pp6.glsl"


layout(location = 0) in vec3 position;
layout(location = 1) in vec3 in_color;


uniform int  proj;
uniform mat4 p;
uniform mat4 vm;
uniform float rad, aspect, nn, ff;

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
    
    if (colm == 1) color = in_color;
    else           color = base_color;
}
