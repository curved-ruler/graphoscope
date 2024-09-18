
#include "uv_sphere_planet.hh"

#include <cmath>
#include <iostream>
#include <cstring>


uv_sphere_planet::uv_sphere_planet (cr::scripter& _conf, float _r, int _part_size, int _part_k)
  : N(_part_size)
  , K(_part_k)
  , radius(_r)
  , conf(_conf)
{
    th0 = cr::pi / 11.0f;
    
    hy = N*K;
    hx = 2*N*K;
    heights   = new float[(hx+1) * (hy+1)];
    std::memset(heights, 0, (hx+1) * (hy+1) * sizeof(float));
    hmin = 0;
    hmax = 0;
    
    dth = (cr::pi - 2.0f * th0) / (hy);
    dfi = (2.0f * cr::pi)       / (hx);
    
    dice01 = new cr::fdice(120, 0.0f, 1.0f);
    
    noised = new ter::del_noise();
    noised->xscale = 1.0f;
    noised->yscale = 1.0f;
    noised->xp = 0.0f;
    noised->yp = 0.0f;
    
    std::string palname;
    conf.getvalue("colscheme.used", palname);
    int palsize  = conf.arraysize(std::string("colscheme.limits")+palname);
    int palsize2 = conf.arraysize(std::string("colscheme.cols")  +palname);
    if (palsize2 != (palsize+1)*3) { std::cout << "WARNING: palette size mismatch" << std::endl; }
    
    cr::height_pal* MAT = new cr::height_pal(palsize+1);
    float* cc = new float[palsize2];
    float* ll = new float[palsize];
    conf.getarray(std::string("colscheme.cols")  +palname, cc, palsize2);
    conf.getarray(std::string("colscheme.limits")+palname, ll, palsize);
    for (int i=0 ; i<palsize+1 ; ++i) { MAT->cols[i]   = cr::vec3{ cc[i*3], cc[i*3+1], cc[i*3+2] }; }
    for (int i=0 ; i<palsize   ; ++i) { MAT->limits[i] = ll[i]; }
    
    int blend;
    conf.getvalue("colscheme.blend", blend, 4);
    if (blend < 1)
    {
        std::cout << "WARNING: palette blend value: " << blend << std::endl;
        blend = 1;
    }
    pal = new cr::height_pal(*MAT, blend);
    
    delete[] cc;
    delete[] ll;
    delete MAT;
    
    create_mesh();
}

uv_sphere_planet::~uv_sphere_planet ()
{
    delete[] heights;
    
    delete dice01;
    delete noised;
    
    gsgl::clearGPU(planetGpu);
    gsgl::clearGPU(debugBuff);
}

void uv_sphere_planet::stat ()
{
    hmax = -10000;
    hmin =  10000;
    for (int i=0 ; i<hy ; ++i)
    {
        for (int j=0 ; j<hx ; ++j)
        {
            float h = heights[i*(hx+1) + j];
            if (h > hmax) hmax = h;
            if (h < hmin) hmin = h;
        }
    }
    std::cout << "height: [" << hmin << ", " << hmax << "]\n"; 
}

void uv_sphere_planet::create_mesh ()
{
    planetMesh.clear();
    
    float th, fi;
    cr::vec3 v0, v1, v2, v3, v4, norm;
    
    for (int i_th = 0 ; i_th < hy ; ++i_th)
    {
        for (int i_fi = 0 ; i_fi < hx ; ++i_fi)
        {
            th  = th0 + (float)i_th * dth;
            fi  = (float)i_fi * dfi;
            v0 = cr::sphere_cartesian(radius, th, fi);
            v0.setlength(heights[i_th*(hx+1) + i_fi] + radius);
            
            fi = (float)(i_fi+1) * dfi;
            v1 = cr::sphere_cartesian(radius, th, fi);
            v1.setlength(heights[i_th*(hx+1) + i_fi+1] + radius);
            
            th = th0 + (float)(i_th+1) * dth;
            v3 = cr::sphere_cartesian(radius, th, fi);
            v3.setlength(heights[(i_th+1)*(hx+1) + i_fi+1] + radius);
            
            fi  = (float)i_fi * dfi;
            v2 = cr::sphere_cartesian(radius, th, fi);
            v2.setlength(heights[(i_th+1)*(hx+1) + i_fi] + radius);
            
            unsigned int ci = pal->get(heights[i_th*(hx+1) + i_fi]);
            norm = cross((v2-v0), (v1-v0));
            norm.normalize();
            
            planetMesh.add_tri(v0, v2, v1, cr::material { pal->cols[ci] }, norm);
            planetMesh.add_tri(v1, v2, v3, cr::material { pal->cols[ci] }, norm);
            
            planetMesh.add_lin(v0, v1, cr::material { pal->cols[ci] });
            planetMesh.add_lin(v0, v2, cr::material { pal->cols[ci] });
            
            planetMesh.add_pnt(v0, cr::material { pal->cols[ci] });
        }
    }
    
    create_contour();
}

void uv_sphere_planet::create_contour ()
{
    contourMesh.clear();
    
    cr::vec3 v0, v1, v2, v4, v5, newnorm, norm, col;
    cr::vec3 zi(0.0f, 0.0f, 1.0f);
    
    //float dz = 0.2f * radius / 40.0f;
    float dz = 20.0f;
    conf.getvalue("contour.dz", dz, 10.0f);
    
    int tas  = (int)cr::mesh_ux::tsize;
    int tris = (int)planetMesh.triangles.size() / tas;
    
    for (float planez = radius+hmin ; planez <= radius+hmax ; planez += dz)
    {
        for (int x=0 ; x<tris ; ++x)
        {
            v0.x = planetMesh.triangles[x*tas];
            v0.y = planetMesh.triangles[x*tas+1];
            v0.z = planetMesh.triangles[x*tas+2];
            v1.x = planetMesh.triangles[x*tas+9];
            v1.y = planetMesh.triangles[x*tas+10];
            v1.z = planetMesh.triangles[x*tas+11];
            v2.x = planetMesh.triangles[x*tas+18];
            v2.y = planetMesh.triangles[x*tas+19];
            v2.z = planetMesh.triangles[x*tas+20];
            
            //norm = (v1-v0) % (v2-v0);
            //norm.normalize();
            
            //int z0 = cr::zero(planez-v0.z, 0.01f) ? 0 : ((planez > v0.z) ? -1 : 1 );
            //int z1 = cr::zero(planez-v1.z, 0.01f) ? 0 : ((planez > v1.z) ? -1 : 1 );
            //int z2 = cr::zero(planez-v2.z, 0.01f) ? 0 : ((planez > v2.z) ? -1 : 1 );
            //int z3 = cr::zero(planez-v3.z, 0.01f) ? 0 : ((planez > v3.z) ? -1 : 1 );
            
            float r0 = sphere_radius(v0);
            float r1 = sphere_radius(v1);
            float r2 = sphere_radius(v2);
            
            int z0 = ((planez > r0) ? -1 : 1 );
            int z1 = ((planez > r1) ? -1 : 1 );
            int z2 = ((planez > r2) ? -1 : 1 );
            
            unsigned int ci = pal->get(r0 - radius);
            
            bool intersect = false;
            

            if ((z0 == 1 && z1 == -1) || (z0 == -1 && z1 == 1))
            {
                v4 = mix(v0, v1, cr::abs(planez-r0) / cr::abs(r0-r1));
                if (z2 == z0)
                {
                    v5 = mix(v1, v2, cr::abs(planez-r1) / cr::abs(r2-r1));
                }
                else
                {
                    v5 = mix(v0, v2, cr::abs(planez-r0) / cr::abs(r2-r0));
                }
                
                //newnorm = (v4-v5) % zi;
                //newnorm.normalize();
                
                intersect = true;
                
            }
            if ((z0 == 1 && z2 == -1) || (z0 == -1 && z2 == 1))
            {
                v4 = mix(v0, v2, cr::abs(planez-r0) / cr::abs(r0-r2));
                if (z1 == z0)
                {
                    v5 = mix(v1, v2, cr::abs(planez-r1) / cr::abs(r2-r1));
                }
                else
                {
                    v5 = mix(v0, v1, cr::abs(planez-r0) / cr::abs(r1-r0));
                }
                
                //newnorm = (v5-v4) % zi;
                //newnorm.normalize();
                
                intersect = true;
            }
            if ((z1 == 1 && z2 == -1) || (z1 == -1 && z2 == 1))
            {
                v4 = mix(v1, v2, cr::abs(planez-r1) / cr::abs(r1-r2));
                if (z0 == z1)
                {
                    v5 = mix(v0, v2, cr::abs(planez-r0) / cr::abs(r0-r2));
                }
                else
                {
                    v5 = mix(v0, v1, cr::abs(planez-r0) / cr::abs(r1-r0));
                }
                
                //newnorm = (v5-v4) % zi;
                //newnorm.normalize();
                
                intersect = true;
            }
            
            if (intersect)
            {
                //if (!same_facing(norm, newnorm))
                //{
                //    newnorm = -newnorm;
                //}
                
                //cr::vec3 v4z(v4.x, v4.y, planez - dz);
                //cr::vec3 v5z(v5.x, v5.y, planez - dz);
                
                //contourMesh.AddTriangle(v5, v4,  v4z, pal->cols[ci], newnorm);
                //contourMesh.AddTriangle(v5, v4z, v5z, pal->cols[ci], newnorm);
                
                if (planez < 0.0f)
                {
                    v4.setlength(radius);
                    v5.setlength(radius);
                }
                contourMesh.add_lin(v4, v5, cr::material { pal->cols[ci] });
                //contourMesh.AddLine(v4, v5, cr::vec3{0,0,0});
            }
        }
    }
    
    gsgl::clearGPU(planetGpu);
    gsgl::MUXtoGPU(planetMesh, contourMesh, planetGpu);
    //gsgl::MUXtoGPU(planetMesh, planetGpu);
}

void uv_sphere_planet::crack (int k, float d)
{
    cr::vec3 v, vn;
    float th, fi;
    
    for (int i=0 ; i<k ; ++i)
    {
        float fi_n = 2.0f * cr::pi * dice01->next();
        float th_n = std::acos(2.0f * dice01->next() - 1.0f);
        vn = cr::sphere_cartesian(radius, th_n, fi_n);
        
#ifdef DEBUG_CRACK
        debugMesh.add_lin(vn.withlength(radius*1.1f), vn.withlength(radius*1.2f), cr::material { cr::vec3(1.0f, 0.0f, 0.0f) });
#endif
        
        for (int i_th = 0 ; i_th <= hy ; ++i_th)
        {
            for (int i_fi = 0 ; i_fi < hx ; ++i_fi)
            {
                th  = th0 + (float)i_th * dth;
                fi  = (float)i_fi * dfi;
                
                v   = cr::sphere_cartesian(radius, th, fi);
                
                if (dot(v, vn) > 0.0f)
                {
                    heights[i_th*(hx+1) + i_fi] += d;
                }
                else
                {
                    heights[i_th*(hx+1) + i_fi] -= d;
                }
            }
        }
        for (int y = 0 ; y <= hy ; ++y) { heights[y*(hx+1) + hx] = heights[y*(hx+1)]; }
    }
    
#ifdef DEBUG_CRACK
    gsgl::clearGPU(debugBuff);
    gsgl::MUXtoGPU(debugMesh, debugBuff);
#endif
    
    stat();
    create_mesh();
}

void uv_sphere_planet::ds ()
{
    /*
    float hm, sigma;
    conf.getvalue("ds.radius_per_rand", hm, 10.0f);
    conf.getvalue("ds.sigma", sigma, 2.0f);
    hm = radius / hm;
    
    cr::fdice dice(220, -1.0f, 1.0f);
    //cr::fdice dice_checkers(200, -1000.0f, 100.0f);
    cr::idice dice_checkers(200, -4, 4);
    
    
    for (int y=N ; y < hy ; y+=N)
    {
        for (int x = 0 ; x < hx ; x+=N)
        {
            heights[y*(hx+1) + x] = dice_checkers.next() * 1.0f;
        }
        heights[y*(hx+1) + hx] = heights[y*(hx+1)];
    }
    for (int x = 0 ; x <= hx ; ++x) { heights[x] = -20.0f; heights[(hy)*(hx+1) + x] = -20.0f; }
    */
    ter::ds_square_grid(heights, N, 2*K, K);
    stat();
    create_mesh();
    return;
    /*
    int i = N;
    float isigma = 1.0f;
    
    while (i >= 2)
    //while (i >= 4)
    {
        int ip2 = i/2;
        
        // diamond
        for (int y=0 ; y+i <= hy ; y+=i)
        {
            //float hm2 = std::sin(da*(y+ip2));
            
            for (int x=0 ; x+i <= hx ; x+=i)
            {
                float a = heights[y*(hx+1) + x];
                float b = heights[(y+i)*(hx+1) + x];
                float c = heights[y*(hx+1) + (x+i)];
                float d = heights[(y+i)*(hx+1) + (x+i)];
                
                //float hm2 = ( (y+ip2) < (N2/2) || (y+ip2) > (N*11/4) ) ? 0.0f : 1.0f;
                
                float r = dice.next() * hm / isigma;
                
                heights[(y+ip2) * (hx+1) + (x+ip2)] = (a + b + c + d) / 4.0f + r;
            }
        }
        
        // square
        int alt = 0;
        for (int y = ip2 ; y < hy ; y+=ip2)
        {
            //float hm2 = std::sin(da*(y));
            
            for (int x=(alt % 2 == 1) ? ip2 : 0 ; x < hx ; x+=i)
            {
                float p = 4.0f;
                
                float a = 0.0f;
                if (x-ip2 >= 0)  { a = heights[y*(hx+1) + (x-ip2)]; }
                else             { p -= 1.0f; }
                
                float b = 0.0f;
                if (x+ip2 < hx)  { b = heights[y*(hx+1) + (x+ip2)]; }
                else             { b = heights[y*(hx+1) + (x+ip2-hx)]; }
                
                float c = 0.0f;
                if (y-ip2 >= 0)  { c = heights[(y-ip2)*(hx+1) + x]; }
                else             { p -= 1.0f; }
                
                float d = 0.0f;
                if (y+ip2 <= hy) { d = heights[(y+ip2)*(hx+1) + x]; }
                else             { p -= 1.0f; }
                
                //float hm2 = ( (y) < (N2/2) || (y) > (N*11/4) ) ? 0.0f : 1.0f;
                
                float r = dice.next() * hm / isigma;
                
                heights[y*(hx+1) + x] = (a + b + c + d) / p + r;
            }
            ++alt;
        }
        
        // step
        i = ip2;
        isigma *= sigma;
    }
    
    for (int y = 0 ; y <= hy ; ++y) { heights[y*(hx+1) + hx] = heights[y*(hx+1)]; }
    */
    /*
    int lr = N/7;
    float lambda = 1.0f;
    float dl = lambda / (float)lr;
    for (int y = lr ; y >= 0 ; --y)
    {
        for (int x = 0 ; x<= 4*N ; ++x)
        {
            heights[y*w + x]         = lambda * heights[y*w + x];
            heights[(3*N - y)*w + x] = lambda * heights[(3*N - y)*w + x];
        }
        lambda -= dl;
    }
    */
    
    stat();
    create_mesh();
}

void uv_sphere_planet::noise1 ()
{
    float min, max, sigma;
    int oct_n;
    conf.getvalue("octaves.rndo_min", min, -1.0f);
    conf.getvalue("octaves.rndo_max", max,  1.0f);
    conf.getvalue("octaves.sigma", sigma, 2.0f);
    conf.getvalue("octaves.oct_n", oct_n, 1);
    
    ter::noise_grad n;
    
    for (int y=0 ; y <= hy ; ++y)
    {
        for (int x = 0 ; x < hx ; ++x)
        {
            heights[y*(hx+1) + x] = (max-min) * n.fbm(float(x)/64.0f, float(y)/64.0f, oct_n, 1.0f/sigma) + min;
        }
        heights[y*(hx+1) + hx] = heights[y*(hx+1)];
    }
    
    stat();
    create_mesh();
}

void uv_sphere_planet::del_noise ()
{
    float min, max, sigma;
    int oct_n;
    conf.getvalue("octaves.rndo_min", min, -1.0f);
    conf.getvalue("octaves.rndo_max", max,  1.0f);
    conf.getvalue("octaves.sigma", sigma, 2.0f);
    conf.getvalue("octaves.oct_n", oct_n, 1);
    
    for (int y=0 ; y <= hy ; ++y)
    {
        for (int x=0 ; x < hx ; ++x)
        {
            heights[y*(hx+1) + x] = (max-min) * noised->value_noise(float(x)/32.0f, float(y)/32.0f) + min;
        }
        heights[y*(hx+1) + hx] = heights[y*(hx+1)];
    }
    
    stat();
    create_mesh();
}

/*
void uv_sphere_planet::draw (const cr::camera& cam, cr::renderer* rrr) const
{
    rrr->render(cam, model_mat(), planetGpu);
}
*/
void uv_sphere_planet::draw (const cr::camera& cam, cr::mat4 tr, cr::renderer* rrr) const
{
    rrr->render(cam, tr, planetGpu);
}

