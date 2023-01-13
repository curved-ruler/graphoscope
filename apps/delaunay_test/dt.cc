
#include <vector>
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

using namespace cr;

float incircle3 (vec2 a, vec2 b, vec2 c, vec2 d)
{
    mat4 m = {
        a.x, a.y, a.x*a.x+a.y*a.y, 1.0f,
        b.x, b.y, b.x*b.x+b.y*b.y, 1.0f,
        c.x, c.y, c.x*c.x+c.y*c.y, 1.0f,
        d.x, d.y, d.x*d.x+d.y*d.y, 1.0f,
    };
    return m.det();
}

float area2 (vec2 a, vec2 b, vec2 c)
{
    return cr::abs( a.x*(b.y-c.y) + b.x*(c.y-a.y) + c.x*(a.y-b.y) ) / 2.0f;
}
float signed_area (vec2 a, vec2 b, vec2 c)
{
    return ( a.x*(b.y-c.y) + b.x*(c.y-a.y) + c.x*(a.y-b.y) ) / 2.0f;
}

bool inside1 (vec2 aa, vec2 bb, vec2 cc, vec2 pp)
{
    vec3 a(aa.x, aa.y, 0.0f);
    vec3 b(bb.x, bb.y, 0.0f);
    vec3 c(cc.x, cc.y, 0.0f);
    vec3 p(pp.x, pp.y, 0.0f);
    
    if (cr::dot(cr::cross(b-a,p-a), cr::cross(b-a,c-a)) < 0.0f) return false;
    if (cr::dot(cr::cross(c-b,p-b), cr::cross(c-b,a-b)) < 0.0f) return false;
    if (cr::dot(cr::cross(a-c,p-c), cr::cross(a-c,b-c)) < 0.0f) return false;
    
    return true;
}

bool inside2 (vec2 a, vec2 b, vec2 c, vec2 p)
{
    float ar  = area2(a,b,c);
    float ar1 = area2(a,b, p);
    float ar2 = area2(a,c, p);
    float ar3 = area2(b,c, p);
    return cr::abs( ar - (ar1+ar2+ar3) ) < 0.0001f;
}

bool inside3 (vec2 a, vec2 b, vec2 c, vec2 p)
{
    float ar  = signed_area(a,b,c);
    float ar1 = signed_area(p,b,c);
    float ar2 = signed_area(p,c,a);
    
    float aaa = ar1 / ar;
    if (aaa < 0.0f || aaa > 1.0f) return false;
    
    float bbb = ar2 / ar;
    if (bbb < 0.0f || bbb > 1.0f) return false;
    
    float ccc = 1.0f - (aaa+bbb);
    if (ccc < 0.0f || ccc > 1.0f) return false;
    
    return true;
}

struct delaunay_maker2
{
    std::vector<vec2>  p;
    std::vector<ivec3> t;
    
    // check ab edge of the i.th triangle
    void checkflip (size_t i, int a, int b)
    {
        if (a<3 && b<3)  return;
        if (i>=t.size()) return;
        
        /*
        int chk0 = 0;
        if (t[i].x == a || t[i].y == a || t[i].z == a) ++chk0;
        if (t[i].x == b || t[i].y == b || t[i].z == b) ++chk0;
        if (chk0 != 2) { std::cout << "ERROR: Delaunay: checkflip - wrong call" << std::endl; return; }
        */
        
        size_t i2 = i;
        int c = -1;
        for (size_t ti=0 ; ti<t.size() ; ++ti)
        {
            if (ti == i) continue;
            
            int chk = 0;
            if (t[ti].x == a || t[ti].y == a || t[ti].z == a) ++chk;
            if (t[ti].x == b || t[ti].y == b || t[ti].z == b) ++chk;
            if (chk == 2)
            {
                if (t[ti].x != a && t[ti].x != b) c = t[ti].x;
                if (t[ti].y != a && t[ti].y != b) c = t[ti].y;
                if (t[ti].z != a && t[ti].z != b) c = t[ti].z;
                i2 = ti;
                break;
            }
        }
        
        if (c == -1)
        {
            std::cout << "ERROR: Delaunay: checkflip - adjacent tirangle not found" << std::endl;
            return;
        }
        
        if ( inside2(p[ t[i].x ], p[ t[i].y ], p[ t[i].z ], p[c]) ) return;
        if ( inside2(p[ t[i].x ], p[ t[i].y ], p[c], p[ t[i].z ]) ) return;
        if ( inside2(p[ t[i].x ], p[c], p[ t[i].z ], p[ t[i].y ]) ) return;
        if ( inside2(p[c], p[ t[i].z ], p[ t[i].y ], p[ t[i].x ]) ) return;
        
        int d = -1;
        if (t[i].x != a && t[i].x != b) d = t[i].x;
        if (t[i].y != a && t[i].y != b) d = t[i].y;
        if (t[i].z != a && t[i].z != b) d = t[i].z;
        
        if (d == c) { std::cout << "d = c  " << i << " / "<< i2 << std::endl; return;  }
        
        if (a>2 && b>2 && d>2 && c<3) return;
        
        float det;
        bool  anticlock;
        
        vec3 aa(p[t[i].x].x, p[t[i].x].y, 1.0f);
        vec3 bb(p[t[i].y].x, p[t[i].y].y, 1.0f);
        vec3 cc(p[t[i].z].x, p[t[i].z].y, 1.0f);
        vec3 ba = bb-aa;
        vec3 ca = cc-aa;
        vec3 z = cross(ba, ca);
        anticlock = z.z > 0.0f;
        det = anticlock ? incircle3(p[ t[i].x ], p[ t[i].y ], p[ t[i].z ], p[c]) :
                          incircle3(p[ t[i].x ], p[ t[i].z ], p[ t[i].y ], p[c]) ;
        
        if ( det > 0.0f )
        //if ( incircle3(p[ t[i].x ], p[ t[i].y ], p[ t[i].z ], p[c]) > 0.001f )
        {
            t[i].x = a;
            t[i].y = d;
            t[i].z = c;
            
            t[i2].x = b;
            t[i2].y = c;
            t[i2].z = d;
            
            checkflip(i,  a, c);
            checkflip(i2, b, c);
        }
    }
};


class plt_scene : public scene
{
    private:
        std::string target_dir;
        
        float minmax = 1.0f;
        int N = 72;
        //int SC = 3000;
        std::vector<cr::vec2>   pts;
        
        std::vector<cr::vec2>   pts2;
        int Ni = 0;
        delaunay_maker2 m;
        
        //std::vector<cr::ivec2> ipts;
        std::vector<int> tris;
        
        cr::mesh_ux del;
        cr::rrr_buffers del_gpu;
        
        gsgl::r_nshaded* rrr;
        //gsgl::r_vshaded* rrr_v;
        
        cr::material col;
        
        float scale;
        float axis;
        float rotation;
        bool  rotdir;
        float xd, yd;
        
        
void delaunay2 (std::vector<int>& out, int nni)
{
    
    
    
    //for (size_t i=0 ; i<pts.size() ; ++i)
    //{
        m.p.push_back(pts[nni]);
        int a = 0, b = 1, c = 2;
        
        size_t ts = m.t.size();
        int tcont = -1;
        for (size_t ti=0 ; ti<ts ; ++ti)
        {
            vec2 va = m.p[ m.t[ti].x ];
            vec2 vb = m.p[ m.t[ti].y ];
            vec2 vc = m.p[ m.t[ti].z ];
            
            //std::cout << cr::abs( inside2(va, vb, vc, pts[i]) ) << std::endl;
            
            if ( inside2(va, vb, vc, pts[nni]) )
            {
                a = m.t[ti].x;
                b = m.t[ti].y;
                c = m.t[ti].z;
                tcont = ti;
                break;
            }
        }
        
        if (tcont == -1)
        {
            std::cout << "ERROR: Delaunay: container tirangle not found " << nni << std::endl;
            return;
        }
        
        m.t[tcont].x = nni+3;
        m.t[tcont].y = a;
        m.t[tcont].z = b;
        m.t.push_back( ivec3(nni+3, a, c) );
        m.t.push_back( ivec3(nni+3, b, c) );
        
        m.checkflip(tcont, a, b);
        m.checkflip(ts,    a, c);
        m.checkflip(ts+1,  b, c);
    //}
    
    out.clear();
    for (size_t i=0 ; i<m.t.size() ; ++i)
    {
        //if (m.t[i].x > 2 && m.t[i].y > 2 && m.t[i].z > 2)
        //{
            out.push_back(m.t[i].x);
            out.push_back(m.t[i].y);
            out.push_back(m.t[i].z);
        //}
    }
}

        
        void init_halo()
        {
            /*
            cr::fdice dice(120, -minmax, minmax);
            for (int i=0 ; i<N ; ++i)
            {
                cr::vec2 p;
                p.x = dice.next();
                p.y = dice.next();
                pts.push_back(p);
            }
            */
            
            for (int i=0 ; i<N ; ++i)
            {
                cr::vec2 p;
                p.x = 3.0f * std::cos(i*(360.0f/float(N))*cr::dtor);
                p.y = 3.0f * std::sin(i*(360.0f/float(N))*cr::dtor);
                pts.push_back(p);
            }
            
            
            
    float xmin = pts[0].x;
    float xmax = pts[0].x;
    float ymin = pts[0].y;
    float ymax = pts[0].y;
    for (size_t i=1 ; i<pts.size() ; ++i)
    {
        if (pts[i].x > xmax) xmax = pts[i].x;
        if (pts[i].x < xmin) xmin = pts[i].x;
        
        if (pts[i].y > ymax) ymax = pts[i].y;
        if (pts[i].y < ymin) ymin = pts[i].y;
    }
    
    cr::vec2 x(  xmin-7,      ymin-7);
    cr::vec2 y(2*xmax-xmin+7, ymin-7);
    cr::vec2 z(  xmin-7,    2*ymax-ymin+7);
    
    pts2.push_back(x);
    pts2.push_back(y);
    pts2.push_back(z);
    
    
    m.p.push_back(cr::vec2(pts2[0].x, pts2[0].y));
    m.p.push_back(cr::vec2(pts2[1].x, pts2[1].y));
    m.p.push_back(cr::vec2(pts2[2].x, pts2[2].y));
    m.t.push_back( ivec3(0,1,2) );
    
    for (size_t i=0 ; i<pts.size() ; ++i)
    {
        pts2.push_back(pts[i]);
    }
    /*
    for (size_t i=0 ; i<pts2.size() ; ++i)
    {
        cr::ivec2 ip( pts2[i].x*SC, pts2[i].y*SC);
        ipts.push_back(ip);
    }
    */
        }

        void updategpu()
        {
            del.clear();
            
            for (size_t i=0 ; i<pts2.size() ; ++i)
            {
                col.albedo.x = 1.0f;
                col.albedo.y = 0.0f;
                col.albedo.z = 0.0f;
                cr::vec3 a( pts2[i].x,   pts2[i].y,   0.0f );
                del.add_pnt(a,col);
            }
            
            for (size_t i=0 ; i<tris.size()/3 ; ++i)
            {
                cr::vec3 a( pts2[tris[i*3]].x,   pts2[tris[i*3]].y,   0.0f );
                cr::vec3 b( pts2[tris[i*3+1]].x, pts2[tris[i*3+1]].y, 0.0f );
                cr::vec3 c( pts2[tris[i*3+2]].x, pts2[tris[i*3+2]].y, 0.0f );
                
                col.albedo.x = 1.0f;
                col.albedo.y = 1.0f;
                col.albedo.z = 1.0f;
                del.add_tri(a,b,c,col);
                //std::cout << tris[i*3] << "     " << tris[i*3+1] << "     " << tris[i*3+2] << std::endl;
                
                col.albedo.x = 0.0f;
                col.albedo.y = 1.0f;
                col.albedo.z = 0.0f;
                del.add_lin(a,b,col);
                del.add_lin(b,c,col);
                del.add_lin(c,a,col);
            }
            
            gsgl::MUXtoGPU(del, del_gpu);
        }
        
        void del_step()
        {
            if (Ni < N)
            {
                delaunay2(tris, Ni);
                updategpu();
                ++Ni;
            }
        }
        
    public:
        plt_scene(cr::scripter& _conf) : scene(_conf)
        {
            _conf.getvalue("folders.target", target_dir);
            
            cr::camera* cam0 = new cr::camera();
            cam0->near = 10.0f;
            cam0->far = 5000.0f;
            cam0->focal = 100.0f;
            cam0->pos  = cr::vec3(0.0f, 0.0f,  100.0f);
            cam0->look = cr::vec3(0.0f, 0.0f, -1.0f);
            cam0->up   = cr::vec3(0.0f, 1.0f,  0.0f);
            cam0->oheight2 = 2.0f;
            cam0->pheight2 = 2.0f;
            cam0->aspect = 1.0f;
    
            cameras.push_back(cam0);
            
            rrr = new gsgl::r_nshaded();
            rrr->setup(&rmode);
            //rrr_v = new gsgl::r_vshaded();
            //rrr_v->setup(&rmode);
            
            
            rmode.back[0] = 0.0f;
            rmode.back[1] = 0.0f;
            rmode.back[2] = 0.0f;
            rmode.back[3] = 1.0f;
            //sysconf.getarray("window.backgr", rmode.back, 3);
            
            scale = 1.0f;
            xd = 0.0f;
            yd = 0.0f;
            
            init_halo();
            del_step();
            
            //pan(-1.50f, 0.0f);
        }
        
        virtual ~plt_scene()
        {
            delete rrr;
            //delete rrr_v;
        }
        
        virtual void step(float /*dt*/) {}
        
        virtual void save() {}
        
        virtual void init_render() {}
        virtual void pre_render() {}
        
        virtual void render()
        {
            cr::mat4 model = cr::scale_mat(scale);
            
            model *= cr::move_mat(cr::vec3(xd,yd,0));
            //model *= cr::rotx_mat(rotation);
            //model *= cr::roty_mat(axis);
            
            framebuf->use();
            
            rrr->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
            rrr->pre_render();
            
            rmode.camtype = 1;
            rmode.colourmode = 2;
            rmode.has_normal = false;
            
            rmode.objtype = 1;
            rrr->render(*(cameras[used_cam]), model, del_gpu);
            
            rmode.objtype = 0;
            rmode.pointsize = 5;
            rrr->render(*(cameras[used_cam]), model, del_gpu);
            
            rmode.objtype = 2;
            rmode.colourmode = 3;
            rmode.has_normal = true;
            //rrr_v->init_render(rmode.screen_w / rmode.pixel_size, rmode.screen_h / rmode.pixel_size);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(1, 1);
            rrr->render(*(cameras[used_cam]), model, del_gpu);
            glDisable(GL_POLYGON_OFFSET_FILL);

            framebuf->render();
        }
        
        void pan(float x, float y)
        {
            xd += x / scale;
            yd += y / scale;
        }
        
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
                pan(-0.01f*x, -0.01f*y);
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
                    //++(rmode.camtype); if (rmode.camtype >= cr::render_mode::cam_n) rmode.camtype=0;
                    break;
                    
                case keys::O :
                    //++(rmode.objtype); if (rmode.objtype >= cr::render_mode::obj_n) rmode.objtype=0;
                    break;
                
                case keys::P :
                    break;
            
                case keys::K :
                    //++(rmode.colourmode); if (rmode.colourmode >= cr::render_mode::col_n) rmode.colourmode=0;
                    break;
                
                case keys::X :
                    pan(0.01f, 0.0f);
                    break;
                    
                case keys::Y :
                    pan(0.0f, 0.01f);
                    break;
                    
                case keys::ENTER :
                    del_step();
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
