
#version 430 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 texin;

uniform float viewportx;
uniform float viewporty;
out vec2 texf;

void main ()
{
    gl_Position = vec4(( 2.0*pos.x - viewportx) / viewportx,
                       (-2.0*pos.y + viewporty) / viewporty, 0.0, 1.0);
    texf = texin;
}
