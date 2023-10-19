
#include "integer.hh"

#include <iostream>
#include <algorithm>

#include "../gen/stringutils.hh"


namespace cr {

const uint8 integer::blocksize = 8;
const uint8 integer::MSB       = 128;
const int8  integer::NaN       = -4;

integer::integer() {}

integer::integer(uint8 x)
{
    blocks.push_back(x);
    blocks.push_back(0);
    shrink_to_fit();
}
integer::integer( int8 x)
{
    blocks.push_back(uint8(x));
}
integer::integer(uint32 x)
{
    uint8 xb[4];
    xb[0] = uint8((x)       & 0xFF);
    xb[1] = uint8((x >> 8)  & 0xFF);
    xb[2] = uint8((x >> 16) & 0xFF);
    xb[3] = uint8((x >> 24));
    
    blocks.push_back(xb[0]);
    blocks.push_back(xb[1]);
    blocks.push_back(xb[2]);
    blocks.push_back(xb[3]);
    blocks.push_back(0);
    
    shrink_to_fit();
}
integer::integer( int32 x)
{
    uint8 xb[4];
    xb[0] = uint8((x)       & 0xFF);
    xb[1] = uint8((x >> 8)  & 0xFF);
    xb[2] = uint8((x >> 16) & 0xFF);
    xb[3] = uint8((x >> 24));
    
    blocks.push_back(xb[0]);
    blocks.push_back(xb[1]);
    blocks.push_back(xb[2]);
    blocks.push_back(xb[3]);
    
    shrink_to_fit();
}

integer::integer(const std::string& sval)
{
    std::string s = trim(sval);
    size_t SN = s.size();
    if (SN == 0) return;
    
    bool negative = (s[0] == '-');
    uint8 base = 10;
    size_t baseflag = s.find_last_of('@');
    if (baseflag != std::string::npos && baseflag != SN-1)
    {
        base = 0;
        uint8 exp = 1;
        for (size_t i=SN ; i>(baseflag+1) ; --i) { base += (s[i-1] - '0') * exp; exp *= 10; }
    }
    //std::cout << "B " << uint32(base) << std::endl;
    
    
    std::vector<uint8> val;
    size_t beg = negative ? 1 : 0;
    size_t end = baseflag == std::string::npos ? SN : baseflag;
    if (beg == end) return;
    
    for (size_t i=beg ; i<end ; ++i)
    {
        char c    = s[i];
        int  cval = -1;
        if      (c >= '0' && c <= '9') cval = c - '0';
        else if (c >= 'a' && c <= 'z') cval = c - 'a' + 10;
        else if (c >= 'A' && c <= 'Z') cval = c - 'A' + 10;
        else if (c == '_' || c == ' ') continue;
        
        if (cval < 0 || cval >= base) return;
        
        val.push_back(uint8(cval));
    }
    
    /*
     * Creation by bits
     * 
    size_t ibit = 0;
    bool   nonz = true;
    while (nonz)
    {
        if (ibit >= blocks.size() * blocksize) { blocks.push_back(0); }
        if (val[0] % 2 == 1)
        {
            uint8 bbit = ibit % blocksize;
            blocks[blocks.size()-1] |= 1 << bbit;
        }
        ++ibit;
        
        std::vector<uint8> val2;
        size_t VN = val.size();
        uint8 carry = 0;
        nonz        = false;
        for (size_t i=VN ; i!=0 ; --i)
        {
            uint8 vali = val[i-1] + carry;
            carry = (vali % 2 == 1) ? base : 0;
            val2.push_back(vali / 2);
            if (vali / 2 > 0) nonz = true;
        }
        std::reverse(val2.begin(), val2.end());
        val = val2;
    }
    */
    
    
    do
    {
        std::vector<uint8> val2;
        size_t VN = val.size();
        uint32 sum = 0;
        
        for (size_t i=0 ; i<VN ; ++i)
        {
            sum = sum * base + val[i];
            //std::cout << sum << std::endl;
            
            if (sum > 255)
            {
                val2.push_back(uint8(sum / 256));
                sum = sum % 256;
            }
        }
        val = val2;
        
        blocks.push_back(uint8(sum));
    }
    while (!val.empty());
    
    
    if (negative)
    {
        uint8 carry = 1;
        for (size_t i = 0 ; i<blocks.size() ; ++i)
        {
            blocks[i] = ~blocks[i];
            uint8 x = blocks[i] + carry;
            carry = x < blocks[i] ? 1 : 0;
            blocks[i] = x;
        }
    }
    else
    {
        if ((blocks[blocks.size()-1] & MSB) == MSB)
        {
            blocks.push_back(0);
        }
    }
}

bool integer::is_nan()  const { return blocks.size() == 0; }
bool integer::is_zero() const
{
    if (is_nan()) return false;
    for (size_t i=0 ; i<blocks.size() ; ++i) { if (blocks[i] != 0) return false; }
    return true;
}
int8 integer::sign() const
{
    if (is_nan()) return NaN;
    if (is_zero()) return 0;
    return ( (blocks[blocks.size()-1] & MSB) == 0 ) ? 1 : -1;
}



void integer::shrink_to_fit()
{
    uint8 del = 0;
    uint8 bit = 0;
    
    if (sign() == -1)
    {
        del = 0xFF;
        bit = MSB;
    }
    else
    {
        del = 0;
        bit = 0;
    }
    
    size_t e = 0;
    for (size_t i=blocks.size() ; i>1 ; --i)
    {
        if (blocks[i-1] == del && (blocks[i-2] & MSB) == bit)
        {
            ++e;
        }
        else
        {
            break;
        }
    }
    if (e > 0) { blocks.erase(blocks.end() - e, blocks.end()); }
}
void integer::stretch()
{
    int8 s = sign();
    if      (s == -1)          { blocks.push_back(0xFF); }
    else if (s == 0 || s == 1) { blocks.push_back(0); }
}


// Not too optimized
integer& integer::operator<<= (uint8 s)
{
    int8 sg = sign();
    if (sg == NaN) return *this;
    
    uint8 fill   = (sg < 0) ? 0xFF : 0;
    bool bitcmp  = (sg < 0) ?    0 : 1;
    
    for (uint8 i=0 ; i<s ; ++i)
    {
        uint8 mask = MSB >> 1;
        bool lastbit = blocks[blocks.size()-1] & mask;
        if (lastbit == bitcmp) { blocks.push_back(fill); }
        
        uint8 carry1 = 0;
        uint8 carry2 = 0;
        for (size_t j=0 ; j<blocks.size() ; ++j)
        {
            carry1 = (blocks[j] & MSB) ? 1 : 0;
            blocks[j] <<= 1;
            blocks[j] |= carry2;
            carry2 = carry1;
        }
    }
    return *this;
}


size_t integer::msb() const
{
    int8 s = sign();
    if (s == NaN) return 0;
    
    uint8 mask = MSB >> 1;
    bool bitcmp  = (s < 0) ?    0 : 1;
    size_t i = 7;
    while (mask)
    {
        bool lastbit = blocks[blocks.size()-1] & mask;
        
        if (lastbit == bitcmp) 
        {
            return ((blocks.size()-1) * blocksize) + i;
        }
        mask >>= 1;
        --i;
    }
    return (blocks.size()-1) * blocksize;
}
int8 integer::cmp(const integer& rhs)
{
    int8 sl = sign();
    int8 sr = rhs.sign();
    if (sl == NaN || sr == NaN) return NaN;
    if (sl <  0   && sr >= 0)   return  -1;
    if (sl >= 0   && sr <  0)   return   1;
    
    size_t lsize = blocks.size();
    size_t rsize = rhs.blocks.size();
    if (lsize > rsize) { return (sl>=0) ?  1 : -1; }
    if (lsize < rsize) { return (sl>=0) ? -1 :  1; }
    
    for (size_t n = lsize ; n>0 ; --n)
    {
        if (blocks[n-1] > rhs.blocks[n-1]) return  1;
        if (blocks[n-1] < rhs.blocks[n-1]) return -1;
    }
    return 0;
}
bool integer::operator==(const integer& rhs) { return this->cmp(rhs) == 0; }
bool integer::operator!=(const integer& rhs) { return this->cmp(rhs) != 0; }
bool integer::operator< (const integer& rhs) { return this->cmp(rhs) <  0; }
bool integer::operator> (const integer& rhs) { return this->cmp(rhs) >  0; }
bool integer::operator<=(const integer& rhs) { return this->cmp(rhs) <= 0; }
bool integer::operator>=(const integer& rhs) { return this->cmp(rhs) >= 0; }

std::string integer::str_dump() const
{
    std::string ret = "";
    for (size_t i=blocks.size() ; i>0 ; --i)
    {
        uint8 mask = MSB;
        while (mask)
        {
            ret += (blocks[i-1] & mask) ? '1' : '0';
            mask >>= 1;
        }
    }
    
    return ret;
}
std::string integer::str(uint8 base) const
{
    //if (base == 16)
    
    std::string ret = "";
    if (is_nan() || base < 2) return ret;
    
    cr::integer ii = (sign() == -1) ? -*this : *this;
    std::vector<uint8> b1;
    for (size_t i=ii.blocks.size() ; i>0 ; --i) { b1.push_back(ii.blocks[i-1]); }
    
    bool nonz;
    do
    {
        nonz = false;
        
        std::vector<uint8> b2;
        size_t BN = b1.size();
        uint8  carry = 0;
        
        for (size_t i=0 ; i<BN ; ++i)
        {
            uint32 vali = b1[i] + carry*256;
            carry = vali % base;
            //std::cout << std::endl << "VC: " << vali << " | " << uint32(carry) << std::endl;
            
            uint8 v = uint8((vali / base) & 0xFF);
            b2.push_back(v);
            if (v > 0) nonz = true;
        }
        b1 = b2;
        
        char c = char(carry);
        ret.push_back( (c < 10) ? c+'0' : c+'A' );
    }
    while (nonz);
    
    if (sign() == -1) ret.push_back('-');
    std::reverse(ret.begin(), ret.end());
    
    return ret;
}





integer integer::operator+ (const integer& other) const
{
    integer res;
    if (is_nan() || other.is_nan()) return res;
    
    size_t nt = blocks.size();
    size_t no = other.blocks.size();
    size_t N = (nt >= no) ? nt : no;
    
    uint8 fillt = (      sign() == -1) ? 0xFF : 0;
    uint8 fillo = (other.sign() == -1) ? 0xFF : 0;
    
    uint8 a, b, carry = 0;
    uint16 c;
    
    for (size_t i=0 ; i<=N ; ++i)
    {
        a = (i < nt) ?       blocks[i] : fillt;
        b = (i < no) ? other.blocks[i] : fillo;
        c = uint16(a) + uint16(b) + uint16(carry);
        carry = uint8(c >> 8);
        res.blocks.push_back(uint8(c & 0xFF));
    }
    
    res.shrink_to_fit();
    return res;
}
integer& integer::operator+= (const integer& other)
{
    *this = *this + other;
    return *this;
}

integer& integer::operator++ ()
{
    uint8  carry = 0;
    uint16 c;
    
    stretch();
    
    uint8 b = 1;
    for (size_t i=0 ; i<blocks.size() ; ++i)
    {
        c = uint16(blocks[i]) + uint16(b) + uint16(carry);
        b = 0;
        carry = uint8(c >> 8);
        blocks[i] = uint8(c & 0xFF);
    }
    
    shrink_to_fit();
    return *this;
}
integer& integer::operator-- ()
{
    uint8  carry = 0;
    uint16 c;
    
    stretch();
    
    for (size_t i=0 ; i<blocks.size() ; ++i)
    {
        c = uint16(blocks[i]) + uint16(0xFF) + uint16(carry);
        carry = uint8(c >> 8);
        blocks[i] = uint8(c & 0xFF);
    }
    
    shrink_to_fit();
    return *this;
}
integer integer::operator- () const
{
    integer res;
    if (is_nan()) return res;
    
    bool neg = (sign() == -1);
    for (size_t i=0 ; i<blocks.size() ; ++i)
    {
        res.blocks.push_back(~blocks[i]);
    }
    res.blocks.push_back(neg ? 0 : 0xFF);
    ++res;
    
    return res;
}
integer integer::operator- (const integer& other) const
{
    return this->operator+(-other);
}
integer& integer::operator-= (const integer& other)
{
    *this = *this - other;
    return *this;
}

integer integer::operator* (const integer& other) const
{
    integer ret;
    
    int8 s1 = sign();
    int8 s2 = other.sign();
    if (s1 == NaN || s2 == NaN) return ret;
    
    ret = 0;
    int8 flip_sign = s1*s2;
    if (flip_sign == 0) return ret;
    
    integer a = (s1 == 1) ? *this : -*this;
    integer b = (s2 == 1) ? other : -other;
    
    integer tmp;
    uint16  c;
    uint8   carry;
    
    for (size_t i=0 ; i<a.blocks.size() ; ++i)
    {
        carry = 0;
        tmp.blocks.clear();
        for (size_t ii = 0 ; ii<i ; ++ii) { tmp.blocks.push_back(0); }
        
        for (size_t j=0 ; j<b.blocks.size() ; ++j)
        {
            c     = uint16(a.blocks[i]) * uint16(b.blocks[j]) + uint16(carry);
            carry = uint8(c >> 8);
            
            tmp.blocks.push_back(uint8(c & 0xFF));
        }
        
        tmp.blocks.push_back(carry);
        tmp.blocks.push_back(0);
        //tmp.shrink_to_fit();
        ret += tmp;
    }
    
    if (flip_sign < 0) ret = -ret;
    
    return ret;
}
integer& integer::operator*= (const integer& other)
{
    *this = *this * other;
    return *this;
}

integer integer::operator/ (const integer& other) const
{
    integer _m;
    integer ret;
    div_and_mod(*this, other, ret, _m);
    return ret;
}
integer& integer::operator/= (const integer& other)
{
    *this = *this / other;
    return *this;
}
integer integer::operator% (const integer& other) const
{
    integer _d;
    integer ret;
    div_and_mod(*this, other, _d, ret);
    return ret;
}
integer& integer::operator%= (const integer& other)
{
    *this = *this % other;
    return *this;
}

void div_and_mod (const integer& a, const integer& b, integer& d, integer& m)
{
    integer D, AAcc;
    
    int8 s1 = a.sign();
    int8 s2 = b.sign();
    if (s1 == integer::NaN || s2 == integer::NaN || s2 == 0)
    {
        d = D; m = AAcc;
        return;
    }
    
    int8 flip_sign = s1*s2;
    integer A = (s1 >= 0) ? a : -a;
    integer B = (s2 >= 0) ? b : -b;
    
    D    = 0;
    AAcc = 0;
    uint8 mask;
    size_t AS = A.blocks.size();
    for (size_t i=1 ; i<=AS; ++i)
    {
        mask = integer::MSB;
        for (size_t j=0 ; j<8 ; ++j)
        {
            bool actbit = A.blocks[AS-i] & mask;
            
            AAcc <<= 1;
            if (actbit) ++AAcc;
            
            if (AAcc >= B)
            {
                D <<= 1;
                ++D;
                AAcc -= B;
            }
            else
            {
                D <<= 1;
            }
            
            mask >>= 1;
        }
    }
    
    d = (flip_sign >= 0) ? D : -D;
    m = (flip_sign >= 0) ? AAcc : -AAcc;
}

}
