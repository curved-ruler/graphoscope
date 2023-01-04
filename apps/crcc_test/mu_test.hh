#pragma once

#include <iostream>
#include <cstdlib>
#include <ctime>

#include "../../crcc/math/mu.hh"


using namespace cr;

void vec_mat_tests(int& sum, int& succ)
{
    int sumv=0, succv=0;

    { // init, constructors, operator=
        vec3 v;
        vec3 v1(1.0f, 2.0f, 3.0f);
        vec3 v2 = v1;
        //vec3 v3 = 10.0f; // kell ez?
        vec3 v4;
        v4.x = 2.1f;
        v4 = v1;
        if (zero(v.x) && zero(v1.y - 2.0f) && zero(v2.z - 3.0f) && zero(v4.x - 1.0f)) {
            ++sumv; ++succv;
        } else {
            std::cout << "Failed: mu::vec3 initialization" << std::endl;
        }
    }
    
    { // basic operators
        vec3 v1(1,2,3);
        vec3 v2(0.5f, 0.5f, 1.0f);
        vec3 v3 = -v1;     vec3 v33(-1.0f, -2.0f, -3.0f);
        vec3 v4 = v1-v2;   vec3 v44(0.5f, 1.5f, 2.0f);
        vec3 v5 = v1+v2;   vec3 v55(1.5f, 2.5f, 4.0f);
        vec3 v6 = v2*3.0f; vec3 v66(1.5f, 1.5f, 3.0f);
        if (v3.eq(v33) && v4.eq(v44) && v5.eq(v55) && v6.eq(v66)) {
            ++sumv; ++succv;
        } else {
            std::cout << "Failed: mu::vec3 basic operators" << std::endl;
        }
    }
    
    { // dot and cross product
        vec3 v1;
        vec3 v2;
    }
    
    { // mat4 inverse
        mat4 e;
        mat4 mi1;
        for (int i=0 ; i<16 ; ++i) {
            mi1.data[i] = (rand() % 120) - 60.0f;
        }
        mat4 mi2 = mi1.inverse();
        if ((mi2*mi1).eq(e) && (mi1*mi2).eq(e)) {
            ++sumv; ++succv;
        } else {
            std::cout << "Failed: mu::mat4 inverse" << std::endl;
        }
    }
    
    {
        mat4 a;
        mat4 b;
        
        float ax[16] = {
            0, 0, 0, 1,
            0, 1, 0, 0,
            0, 0, 1, 0,
            1, 0, 0, 0
        };
        float bx[16] = {
            0, 0, 1, 0,
            0, 0, 0, 1,
            0, 1, 0, 0,
            1, 0, 0, 0
        };
        a.set(ax);
        b.set(bx);
        mat4 c = a*b;
        std::cout << "M4C :" << std::endl << c.str() << std::endl;
        c = b+a;
        std::cout << "M4C :" << std::endl << c.str() << std::endl;
    }
    
    
    //std::cout << "Tests: mu: vec and mat: " << succv << "/" << sumv << std::endl;
    sum += sumv; succ+=succv;
}


void quat_tests(int& sum, int& succ)
{
    quat q1;
    quat q2(1.0f,2.0f,3.0f,4.0f);
    std::cout << (q1+q2).str() << std::endl;
}
/*
void line_plane_tests(int& sum, int& succ)
{
    vec3 Q1(0,-1,0);
    vec3 Q2(2,-1,0);
    line3 L(vec3(0,0,0), vec3(1,1,0));
    plane S(vec3(1,1,1), vec3(1,0,0));
}
*/

void run_mu_tests (int& sum, int& succ)
{
    srand((unsigned int)time(0));
    
    int sumi=0, succi=0;
    vec_mat_tests(sumi, succi);
    //quat_tests(sumi, succi);
    //line_plane_tests(sumi, succi);
    //std::cout << "Tests: mu: " << succi << "/" << sumi << std::endl;
    sum += sumi; succ += succi;
}
