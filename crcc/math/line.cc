
#include "line.hh"


namespace cr {


line2::line2 () { P.nill(); I.nill(); }
line2::line2 (vec2 PP, vec2 II) { P = PP; I = II.normal(); }
        
void line2::set2p(vec2 P1, vec2 P2)
{
    P = P1; I = (P2-P1).normal();
}
        
float line2::distance (vec2 Q) const
{
    return (P + I*t(Q) - Q).length();
}
float line2::side (vec2 Q) const
{
    return dot( I.left(), (Q-P) );
}
float line2::t (vec2 Q) const
{
    return dot( I, (Q - P) );
}
vec2 line2::tangent (vec2 Q) const
{
    return (P + I*t(Q));
}




line3::line3 () { P.nill(); I.nill(); }
line3::line3 (vec3 PP, vec3 II) { P = PP; I = II.normal(); }
        
void line3::set2p(vec3 P1, vec3 P2)
{
    P = P1; I = (P2-P1).normal();
}
        
float line3::distance (vec3 Q) const
{
    return (cross(I, (Q - P))).length();
}
        
float line3::t (vec3 Q) const
{
    return dot( I, (Q - P) );
}
vec3 line3::tangent (vec3 Q) const
{
    return (P + I*t(Q));
}


}
