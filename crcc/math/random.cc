
#include "random.hh"

#include <cmath>


namespace cr {

fdice::fdice(unsigned int seed, float min, float max)
{
    dist = new std::uniform_real_distribution<float>(min, max);
    gen = new std::mt19937(seed);
}
fdice::~fdice()
{
    delete dist;
    delete gen;
}
float fdice::next()
{
    return dist->operator()(*gen);
}



idice::idice(unsigned int seed, int min, int max)
{
    dist = new std::uniform_int_distribution<int>(min, max);
    gen = new std::mt19937(seed);
}
idice::~idice()
{
    delete dist;
    delete gen;
}
int idice::next()
{
    return dist->operator()(*gen);
}

}
