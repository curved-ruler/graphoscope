
#include "delaunay.hh"

namespace cr {

float incircle (vec2 a, vec2 b, vec2 c, vec2 d)
{
    mat4 m = {
        a.x, a.y, a.x*a.x+a.y*a.y, 1,
        b.x, b.y, b.x*b.x+b.y*b.y, 1,
        c.x, c.y, c.x*c.x+c.y*c.y, 1,
        d.x, d.y, d.x*d.x+d.y*d.y, 1,
    };
    return m.det();
}

float tri_s_area(vec2 a, vec2 b, vec2 c)
{
    return ( a.x*(b.y-c.y) + b.x*(c.y-a.y) + c.x*(a.y-b.y) ) / 2.0f;
}

vec3 barycentric (vec2 a, vec2 b, vec2 c, vec2 p)
{
    float ar  = tri_s_area(a,b,c);
    float ar1 = tri_s_area(p,b,c);
    float ar2 = tri_s_area(p,c,a);
    
    float ac = ar1 / ar;
    float bc = ar2 / ar;
    return vec3(ac, bc, 1.0f - (ac+bc));
}

bool tri_inside (vec2 a, vec2 b, vec2 c, vec2 p)
{
    vec3 coords = barycentric(a,b,c,p);
    
    if (coords.x < 0.0f || coords.x > 1.0f) return false;
    if (coords.y < 0.0f || coords.y > 1.0f) return false;
    if (coords.z < 0.0f || coords.z > 1.0f) return false;
    
    return true;
}






void delaunay_maker::checkflip (size_t i, int a, int b)
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
    
    if ( tri_inside(p[ t[i].x ], p[ t[i].y ], p[ t[i].z ], p[c]) ) return;
    if ( tri_inside(p[ t[i].x ], p[ t[i].y ], p[c], p[ t[i].z ]) ) return;
    if ( tri_inside(p[ t[i].x ], p[c], p[ t[i].z ], p[ t[i].y ]) ) return;
    if ( tri_inside(p[c], p[ t[i].z ], p[ t[i].y ], p[ t[i].x ]) ) return;
    
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
    det = anticlock ? incircle(p[ t[i].x ], p[ t[i].y ], p[ t[i].z ], p[c]) :
                      incircle(p[ t[i].x ], p[ t[i].z ], p[ t[i].y ], p[c]) ;
    
    if ( det > 0.0f )
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


void delaunay_step(delaunay_maker& m, const std::vector<vec2>& pts, size_t pti)
{
    if ( pti < pts.size() )
    {
        m.p.push_back(pts[pti]);
        int a = 0, b = 1, c = 2;
        
        size_t ts = m.t.size();
        int tcont = -1;
        for (size_t ti=0 ; ti<ts ; ++ti)
        {
            vec2 va = m.p[ m.t[ti].x ];
            vec2 vb = m.p[ m.t[ti].y ];
            vec2 vc = m.p[ m.t[ti].z ];
            
            if ( tri_inside(va, vb, vc, pts[pti]) )
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
            std::cout << "ERROR: Delaunay: container tirangle not found " << pti << std::endl;
            return;
        }
        
        m.t[tcont].x = pti+3;
        m.t[tcont].y = a;
        m.t[tcont].z = b;
        m.t.push_back( ivec3(pti+3, a, c) );
        m.t.push_back( ivec3(pti+3, b, c) );
        
        m.checkflip(tcont, a, b);
        m.checkflip(ts,    a, c);
        m.checkflip(ts+1,  b, c);
    }
}



void delaunay (const std::vector<vec2>& pts, std::vector<int>& out)
{
    if (pts.size() < 3) return;
    
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
    
    delaunay_maker m;
    
    m.p.push_back( vec2(  xmin-3,      ymin-3) );
    m.p.push_back( vec2(2*xmax-xmin+3, ymin-3) );
    m.p.push_back( vec2(  xmin-3,    2*ymax-ymin+3) );
    
    m.t.push_back( ivec3(0,1,2) );
    
    
    
    for (size_t i=0 ; i<pts.size() ; ++i)
    {
        delaunay_step(m, pts, i);
    }
    
    
    
    for (size_t i=0 ; i<m.t.size() ; ++i)
    {
        if (m.t[i].x > 2 && m.t[i].y > 2 && m.t[i].z > 2)
        {
            out.push_back(m.t[i].x - 3);
            out.push_back(m.t[i].y - 3);
            out.push_back(m.t[i].z - 3);
        }
    }
}

}
