
#pragma once

#include <vector>
#include <iostream>

#include "../base.hh"

namespace chars {

enum chrs {
    eol = 10
};

}

namespace cr {

class str32
{
    public:
        std::vector<uint32> data;
    
    public:
        str32();
        str32(const char* s);
        
        size_t size () const;
};



const uint8 utf8_1      = 0;
const uint8 utf8_1_mask = 0b10000000;

const uint8 utf8_2      = 0b11000000;
const uint8 utf8_2_mask = 0b11100000;

const uint8 utf8_3      = 0b11100000;
const uint8 utf8_3_mask = 0b11110000;

const uint8 utf8_4      = 0b11110000;
const uint8 utf8_4_mask = 0b11111000;

const uint8 utf8_d      = 0b10000000;
const uint8 utf8_d_mask = 0b11000000;

int unicode_8_to_32 (char d0, char d1, char d2, char d3, uint32& u32ch);
int unicode_32_to_8 (uint32 u32ch, char& ch8_0, char& ch8_1, char& ch8_2, char& ch8_3);
uint32 next_unicode (const std::string& str, size_t& pos);



class str8
{
    public:
        std::vector<char> data;
        
    public:
        str8();
        str8(const char* s);
        
        size_t size() const;
        
              char& operator[](size_t i);
        const char& operator[](size_t i) const;
};


} // namespace cr

std::ostream& operator<< (std::ostream& os, const cr::str32& s);
