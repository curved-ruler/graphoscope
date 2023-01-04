#pragma once

#include "../math/vec.hh"

namespace cr
{

enum colmode
{
    CONT_CUT, // Continuous color (0..1, mix), cut if <0 or >1
    CONT_MOD, // Continuous color (0..1, mix), modulo if <0 or >1
    DISC_CUT, // Discrete color (0,1,2,..N), cut if <0 or >N
    DISC_MOD  // Discrete color (0,1,2,..N), modulo if <0 or >N
};

class palette {
    private:
        int size;
        vec3* basecols;
        colmode cm;
        
    private:
        // Continous helper: 0<x<1
        vec3 get01(float x) const;
        
        vec3 get_contcut(float pos) const;
        vec3 get_contmod(float pos) const;
        vec3 get_disccut(float i) const;
        vec3 get_discmod(float i) const;
        
    public:
        palette(int size);
        //palette(const palette& other);
        ~palette();
        
        void    setmode (colmode mode);
        colmode getmode () const;
        
        void setcol (vec3 c, int i);
        vec3 getcol (float pos) const;
};

struct height_pal
{
    int colnum;
    vec3* cols;
    float* limits;
    
    height_pal  (int _n);
    height_pal  (const height_pal& base, int sub);
    ~height_pal ();
    
    unsigned int get(float height);
};

}
