
#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 in_color;
out vec3 vo_color;
    
void main () {
    gl_Position = vec4(position, 1.0);
    vo_color = in_color;
}
