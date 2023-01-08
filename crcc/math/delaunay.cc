
#include "delaunay.hh"

namespace cr {

float incircle(vec2 a, vec2 b, vec2 c, vec2 d)
{
    mat4 m = {
        a.x, a.y, a.x*a.x+a.y*a.y, 1,
        b.x, b.y, b.x*b.x+b.y*b.y, 1,
        c.x, c.y, c.x*c.x+c.y*c.y, 1,
        d.x, d.y, d.x*d.x+d.y*d.y, 1,
    };
    return m.det();
}

float area(vec2 a, vec2 b, vec2 c)
{
    return cr::abs( a.x*(b.y-c.y) + b.x*(c.y-a.y) + c.x*(a.y-b.y)) / 2.0f;
}


float inside (vec2 a, vec2 b, vec2 c, vec2 p)
{
    float ar  = area(a,b,c);
    float ar1 = area(a,b, p);
    float ar2 = area(a,c, p);
    float ar3 = area(b,c, p);
    return ar - (ar1+ar2+ar3);
}

struct delaunay_maker
{
    std::vector<vec2>  p;
    std::vector<ivec3> t;
    
    // check ab edge of the i.th triangle
    void checkflip(size_t i, int a, int b)
    {
        if (a<3 && b<3)  return;
        if (i>=t.size()) return;
        
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
        
        if (incircle(p[ t[i].x ], p[ t[i].y ], p[ t[i].z ], p[c]) > 0.0001)
        {
            int d = -1;
            if (t[i].x != a && t[i].x != b) d = t[i].x;
            if (t[i].y != a && t[i].y != b) d = t[i].y;
            if (t[i].z != a && t[i].z != b) d = t[i].z;
            
            t[i].x = a;
            t[i].y = c;
            t[i].z = d;
            
            t[i2].x = b;
            t[i2].y = c;
            t[i2].z = d;
            
            checkflip(i,  a, c);
            checkflip(i2, b, c);
        }
    }
};

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
    m.p.push_back( vec2(xmin-1, ymin-1) );
    m.p.push_back( vec2((xmax-xmin)*2+1, ymin-1) );
    m.p.push_back( vec2(xmin-1, (ymax-ymin)*2+1) );
    m.t.push_back( ivec3(0,1,2) );
    
    
    
    for (size_t i=0 ; i<pts.size() ; ++i)
    {
        m.p.push_back(pts[i]);
        int a = 0, b = 1, c = 2;
        
        size_t ts = m.t.size();
        for (size_t ti=1; ti<m.t.size() ; ++ti)
        {
            vec2 va = m.p[ m.t[ti].x ];
            vec2 vb = m.p[ m.t[ti].y ];
            vec2 vc = m.p[ m.t[ti].z ];
            if ( inside(va, vb, vc, pts[i]) < -0.0001 )
            {
                a = m.t[ti].x;
                b = m.t[ti].y;
                c = m.t[ti].z;
                break;
            }
        }
        
        m.t.push_back( ivec3(i+3, a, b) );
        m.t.push_back( ivec3(i+3, a, c) );
        m.t.push_back( ivec3(i+3, b, c) );
        
        m.checkflip(ts,   a, b);
        m.checkflip(ts+1, a, c);
        m.checkflip(ts+2, b, c);
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
