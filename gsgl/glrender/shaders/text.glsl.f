
#version 430 core

precision mediump float;

in vec2 texf;
in vec3 c1;
in vec3 c2;

out vec4 col;

uniform sampler2D texsampler;

void main()
{
    float t = texture(texsampler, texf).r;
    vec3 c  = c1*t + c2*(1.0-t);
    col     = vec4(c, 1.0);
}
