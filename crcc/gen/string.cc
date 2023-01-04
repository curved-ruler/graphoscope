
#include "string.hh"

namespace cr {

str32::str32() {}
str32::str32(const char* s)
{
    str8 s8(s);
    
    //TODO
    //s8.data.push_back(0);
    //s8.data.push_back(0);
    //s8.data.push_back(0);
    uint32 ch = 0;
    int type;
    for (size_t i = 0 ; i<s8.data.size()-3 ; )
    {
        type = unicode_8_to_32(s8.data[i], s8.data[i+1], s8.data[i+2], s8.data[i+3], ch);
        if (type <= 0) { std::cout << "UTF8 error" << std::endl; break; }
        else           { data.push_back(ch); i += type; }
    }
}

size_t str32::size () const
{
    return data.size();
}




str8::str8() {}
str8::str8(const char* s)
{
    int i=0;
    str8 s8;
    while (s[i] != 0) { s8.data.push_back(s[i]); ++i; }
}

      char& str8::operator[](size_t i)       { return data[i]; }
const char& str8::operator[](size_t i) const { return data[i]; }





int unicode_8_to_32 (char d0, char d1, char d2, char d3, uint32& u32ch)
{
    int type = -1;
    uint8 uch0 = (uint8)(d0);
    uint8 uch1 = (uint8)(d1);
    uint8 uch2 = (uint8)(d2);
    uint8 uch3 = (uint8)(d3);
    if      ((uch0 & utf8_1_mask) == utf8_1) { type = 1; }
    else if ((uch0 & utf8_2_mask) == utf8_2) { type = 2; }
    else if ((uch0 & utf8_3_mask) == utf8_3) { type = 3; }
    else if ((uch0 & utf8_4_mask) == utf8_4) { type = 4; }
        
    uint32 ch0, ch1, ch2, ch3;
    switch (type)
    {
        case 1:
            u32ch = uch0;
            break;
        case 2:
            ch0 = uch0 & (~utf8_2_mask);
            ch1 = uch1 & (~utf8_d_mask);
            u32ch = (ch0 << 6) + (ch1);
            break;
        case 3:
            ch0 = uch0 & (~utf8_3_mask);
            ch1 = uch1 & (~utf8_d_mask);
            ch2 = uch2 & (~utf8_d_mask);
            u32ch = (ch0 << 12) + (ch1 << 6) + (ch2);
            break;
        case 4:
            ch0 = uch0 & (~utf8_4_mask);
            ch1 = uch1 & (~utf8_d_mask);
            ch2 = uch2 & (~utf8_d_mask);
            ch3 = uch3 & (~utf8_d_mask);
            u32ch = (ch0 << 18) + (ch1 << 12) + (ch2 << 6) + (ch3);
            break;
        default:
            //type error
            break;
    }
    return type;
}
uint32 next_unicode (const std::string& str, size_t& pos)
{
    uint32 c = 0;
    char cc[4];
    cc[0] = 0; cc[1] = 0; cc[2] = 0; cc[3] = 0;
    
    for (size_t i=0 ; i<4 ; ++i)
    {
        if ( (i+pos)<str.size() ) { cc[i] = str[i+pos]; }
    }
    int t = unicode_8_to_32(cc[0], cc[1], cc[2], cc[3], c);
    if (t > 0) pos += t;
    
    return c;
}


int unicode_32_to_8 (uint32 u32ch, char& ch8_0, char& ch8_1, char& ch8_2, char& ch8_3)
{
    int type = -1;
    if      (u32ch <= 0x7F)    { type = 1; }
    else if (u32ch <= 0x7FF)   { type = 2; }
    else if (u32ch <= 0xFFFF)  { type = 3; }
    else if (u32ch <= 0x1FBFF) { type = 4; }
        
    uint32 ch0, ch1, ch2, ch3;
    switch (type)
    {
        case 1:
            ch8_0 = char(u32ch);
            break;
        case 2:
            ch0 = (u32ch >> 6)   | 0xC0;
            ch1 = (u32ch & 0x3F) | 0x80;
            ch8_0 = char(ch0);
            ch8_1 = char(ch1);
            break;
        case 3:
            ch0 = ((u32ch >> 12))        | 0xE0;
            ch1 = ((u32ch >>  6) & 0x3F) | 0x80;
            ch2 = ((u32ch)       & 0x3F) | 0x80;
            ch8_0 = char(ch0);
            ch8_1 = char(ch1);
            ch8_2 = char(ch2);
            break;
        case 4:
            ch0 = ((u32ch >> 18))        | 0xF0;
            ch1 = ((u32ch >> 12) & 0x3F) | 0x80;
            ch2 = ((u32ch >>  6) & 0x3F) | 0x80;
            ch3 = ((u32ch)       & 0x3F) | 0x80;
            ch8_0 = char(ch0);
            ch8_1 = char(ch1);
            ch8_2 = char(ch2);
            ch8_3 = char(ch3);
            break;
        default:
            // type error
            break;
    }
    return type;
}

} // namespace cr

std::ostream& operator<< (std::ostream& os, const cr::str32& s)
{
    /*
    cr::str8 s8;
    char ch[4];
    int type;
    for (size_t i=0 ; i<s.size() ; ++i)
    {
        type = cr::unicode_32_to_8(s.data[i], ch[0], ch[1], ch[2], ch[3]);
        //if (type <= 0) { std::cout << "U32 error, file: " << filename << " at: " << i << std::endl; break; }
        for (int j = 0 ; j<type && j<4 ; ++j)
        {
            s8.data.push_back(ch[j]);
        }
    }
    s8.data.push_back(0);
    os << &(s8.data[0]);
    */
    char ch[4];
    int type;
    for (size_t i=0 ; i<s.size() ; ++i)
    {
        type = cr::unicode_32_to_8(s.data[i], ch[0], ch[1], ch[2], ch[3]);
        //if (type <= 0) { std::cout << "U32 error, file: " << filename << " at: " << i << std::endl; break; }
        for (int j = 0 ; j<type && j<4 ; ++j)
        {
            os << ch[j];
        }
    }
    return os;
}
