#pragma once
/*
#include "utils.hh"
#include "vec.hh"
#include "line.hh"


namespace cr {

class iplane
{
    public:
        virtual void set (vec3 P, vec3 N) = 0;
        virtual void set (vec3 P0, vec3 P1, vec3 P2) = 0;
        virtual void set (float _a, float _b, float _c, float _d) = 0;
    
        virtual vec3 normal () const = 0;
    
        virtual float distance (vec3 Q) const = 0;
        virtual vec3  tangent  (vec3 Q) const = 0;
    
        virtual bool validate () const = 0;
};

class plane_eq : public iplane
{
    public:
        // ax + by + cz + d = 0
        float a,b,c,d;
        
    public:
        plane_eq () : a(0), b(0), c(0), d(0) {}
        plane_eq (vec3 P, vec3 N) { set(P,N); }
        plane_eq (vec3 P0, vec3 P1, vec3 P2) { set(P0,P1,P2); }
        plane_eq (float aa, float bb, float cc, float dd) { set(aa,bb,cc,dd); }
        
        virtual void set (vec3 P, vec3 N)
        {
            vec3 NN = N.normal();
            a = NN.x; b = NN.y; c = NN.z;
            d = -(P.x*NN.x + P.y*NN.y + P.z*NN.z);
        }
        virtual void set (vec3 P0, vec3 P1, vec3 P2)
        {
            set(P0, (P1-P0) % (P2-P0));
        }
        virtual void set (float aa, float bb, float cc, float dd)
        {
            // TODO normalize
            a = aa; b = bb; c = cc; d = dd;
        }
        
        inline virtual vec3 normal() const
        {
            return vec3(a,b,c);
        }
        
        inline virtual float distance (vec3 Q) const
        {
            return (a*Q.x + b*Q.y + c*Q.z + d);
        }
        
        inline virtual vec3 tangent (vec3 Q) const
        {
            float alfa = Q.x*(a*a - a) + Q.y*(b*b - b) + Q.z*(c*c - c);
            return (Q + normal()*alfa);
        }
        
        inline virtual bool validate () const
        {
            return zero(a*a+b*b+c*c - 1.0f) ? false : true;
        }
};

class plane_PN : public iplane {
    private:
        vec3 P;
        vec3 N;
        
    public:
        Plane_PN () { P.nill(); N.nill(); }
        Plane_PN (vec3 PP, vec3 NN) { set(PP,NN); }
        Plane_PN (vec3 P0, vec3 P1, vec3 P2) { set(P0,P1,P2); }
        Plane_PN (float a, float b, float c, float d) { set(a,b,c,d); }
        
        void set (vec3 PP, vec3 NN) { P = PP; N = normal(NN); }
        void set (vec3 P0, vec3 P1, vec3 P2)
        {
            set((P1-P0) % (P2-P0), P0);
        }
        void set (float a, float b, float c, float d)
        {
            set(a,b,c,d,vec3(0,0,0));
        }
        void set (float a, float b, float c, float d, vec3 R) {
            N.set(a,b,c);
            N.normalize();
            alfa = R.x*(a*a-a) + R.y*(b*b-b) + R.z*(c*c-c);
            //alfa /= (a*a + b*b + c*c);
            P = R + N*alfa;
        }
        
        //void abcd (float& a, float& b, float& c, float& d);
        
        bool validate() {
            if (zero(N.x) && zero(N.y) && zero(N.z)) return false;
            return true;
        }
};

typedef plane_PN plane;

}
*/
