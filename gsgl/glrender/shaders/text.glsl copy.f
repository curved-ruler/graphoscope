
#version 430 core

precision mediump float;

in vec2 texf;
out vec4 col;
uniform sampler2D texsampler;

void main()
{
    float c = texture(texsampler, texf).r;
    col = vec4(c, c, c, 1.0);
}
