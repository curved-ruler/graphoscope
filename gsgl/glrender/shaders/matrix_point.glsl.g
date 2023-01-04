
#version 430 core

layout(triangles) in;
layout(points, max_vertices=$NUMP$) out;
in vec3 vo_color[3];

uniform vec3 base_color;
uniform mat4 pvm;
uniform int colm;
uniform float coordinates[2*$NUMP$];

out vec3 color;

void main() {

    for (int i=0 ; i<$NUMP$ ; ++i)
    //for (int i=0 ; i<1 ; ++i)
    {
        /*
        if (coordinates[i] > -0.01 && coordinates[i] < 0.01) {
            color = vec4(1.0, 0.0, 0.0, 1.0);
        } else if (coordinates[i] > 0.32 && coordinates[i] < 0.34) {
            color = vec4(0.0, 1.0, 0.0, 1.0);
        } else if (coordinates[i] > 0.65 && coordinates[i] < 0.67) {
            color = vec4(0.0, 0.0, 1.0, 1.0);
        } else {
            color = vec4(0.0, 0.0, 0.0, 1.0);
        }
        */
        
        if (colm == 1) color = vo_color[0];
        else           color = base_color;
        
        vec4 p =  gl_in[0].gl_Position +
                 (gl_in[1].gl_Position - gl_in[0].gl_Position) * coordinates[2*i] +
                 (gl_in[2].gl_Position - gl_in[0].gl_Position) * coordinates[2*i + 1];
        gl_Position = pvm * p;
        EmitVertex();
    }
}
