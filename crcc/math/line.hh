#pragma once

#include "utils.hh"
#include "vec.hh"


namespace cr {

class line2
{
    public:
        vec2 P;
        vec2 I;
        
    public:
        line2 ();
        line2 (vec2 PP, vec2 II);
        
        void set2p (vec2 P1, vec2 P2);
        
        float distance (vec2 Q) const;
        float side     (vec2 Q) const;
        float t        (vec2 Q) const;
        vec2  tangent  (vec2 Q) const;
};
    
class line3
{
    public:
        vec3 P;
        vec3 I;
        
    public:
        line3 ();
        line3 (vec3 PP, vec3 II);
        
        void set2p (vec3 P1, vec3 P2);
        
        float distance (vec3 Q) const;
        float t        (vec3 Q) const;
        vec3  tangent  (vec3 Q) const;
};

}
