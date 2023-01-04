#include "bitutils.hh"

#include <iostream>


namespace cr {


void printbits (unsigned int x)
{
    unsigned int mask = 1;
    int s = sizeof(x)*8;
    int i;
    
    mask <<= (s-1);
    for (i=0 ; i<s ; i++) {
        std::cout << (x & mask ? 1 : 0) << " ";
        mask >>= 1;
    }
    
}


void set_bytes (unsigned int& x, unsigned char r,
                                 unsigned char g,
                                 unsigned char b,
                                 unsigned char a)
{
    x = (r<<24) | (g<<16) | (b<<8) | (a);
}

void get_bytes (unsigned int x, unsigned char& r,
                                unsigned char& g,
                                unsigned char& b,
                                unsigned char& a)
{
    a = (unsigned char)((x       & 0xFF));
    b = (unsigned char)((x >>  8 & 0xFF));
    g = (unsigned char)((x >> 16 & 0xFF));
    r = (unsigned char)((x >> 24 & 0xFF));
}



unsigned int reverse (unsigned int n)
{
    unsigned int v = n;   // input bits to be reversed
    unsigned int r = v;   // r will be reversed bits of v
    unsigned int s = sizeof(v)*8 - 1; // extra shift needed at end
    
    for (v >>= 1; v; v >>= 1)
    {   
        r <<= 1;
        r |= v & 1;
        s--;
    }
    r <<= s;

    return r;

}

unsigned int bit_reverse (unsigned int n, unsigned int bits)
{
    unsigned int v = n;   // input bits to be reversed
    unsigned int r = v;   // r will be reversed bits of v
    unsigned int mask = 1<<bits;
    unsigned int i;
    
    if (bits>32) return 0;
    
    for (i=1 ; i<bits ; i++)
    {
        v >>=1;  
        r <<= 1;
        r |= v & 1;
    }
    r &= mask-1;

    return r;

}





int least_bigger_2n(int x)
{
    int ret = 1;
    while (ret < x)
    {
        ret *= 2;
    }
    return ret;
}


}
