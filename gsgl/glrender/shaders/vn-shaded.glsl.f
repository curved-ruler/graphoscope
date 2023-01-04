
#version 430 core

// precision mediump float;
in vec3 color;

uniform float alpha;

out vec4 out_color;

void main()
{
    out_color = vec4(color, alpha);
    //float z1 = gl_FragCoord.z * 2.0 - 1.0;
    //float z2 = (2.0 * 0.001 * 10.0) / (10.0 + 0.001 - z1 * (10.0 - 0.001));
    //out_color = vec4(vec3(1.0-z2), alpha);
    //out_color = vec4(vec3(gl_FragCoord.z / 1.5), alpha);
}
