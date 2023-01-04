
#pragma once

#include <vector>
#include <string>

#include "../base.hh"

namespace cr {


class integer {
    private:
        std::vector<uint8> blocks;
        
        static const uint8 blocksize; // bitsize of a block
        static const uint8 MSB;       // most significant bit mask in a block
        
        static const uint8 NaN;       // sign() returns this if NaN
        
    public:
        integer();
        integer(const std::string& sval);
        integer(uint8  x);
        integer( int8  x);
        integer(uint32 x);
        integer( int32 x);
        
        bool is_nan()  const;
        bool is_zero() const;
        int8 sign()    const;
        size_t msb()   const;
        
        int8 cmp(const integer& rhs);
        bool operator==(const integer& rhs);
        bool operator!=(const integer& rhs);
        bool operator< (const integer& rhs);
        bool operator> (const integer& rhs);
        bool operator<=(const integer& rhs);
        bool operator>=(const integer& rhs);
        
        integer& operator++ ();
        integer& operator-- ();
        integer  operator-  () const;
        
        integer  operator+  (const integer& other) const;
        integer& operator+= (const integer& other);
        
        integer  operator-  (const integer& other) const;
        integer& operator-= (const integer& other);
        
        integer  operator*  (const integer& other) const;
        integer& operator*= (const integer& other);
        
        friend void div_and_mod (const integer& a, const integer& b, integer& d, integer& m);
        integer  operator/  (const integer& other) const;
        integer& operator/= (const integer& other);
        integer  operator%  (const integer& other) const;
        integer& operator%= (const integer& other);
        
        
        integer& operator<<= (uint8 s);
        
        
        std::string str_dump() const;
        std::string str(uint8 base = 10) const;
        
    private:
        void shrink_to_fit();
        void stretch();
};

void div_and_mod (const integer& a, const integer& b, integer& d, integer& m);


}
