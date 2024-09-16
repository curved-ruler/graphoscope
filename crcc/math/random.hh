#pragma once

#include <random>

namespace cr {

class fdice
{
    private:
        std::uniform_real_distribution<float> * dist;
        std::mt19937 * gen;
    
    public:
        fdice(unsigned int seed, float min, float max);
        ~fdice();
        float next();
};

class idice
{
    private:
        std::uniform_int_distribution<int> * dist;
        std::mt19937 * gen;
    
    public:
        idice(unsigned int seed, int min, int max);
        ~idice();
        int next();
};

}
