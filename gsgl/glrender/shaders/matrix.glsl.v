#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 in_color;

uniform vec3 base_color;
uniform mat4 pvm;
uniform int colm;

out vec3 color;


void main () {
    gl_Position = pvm * vec4(position, 1.0);
    
    if (colm == 1) color = in_color;
    else           color = base_color;
}
