
#version 430 core

// precision mediump float;
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D screenTexture;

void main()
{
    vec2 tdim = 1.0 / vec2(textureSize(screenTexture, 0));
    vec2 uvx1 = TexCoords + vec2(tdim.x,0.0);
    vec2 uvx2 = TexCoords - vec2(tdim.x,0.0);
    vec2 uvy1 = TexCoords + vec2(0.0,tdim.y);
    vec2 uvy2 = TexCoords - vec2(0.0,tdim.y);
    vec3 a = texture(screenTexture, uvx1).rgb - texture(screenTexture, uvx2).rgb;
    vec3 b = texture(screenTexture, uvy1).rgb - texture(screenTexture, uvy2).rgb;
    
    FragColor = vec4( (a+b)/2.0 + 0.3, 1.0);
}

