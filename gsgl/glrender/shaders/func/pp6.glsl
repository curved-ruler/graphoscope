
// in vec4 projection
//    =
// in float rad, in float aspect, in float nn, in float ff

vec3 safe_norm (in vec3 v)
{
    return (abs(v.x) < 0.00001 && abs(v.y) < 0.00001 && abs(v.z) < 0.00001) ? vec3(0.0, 0.0, 0.0) : normalize(v);
}

vec4 transform6pp (in vec4 position, in mat4 vm, in float rad, in float aspect, in float nn, in float ff)
{
    vec4 v   = vm * position;
    vec3 iv  = normalize(v.xyz);
    vec3 vxy = normalize(vec3(v.x, v.y, 0.0000001));
    float d  = dot(vec3(0.0, 0.0, -0.999999), iv);
    //float a  = (d < 0.99) ? acos(d) : sqrt(iv.x*iv.x + iv.y*iv.y);
    float a = acos(d);
    vec3 r   = vxy * a * rad;
    
    //float z  = -sign(v.z)*(length(v.xyz) / (ff-nn) - nn);
    //float z  = -v.z / (ff-nn) - nn;
    //if (v.z > 0.0) z = 0.0;
    
    float beta = (1 + ff/nn) / (1 - ff/nn);
    float alph = (-1-beta) / nn;
    float z  = alph * length(v.xyz) + beta;
    if (v.z > 0.0) z = -1.05;

    //float dz = 1.0 - dot(iv, vxy);
    
    return vec4(r.x / aspect, r.y, z, 1.0);
}

vec4 transform4pp (in vec4 position, in mat4 vm, in float rad, in float aspect, in float nn, in float ff)
{
    vec4 v   = vm * position;
    vec3 v2  = normalize(v.xyz);
    vec3 v3  = normalize(vec3(v.x, 0.0, v.z));
    float x  = rad / dot(v2, v3);
    float d  = dot(vec3(0.0, 0.0, -1.0), v3);
    float a  = (d < 0.99) ? acos(d) : abs(v3.x);
    
    float beta = (1 + ff/nn) / (1 - ff/nn);
    float alph = (-1-beta) / nn;
    float z  = alph * length(v.xyz) + beta;
    if (v.z > 0.0) z = -2.0;
    
    return vec4((v.x >= 0.0 ? a : -a) * rad / aspect, (x*v2).y, z, 1.0);
}
