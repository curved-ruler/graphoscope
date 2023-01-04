
#include <cmath>
#include <cstring>
#include <iostream>

#include <apps/appbase/scene.hh>
#include <apps/appbase/glfw_loop.hh>
#include <crcc/crcc.hh>
#include <gsgl/gsgl.hh>

glfw_loop* global_loop;

void key_callback         (GLFWwindow* /*wnd*/, int key, int scancode, int action, int mods) { global_loop->key(key, scancode, action, mods); }
void scroll_callback      (GLFWwindow* /*wnd*/, double xoffset, double yoffset)              { global_loop->scroll(xoffset, yoffset); }
void cursorpos_callback   (GLFWwindow* /*wnd*/, double xpos, double ypos)                    { global_loop->cursorpos(xpos, ypos); }
void mousebutton_callback (GLFWwindow* /*wnd*/, int button, int action, int mods)            { global_loop->mousebutton(button, action, mods); }



namespace TR
{
    
    float e_2 (float x) { return x<0 ? -x*x : x*x; }
    float e_4 (float x) { return x<0 ? -x*x*x*x : x*x*x*x; }
    
    cr::vec3 tr00 (const cr::vec3& p, float /*param*/)
    {
        return p;
    }
    
    cr::vec3 tr01 (const cr::vec3& p, float param)
    {
        return cr::vec3(e_2(p.x)*param,
                        e_2(p.y)*param,
                        0.0f);
    }
    
    cr::vec3 tr02 (const cr::vec3& p, float /*param*/)
    {
        return cr::vec3(p.x < 0.0f ? -(p.x+p.y)*(p.x+p.y) : (p.x+p.y)*(p.x+p.y),
                        //p.y < 0 ? -p.y*p.y : p.y*p.y,
                        3.0f*p.y,
                        p.z);
    }
    
    cr::vec3 tr03 (const cr::vec3& p, float /*param*/)
    {
        return cr::vec3(p.x,
                        p.x+p.y < 0.0f ? pow(-(p.x+p.y), 0.5f) : pow((p.x+p.y), 0.5f),
                        p.z);
    }
    
    cr::vec3 tr04 (const cr::vec3& p, float /*param*/)
    {
        return cr::vec3(p.x,
                        0.001f*p.y*p.x*p.x + p.y,
                        p.z);
    }
    
    cr::vec3 tr05 (const cr::vec3& p, float /*param*/)
    {
        return cr::vec3(p.x*(p.x+p.y),
                        p.y*(p.x+p.y),
                        p.z);
    }
    
    cr::vec3 tr06 (const cr::vec3& p, float /*param*/)
    {
        float a = p.x*(fabs(p.x)+fabs(p.y));
        float b = p.y*(fabs(p.x)+fabs(p.y));
        
        return cr::vec3(a,
                        b,
                        //0.2f*a*b
                        0.0f);
    }
    
    cr::vec3 tr07 (const cr::vec3& p, float /*param*/)
    {
        float a = p.x*p.x-p.y*p.y;
        float b = 2.0f*p.x*p.y;
        
        return cr::vec3(a,
                        b,
                        //0.1f*a*b
                        0.0f);
    }
    
    // complex power z^param
    cr::vec3 tr08 (const cr::vec3& p, float param)
    {
        float r  = std::sqrt(p.x*p.x + p.y*p.y);
        float fi = param * std::atan2(p.y, p.x);
        //float fi2 = param * fi;
        float r2 = std::pow(r, param);
        return cr::vec3{ r2*std::cos(fi), r2*std::sin(fi), p.z };
    }
    
    cr::vec3 tr09 (const cr::vec3& p, float param)
    {
        bool a = p.x < p.y;
        bool b = p.x*p.y < 0.0f;
        float r  = std::sqrt(p.x*p.x + p.y*p.y);
        float fi = param * std::atan2(p.y, p.x);
        //float fi2 = param * fi;
        float r2 = std::pow(r, param);
        cr::vec3 ret { r2*std::cos(fi), r2*std::sin(fi), p.z };
        if (a) ret.x = -ret.x;
        if (b) ret.y = -ret.y;
        return ret;
    }
    
    // exp(z)
    cr::vec3 tr10 (const cr::vec3& p, float /*param*/)
    {
        return cr::vec3{ std::exp(p.x)*std::cos(p.y), std::exp(p.x)*std::sin(p.y), p.z };
    }
    
    
    typedef cr::vec3(*trfunc)(const cr::vec3&, float);
    constexpr int NT = 11;
    const trfunc transformations[NT] = { tr00, tr01, tr02, tr03, tr04, tr05, tr06, tr07, tr08, tr09, tr10 };
    
    
    
    cr::vec3 trmix(const cr::vec3& p, float param, float t)  { return tr06(p, param) * (1.0f-t) + tr05(p, param) * (t); }
    
    cr::vec3 tr(const cr::vec3& p, float param, int tr_i)
    {
        if (tr_i>=0 && tr_i<=NT) return (*transformations[tr_i])(p, param);
        else                     return (*transformations[0])(p, param);
    }
};

class plt_scene : public scene
{
    private:
        std::string target_dir;
        
        int xdiv, ydiv;
        float d, dx, dy;
        
        cr::mesh_ux grid;
        cr::mesh_ux grid_tr;
        cr::rrr_buffers halo;
        
        gsgl::r_nshaded* rrr;
        
        unsigned int linn, trin;
        cr::material col;
        
        float tmix;
        float start;
        float ds;
        int steps;
        
        float p, dp;
        
        int tr_first;
        
        float scale;
        float axis;
        float rotation;
        bool  rotdir;
        
        void init_square_halo()
        {
            std::cout << "SQUARE" << std::endl;
            
            linn = 0;
            trin = 0;
            
            xdiv = 101;
            ydiv = 101;
            dx = 1.0f;
            dy = 1.0f;
            scale = 1.0f;
            
            float xx, yy;
            
            for (int y=0 ; y < ydiv ; ++y)
            {
                for (int x=0 ; x < xdiv ; ++x)
                {
                    xx = ( -((xdiv-1) / 2) + x ) * dx;
                    yy = ( -((ydiv-1) / 2) + y ) * dy;
                    
                    //if (y == 0) std::cout << "X " << xx << std::endl;
                    grid.add_lin(cr::vec3{xx, yy, 0.0f}, cr::vec3{xx+dx, yy, 0.0f}, col);
                    grid.add_lin(cr::vec3{xx, yy, 0.0f}, cr::vec3{xx, yy+dy, 0.0f}, col);
                    grid_tr.add_lin(cr::vec3{xx, yy, 0.0f}, cr::vec3{xx+dx, yy, 0.0f}, col);
                    grid_tr.add_lin(cr::vec3{xx, yy, 0.0f}, cr::vec3{xx, yy+dy, 0.0f}, col);
                    linn += 2;
                    
                    grid.add_tri(cr::vec3{xx, yy, 0.0f}, cr::vec3{xx, yy+dy, 0.0f}, cr::vec3{xx+dy, yy, 0.0f}, col);
                    grid_tr.add_tri(cr::vec3{xx, yy, 0.0f}, cr::vec3{xx, yy+dy, 0.0f}, cr::vec3{xx+dy, yy, 0.0f}, col);
                    //grid.add_tri(cr::vec3{xx+dx, yy+dy, 0.0f}, cr::vec3{xx+dx, yy, 0.0f}, cr::vec3{xx, yy+dy, 0.0f}, col);
                    trin += 1;
                }
                //std::cout << "Y " << yy << std::endl;
            }
            
            transform();
            gsgl::MUXtoGPU(grid_tr, halo);
        }
        
        void init_hexa_halo()
        {
            std::cout << "HEXA" << std::endl;
            
            linn = 0;
            trin = 0;
            
            d = 1.0f;
            xdiv = 99;
            ydiv = 99;
            scale = 1.0f;
            
            cr::vec3 vdy(0.0f, d*std::sqrt(3.0f), 0.0f);           vdy  /= 10.0f;
            cr::vec3 vdx1(d*1.5f,  d*std::sqrt(3.0f)/2.0f, 0.0f);  vdx1 /= 10.0f;
            cr::vec3 vdx2(d*1.5f, -d*std::sqrt(3.0f)/2.0f, 0.0f);  vdx2 /= 10.0f;
            
            cr::vec3 v0( d/2.0f, -d*std::sqrt(3.0f)/2.0f, 0.0f);   v0 /= 10.0;
            cr::vec3 v1(-d/2.0f, -d*std::sqrt(3.0f)/2.0f, 0.0f);   v1 /= 10.0;
            cr::vec3 v2(-d,               0.0f          , 0.0f);   v2 /= 10.0;
            cr::vec3 v3(-d/2.0f,  d*std::sqrt(3.0f)/2.0f, 0.0f);   v3 /= 10.0;
            
            cr::vec3 vfirst(-((float)(xdiv+1) / 2.0f)*(1.5f*d), -((float)(ydiv+1) / 2.0f)*(std::sqrt(3.0f)*d), 0.0f);
            vfirst /= 10.0f;
            cr::vec3 v;
            
            for (int x=0 ; x <= xdiv ; ++x)
            {
                
                v = vfirst;
                
                for (int y=0 ; y <= ydiv ; ++y)
                {
                    grid.add_lin(v + v0, v + v1, col);
                    grid.add_lin(v + v1, v + v2, col);
                    grid.add_lin(v + v2, v + v3, col);
                    grid_tr.add_lin(v + v0, v + v1, col);
                    grid_tr.add_lin(v + v1, v + v2, col);
                    grid_tr.add_lin(v + v2, v + v3, col);
                    linn += 3;
                    
                    grid.add_tri(v, v + v0, v + v1, col);
                    grid_tr.add_tri(v, v + v0, v + v1, col);
                    trin += 1;
                    
                    v += vdy;
                }
                
                (x%2) ? vfirst += vdx1 : vfirst += vdx2;
            }
            
            transform();
            gsgl::MUXtoGPU(grid_tr, halo);
        }
        
    public:
        plt_scene(cr::scripter& _conf) : scene(_conf)
        {
            _conf.getvalue("folders.target", target_dir);
            
            cr::camera* cam0 = new cr::camera();
            cam0->near = 1.0f;
            cam0->far = 500.0f;
            cam0->focal = 10.0f;
            cam0->pos  = cr::vec3(0.0f, 0.0f,  10.0f);
            cam0->look = cr::vec3(0.0f, 0.0f, -1.0f);
            cam0->up   = cr::vec3(0.0f, 1.0f,  0.0f);
            cam0->oheight2 = 2.0f;
            cam0->pheight2 = 2.0f;
            cam0->aspect = 1.0f;
    
            cameras.push_back(cam0);
            
            rrr = new gsgl::r_nshaded();
            rrr->setup(&rmode);
            
            
            rmode.back[0] = 0.0f;
            rmode.back[1] = 0.0f;
            rmode.back[2] = 0.0f;
            rmode.back[3] = 1.0f;
            sysconf.getarray("window.backgr", rmode.back, 3);
            
            col.albedo = {0.0f, 0.0f, 0.0f};
            float c[3];
            sysconf.getarray("window.col", c, 3);
            col.albedo.x = c[0];
            col.albedo.y = c[1];
            col.albedo.z = c[2];
            
            scale = 1.0f;
            
            steps = 0;
            ds    = 0.02f;
            //start = -1.0f;
            start = 0.0f;
            tmix = start;
            std::cout << "T " << tmix << std::endl;
            
            _conf.getvalue("data.P",  p, 1.0f);
            _conf.getvalue("data.dP", dp, 0.1f);
            std::cout << "P " << p << std::endl;
            
            _conf.getvalue("data.tr_first", tr_first, 0);
            
            int halo_method;
            _conf.getvalue("data.halo_method", halo_method, 0);
            if (halo_method == 0) init_square_halo();
            else                  init_hexa_halo();
            
            rmode.camtype = 1;
            rmode.objtype = 1;
            rmode.colourmode = 2;
            pan(-1.50f, 0.0f);
            transform();
            gsgl::updateGPU(grid_tr, halo);
        }
        
        virtual ~plt_scene()
        {
            glDeleteBuffers(1, &(halo.tri_buf));
            glDeleteBuffers(1, &(halo.lin_buf));
        }
        
        virtual void step(float /*dt*/) {}
        
        virtual void save() {}
        
        virtual void init_render() {}
        virtual void pre_render() {}
        virtual void render()
        {
            cr::mat4 model = cr::scale_mat(scale);
            //model *= cr::rotx_mat(rotation);
            //model *= cr::roty_mat(axis);
            
            framebuf->use();
            
            rrr->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
            rrr->pre_render();
            rrr->render(*(cameras[used_cam]), model, halo);

            framebuf->render();
        }
        
        void pan(float x, float y)
        {
            for (unsigned int i=0 ; i<linn*2 ; ++i)
            {
                grid.lines[i*6]     += x;
                grid.lines[i*6 + 1] += y;
            }
            
            for (unsigned int i=0 ; i<trin*3 ; ++i)
            {
                grid.triangles[i*9]     += x;
                grid.triangles[i*9 + 1] += y;
            }
        }
        
        void transform()
        {
            for (unsigned int i=0 ; i<linn*2 ; ++i)
            {
                cr::vec3 v_in(
                    grid.lines[i*6],
                    grid.lines[i*6 + 1],
                    grid.lines[i*6 + 2]
                );
                
                //cr::vec3 v_out = v_in;
                
                //cr::vec3 v_out = TR::tr(v_in, p, tr_first);
                //cr::vec3 v_out = TR::tr(v_in, 6.0f, 8);
                //cr::vec3 v_out = TR::tr08(v_in);
                //v_out = TR::tr(v_out, 2.0f, 8);
                
                //cr::vec3 v_out = TR::tr09(v_in, 0.0f);
                
                //cr::vec3 v_out = ( v_in + TR::tr08( v_in, -1.0f) ) / 2.0f;
                cr::vec3 v0 = v_in; v0.x += 1.0f;
                cr::vec3 v1 = v_in; v1.x -= 10.0f;
                cr::vec3 v2 = cr::vec3{v0.x*v1.x - v0.y*v1.y, v0.x*v1.y + v0.y*v1.y, v_in.z};
                cr::vec3 v_out = TR::tr08( v2, 1.5f);
                //cr::vec3 v4 = v1 + v2 / 2.0f;
                //cr::vec3 v_out;
                //v_out.x = (v3.x*v4.x + v3.y*v4.y) / (v4.x*v4.x + v4.y*v4.y);
                //v_out.y = (v3.y*v4.x - v3.x*v4.y) / (v4.x*v4.x + v4.y*v4.y);
                v_out.z = v_in.z;
                
                
                grid_tr.lines[i*6]     = v_out.x;
                grid_tr.lines[i*6 + 1] = v_out.y;
                grid_tr.lines[i*6 + 2] = v_out.z;
            }
            
            for (unsigned int i=0 ; i<trin*3 ; ++i)
            {
                cr::vec3 v_in(
                    grid.triangles[i*9],
                    grid.triangles[i*9 + 1],
                    grid.triangles[i*9 + 2]
                );
                
                //cr::vec3 v_out = TR::tr(v_in, p, tr_first);
                //cr::vec3 v_out = TR::tr(v_in, 6.0f, 8);
                //cr::vec3 v_out = TR::tr08(v_in);
                //v_out = TR::tr(v_out, 2.0f, 8);
                
                cr::vec3 v1 = TR::tr09( v_in, 0.0f);
                cr::vec3 v2 = TR::tr09(-v_in, 0.0f);
                cr::vec3 v_out = v1 + v2 / 2.0f;
                v_out.z = v_in.z;
                
                grid_tr.triangles[i*9]     = v_out.x;
                grid_tr.triangles[i*9 + 1] = v_out.y;
                grid_tr.triangles[i*9 + 2] = v_out.z;
            }
        }
        
        void update () { gsgl::updateGPU(grid_tr, halo); }
        
        /*
        void relax_z ()
        {
            float perturb_eps = 0.0001f;
            float force_eps   = 0.0001f;
            float dforce_eps  = 0.00001f;
            float displ       = 0.05f;
            
            for (int y=0 ; y <= ydiv ; ++y)
            {
                for (int x=0 ; x <= xdiv ; ++x)
                {
                    if (x == xdiv/2 && y == ydiv/2) continue;
                    
                    float springlen = relax_values[y*(xdiv+1) + x];
                    
                    cr::vec3 v0     ( grid[(y*(xdiv+1) + x)*3],     grid[(y*(xdiv+1) + x)*3 + 1],     grid[(y*(xdiv+1) + x)*3 + 2] );
                    cr::vec3 v0_dup ( grid[(y*(xdiv+1) + x)*3],     grid[(y*(xdiv+1) + x)*3 + 1],     grid[(y*(xdiv+1) + x)*3 + 2] + perturb_eps );
                    
                    //int n = 0;
                    float forceup = 0.0f;
                    float dforce  = 0.0f;
                    
                    if (x>0)
                    {
                        cr::vec3 v1 ( grid[(y*(xdiv+1) + x-1)*3],   grid[(y*(xdiv+1) + x-1)*3 + 1],   grid[(y*(xdiv+1) + x-1)*3 + 2] );
                        cr::vec3 vf = (v1 - v0).normal();
                        forceup += ( vf * ((v1-v0).length() - springlen) ).z;
                        cr::vec3 vfd = (v1 - v0_dup).normal();
                        dforce += ( vfd * ((v1-v0_dup).length() - springlen) ).z;
                        //++n;
                    }
                    if (x<xdiv)
                    {
                        cr::vec3 v2 ( grid[(y*(xdiv+1) + x+1)*3],   grid[(y*(xdiv+1) + x+1)*3 + 1],   grid[(y*(xdiv+1) + x+1)*3 + 2] );
                        cr::vec3 vf = (v2 - v0).normal();
                        forceup += ( vf * ((v2-v0).length() - springlen) ).z;
                        cr::vec3 vfd = (v2 - v0_dup).normal();
                        dforce += ( vfd * ((v2-v0_dup).length() - springlen) ).z;
                        //++n;
                    }
                    if (y>0)
                    {
                        cr::vec3 v3 ( grid[((y-1)*(xdiv+1) + x)*3], grid[((y-1)*(xdiv+1) + x)*3 + 1], grid[((y-1)*(xdiv+1) + x)*3 + 2] );
                        cr::vec3 vf = (v3 - v0).normal();
                        forceup += ( vf * ((v3-v0).length() - springlen) ).z;
                        cr::vec3 vfd = (v3 - v0_dup).normal();
                        dforce += ( vfd * ((v3-v0_dup).length() - springlen) ).z;
                        //++n;
                    }
                    if (y<ydiv)
                    {
                        cr::vec3 v4 ( grid[((y+1)*(xdiv+1) + x)*3], grid[((y+1)*(xdiv+1) + x)*3 + 1], grid[((y+1)*(xdiv+1) + x)*3 + 2] );
                        cr::vec3 vf = (v4 - v0).normal();
                        forceup += ( vf * ((v4-v0).length() - springlen) ).z;
                        cr::vec3 vfd = (v4 - v0_dup).normal();
                        dforce += ( vfd * ((v4-v0_dup).length() - springlen) ).z;
                        //++n;
                    }
                    
                    if (fabs(forceup) > force_eps)
                    {
                        grid[(y*(xdiv+1) + x)*3 + 2] += forceup * displ;
                    }
                    else if (fabs(dforce) > dforce_eps)
                    {
                        grid[(y*(xdiv+1) + x)*3 + 2] += dforce * displ;
                    }
                }
            }
        }
        */
        
        
        /*
        void relax_z ()
        {
            float dz = 0.002f;
            int tries2 = 10;
            
            for (int y=ydiv-1 ; y>=0 ; --y)
            {
                for (int x=1 ; x<=xdiv ; ++x)
                {
                    cr::vec3 v0 ( grid[(y*(xdiv+1) + x-1)*3],     grid[(y*(xdiv+1) + x-1)*3 + 1],     grid[(y*(xdiv+1) + x-1)*3 + 2] );
                    cr::vec3 v1 ( grid[(y*(xdiv+1) + x)*3],       grid[(y*(xdiv+1) + x)*3 + 1],       grid[(y*(xdiv+1) + x)*3 + 2] );
                    cr::vec3 v2 ( grid[((y+1)*(xdiv+1) + x-1)*3], grid[((y+1)*(xdiv+1) + x-1)*3 + 1], grid[((y+1)*(xdiv+1) + x-1)*3 + 2] );
                    
                    float zavg = (x < xdiv) ? (grid[((y+1)*(xdiv+1) + x+1)*3 + 2] + v2.z + v1.z) / 3.0f : (v2.z + v1.z) / 2.0f;
                    
                    float len2 = (v2-v0).length();
                    float len1 = (v1-v0).length();
                    if (len2 < 0.000001f || len1 < 0.000001f) continue;
                    float a = std::acos(((v1-v0)*(v2-v0)) / (len1*len2)) - PIDEF/2.0f;
                    float znew = v1.z;
                    
                    cr::vec3 v3 = v1;
                    v3.z = zavg + dz * (-tries2);
                    for (int i=-tries2 ; i<=tries2 ; ++i)
                    {
                        float len3 = (v3-v0).length();
                        if (len3 < 0.000001f) { v3.z += dz; continue; }
                        float a3 = std::acos(((v3-v0)*(v2-v0)) / (len3*len2)) - PIDEF/2.0f;
                        if (fabs(a3) < fabs(a))
                        {
                            znew = v3.z;
                        }
                        v3.z += dz;
                    }
                    grid[(y*(xdiv+1) + x)*3 + 2] = znew;
                }
            }
        }
        */
        
        void startrot()
        {
            if ((axis > 90.0f) && (axis < 270.0f)) {
                rotdir = false;
            } else {
                rotdir = true;
            }
        }
        
        virtual void mousescroll (float /*x*/, float y)
        {
            //float sw_base = 1.25f;
            //float sw = (y > 0.0f) ? (sw_base) * y : (-1.0f / sw_base) * y;
            //float s2 = scale * sw;
            float s2 = scale * std::pow(1.25f,y);
            
            scale = s2;
            /*
            if (s2 > 0.000001f && s2 < 1000.0f)
            {
                scale = s2;
            }
            */
        }

        virtual void mousebutton (int /*key*/, int action, int /*mods*/)
        {
            if (action == keys::PRESS) startrot();
        }
        
        virtual void mousedrag (float /*xp*/, float /*yp*/, float x, float y, int btn)
        {
            if (btn == keys::MOUSE_M)
            {
                if (rotdir)
                {
                    rotation += y;
                }
                else
                {
                    rotation -= y;
                }
                axis -= x;
        
                // Ensure [0,360]
                axis = axis - std::floor(axis/360.0f)*360.0f;
                rotation = rotation - std::floor(rotation/360.0f)*360.0f;
            }
            else if (btn == keys::MOUSE_L)
            {
                pan(-0.02f*x, -0.02f*y);
                transform();
                gsgl::updateGPU(grid_tr, halo);
            }
        }
        
        virtual void keyaction (int key, int action, int /*mods*/)
        {
            //int nump   = 50;
            //float dpan = -3.0f / nump;
            
            if (action == keys::RELEASE) return;
            switch (key)
            {
                case keys::I :
                    ++(rmode.camtype); if (rmode.camtype >= cr::render_mode::cam_n) rmode.camtype=0;
                    break;
                    
                case keys::O :
                    ++(rmode.objtype); if (rmode.objtype >= cr::render_mode::obj_n) rmode.objtype=0;
                    break;
                
                case keys::P :
                    p += dp;
                    std::cout << "P " << p << std::endl;
                    transform();
                    gsgl::updateGPU(grid_tr, halo);
                    break;
            
                case keys::K :
                    ++(rmode.colourmode); if (rmode.colourmode >= cr::render_mode::col_n) rmode.colourmode=0;
                    break;
                
                case keys::W :
                    /*
                    for (int i=0 ; i<nump ; ++i)
                    {
                        pan(dpan, 0.0f);
                        transform();
                        r::updateGPU(gridgeom_tr, halo);
                        global_loop->one_loop();
                        global_loop->key(GLFW_KEY_F8, 0, GLFW_PRESS, 0);
                        
                    }
                    */
                    break;
                
                case keys::X :
                    pan(0.01f, 0.0f);
                    transform();
                    gsgl::updateGPU(grid_tr, halo);
                    break;
                    
                case keys::Y :
                    pan(0.0f, 0.01f);
                    transform();
                    gsgl::updateGPU(grid_tr, halo);
                    break;
                
                case keys::R :
                    //relax_z();
                    //update_gpu();
                    break;
                
                case keys::T :
                    ++steps;
                    tmix = start + (ds * steps);
                    std::cout << "T " << tmix << std::endl;
                    transform();
                    gsgl::updateGPU(grid_tr, halo);
                    break;
                default:
                    break;
            }
        }
};

int main ()
{
    cr::scripter ttconf("plane_transform.conf");
    
    global_loop = new glfw_loop(ttconf);
    scene* s = new plt_scene(ttconf);
    global_loop->set_scene(s);
    
    
    glfwSetKeyCallback        (global_loop->window, key_callback);
    glfwSetScrollCallback     (global_loop->window, scroll_callback);
    glfwSetCursorPosCallback  (global_loop->window, cursorpos_callback);
    glfwSetMouseButtonCallback(global_loop->window, mousebutton_callback);
    
    
    
    int ret = global_loop->loop();
    global_loop->done();
    
    delete s;
    delete global_loop;
    return ret;
}
