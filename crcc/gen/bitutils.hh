#pragma once


namespace cr {

void printbits (unsigned int x);

void set_bytes (unsigned int& x, unsigned char r,
                                 unsigned char g,
                                 unsigned char b,
                                 unsigned char a);

void get_bytes (unsigned int x, unsigned char& r,
                                unsigned char& g,
                                unsigned char& b,
                                unsigned char& a);


unsigned int reverse (unsigned int n);

unsigned int bit_reverse (unsigned int n, unsigned int bits);


int least_bigger_2n (int x);


}
