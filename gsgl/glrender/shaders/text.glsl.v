
#version 430 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 texin;
layout (location = 2) in uint cols;

uniform float viewportx;
uniform float viewporty;
out vec2 texf;
out vec3 c1;
out vec3 c2;

void main ()
{
    gl_Position = vec4(( 2.0*pos.x - viewportx) / viewportx,
                       (-2.0*pos.y + viewporty) / viewporty, 0.0, 1.0);
    texf = texin;
    
    uint c12 = cols;
    
    uint b2 = c12 & 0x1F;
    c12 >>= 5;
    uint g2 = c12 & 0x1F;
    c12 >>= 5;
    uint r2 = c12 & 0x1F;
    c12 >>= 5;
    
    uint b1 = c12 & 0x1F;
    c12 >>= 5;
    uint g1 = c12 & 0x1F;
    c12 >>= 5;
    uint r1 = c12 & 0x1F;
    
    float cm = float(0x1F);
    c1 = vec3( float(r1)/cm, float(g1)/cm, float(b1)/cm );
    c2 = vec3( float(r2)/cm, float(g2)/cm, float(b2)/cm );
}
