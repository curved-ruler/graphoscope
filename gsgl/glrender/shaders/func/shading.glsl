
const float shininess = 4.0;

vec3 shade_diffuse (in vec3 color, in vec3 norm, in vec3 light)
{
    vec3 N = normalize(norm);
    //vec3 dnl = vec3(abs(dot(N,light)) * 0.5);
    //vec3 dnl = vec3((dot(N,light) + 0.3) * 0.1);
    return color*dot(N,light);
}

vec3 shade_specular (in vec3 color, in vec3 norm, in vec3 light, in vec3 view)
{
    light = -light;
    vec3 halfd = normalize(light + view);
    float sangle = max(dot(halfd, norm), 0.0);
    //float sangle = dot(halfd, norm) + 0.3;
    float specular = pow(sangle, shininess);
    //return clamp(color*specular*0.05, 0.0, 1.0);
    return color*specular;
}
