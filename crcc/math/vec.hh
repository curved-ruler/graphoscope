#pragma once

#include <cmath>
#include <string>
#include <sstream>

#include "../base.hh"
#include "mconst.hh"
#include "utils.hh"



namespace cr {

template <typename T>
class vec2_t
{
    public:
        T x,y;
    
    public:
        vec2_t()
        {
            nill();
        }
        
        vec2_t(T xx, T yy) : x(xx), y(yy) {}
        
        bool operator== (const vec2_t<T>& other) const
        {
            if (x == other.x && y == other.y) return true;
            return false;
        }
        
        // floatlike op==
        bool eq (const vec2_t<T>& other) const
        {
            if (zero(x-other.x) && zero(y-other.y)) return true;
            return false;
        }
        
        inline void nill()
        {
            x = y = (T)0;
        }
        
        
        vec2_t<T> operator+ (const vec2_t<T>& bv) const
        {
            return vec2_t<T>(this->x + bv.x, this->y + bv.y);
        }
        
        vec2_t<T> operator- () const
        {
            return vec2_t<T>(-this->x, -this->y);
        }
        
        vec2_t<T> operator- (const vec2_t<T>& bv) const
        {
            return vec2_t<T>(this->x - bv.x, this->y - bv.y);
        }
        
        vec2_t<T> operator* (const vec2_t<T>& bv) const
        {
            return vec2_t<T>(this->x*bv.x, this->y*bv.y);
        }
        
        vec2_t<T> operator* (T d) const
        {
            return vec2_t<T>(x*d, y*d);
        }
        
        vec2_t<T> operator/ (T d) const
        {
            return vec2_t<T>(x/d, y/d);
        }
        
        vec2_t<T> operator/ (const vec2_t<T>& bv) const
        {
            return vec2_t<T>(this->x / bv.x, this->y / bv.y);
        }
        
        void operator*= (T d)
        {
            this->x *= d;
            this->y *= d;
        }
        
        void operator/= (const T d)
        {
            this->x /= d;
            this->y /= d;
        }
        
        void operator+= (const vec2_t<T>& bv)
        {
            this->x += bv.x;
            this->y += bv.y;
        }
        
        void operator-= (const vec2_t<T>& bv)
        {
            this->x -= bv.x;
            this->y -= bv.y;
        }
        
        vec2_t<T> left() const
        {
            return vec2_t<T>(-y, x);
        }
        vec2_t<T> right() const
        {
            return vec2_t<T>(y, -x);
        }
        
        
        T length() const
        {
            return sqrt(this->x*this->x + this->y*this->y);
        }
        
        T lenn() const
        {
            return (this->x*this->x + this->y*this->y);
        }
        
        void maximize(T maxlen)
        {
            T len = length();
            if (len > maxlen)
            {
                this->x *= maxlen / len;
                this->y *= maxlen / len;
            }
        }
        
        void setlength(T nl)
        {
            T len = length();
            if (len > cr::eps)
            {
                this->x *= nl / len;
                this->y *= nl / len;
            }
        }
        
        void normalize()
        {
            setlength((T)1);
        }
        
        vec2_t<T> normal() const
        {
            T len = length();
            if (len > cr::eps)
            {
                return vec2_t<T>(x / len, y / len);
            }
            else
            {
                return vec2_t<T>((T)0, (T)0);
            }
        }

        std::string str() const
        {
            std::stringstream ss;
            ss << "V[" << this->x << ", " << this->y << "]";
            return ss.str();
        }
};

template <typename T>
class vec3_t
{
    public:
        T x,y,z;
    
    public:
        vec3_t()
        {
            nill();
        }
        
        vec3_t(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
        
        
        vec2_t<T> xy() const
        {
            return vec2_t<T>(x,y);
        }
        vec2_t<T> xz() const
        {
            return vec2_t<T>(x,z);
        }
        vec2_t<T> yz() const
        {
            return vec2_t<T>(y,z);
        }
        
        bool operator== (const vec3_t<T>& other) const
        {
            if (x == other.x && y == other.y && z = other.z) return true;
            return false;
        }
        
        // floatlike op==
        bool eq (const vec3_t<T>& other) const
        {
            if (zero(x-other.x) && zero(y-other.y) && zero(z-other.z)) return true;
            return false;
        }
        
        inline void set(T xx, T yy, T zz)
        {
            x = xx;
            y = yy;
            z = zz;
        }
        
        inline void nill()
        {
            x = y = z = (T)0;
        }
        
        
        vec3_t<T> operator+ (const vec3_t<T>& bv) const
        {
            return vec3_t<T>(this->x + bv.x, this->y + bv.y, this->z + bv.z);
        }
        
        vec3_t<T> operator- () const
        {
            return vec3_t<T>(-this->x, -this->y, -this->z);
        }
        
        vec3_t<T> operator- (const vec3_t<T>& bv) const
        {
            return vec3_t<T> (this->x - bv.x, this->y - bv.y, this->z - bv.z);
        }
        
        vec3_t<T> operator* (const vec3_t<T>& bv) const
        {
            return vec3_t<T>(this->x*bv.x, this->y*bv.y, this->z*bv.z);
        }
        
        vec3_t<T> operator* (T d) const
        {
            return vec3_t<T>(x*d, y*d, z*d);
        }
        
        vec3_t<T> operator/ (T d) const
        {
            return vec3_t<T>(x/d, y/d, z/d);
        }
        
        vec3_t<T> operator/ (const vec3_t<T>& bv) const
        {
            return vec3_t<T>(this->x / bv.x, this->y / bv.y, this->z / bv.z);
        }
        
        void operator*= (T d)
        {
            this->x *= d;
            this->y *= d;
            this->z *= d;
        }
        
        void operator/= (const T d)
        {
            this->x /= d;
            this->y /= d;
            this->z /= d;
        }
        
        void operator+= (const vec3_t<T>& bv)
        {
            this->x += bv.x;
            this->y += bv.y;
            this->z += bv.z;
        }
        
        void operator-= (const vec3_t<T>& bv)
        {
            this->x -= bv.x;
            this->y -= bv.y;
            this->z -= bv.z;
        }
        
        void operator%= (const vec3_t<T>& bv)
        {
            vec3_t<T> v = *this % bv;
            *this = v;
        }
        
        
        
        T length() const
        {
            return sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
        }
        
        T lenn() const
        {
            return (this->x*this->x + this->y*this->y + this->z*this->z);
        }
        
        void maximize(T maxlen)
        {
            T len = length();
            if (len > maxlen)
            {
                this->x *= maxlen / len;
                this->y *= maxlen / len;
                this->z *= maxlen / len;
            }
        }
        
        void setlength(T nl)
        {
            T len = length();
            if (len > cr::eps)
            {
                this->x *= nl / len;
                this->y *= nl / len;
                this->z *= nl / len;
            }
        }
        
        vec3_t<T> withlength(T nl) const
        {
            T len = length();
            if (len > cr::eps) { return *this * (nl/len); }
            return vec3_t<T>((T)0, (T)0, (T)0);
        }
        
        void normalize()
        {
            setlength((T)1);
        }
        
        vec3_t<T> normal() const
        {
            T len = length();
            if (len > cr::eps)
            {
                return vec3_t<T>(x / len, y / len, z / len);
            }
            else
            {
                return vec3_t<T>((T)0, (T)0, (T)0);
            }
        }

        std::string str() const
        {
            std::stringstream ss;
            ss << "[" << this->x << ", " << this->y << ", " << this->z << "]";
            return ss.str();
        }
};

template <typename T>
class vec4_t
{
    public:
        T x, y, z, w;
    
    public:
        vec4_t()
        {
            nill();
        }
        
        vec4_t(T xx, T yy, T zz, T ww) : x(xx), y(yy), z(zz), w(ww) {}
        
        inline void nill()
        {
            x = y = z = w = (T)0;
        }
        
        bool operator== (const vec4_t<T>& other) const
        {
            if (x == other.x && y == other.y && z == other.z && w = other.w) return true;
            return false;
        }
        
        // floatlike op==
        bool eq (const vec4_t<T>& other) const
        {
            if (zero(x-other.x) && zero(y-other.y) && zero(z-other.z) && zero(w-other.w)) return true;
            return false;
        }
        
        T length() const
        {
            return sqrt(this->x*this->x + this->y*this->y + this->z*this->z + this->w*this->w);
        }
        
        T lenn() const
        {
            return (this->x*this->x + this->y*this->y + this->z*this->z + this->w*this->w);
        }
        
        void maximize(T maxlen)
        {
            T len = length();
            if (len > maxlen)
            {
                this->x *= maxlen / len;
                this->y *= maxlen / len;
                this->z *= maxlen / len;
                this->w *= maxlen / len;
            }
        }
        
        void setlength(T nl)
        {
            T len = length();
            if (len > cr::eps)
            {
                this->x *= nl / len;
                this->y *= nl / len;
                this->z *= nl / len;
                this->w *= nl / len;
            }
        }
        
        void normalize()
        {
            setlength((T)1);
        }
        
        vec2_t<T> normal() const
        {
            T len = length();
            if (len > cr::eps)
            {
                return vec2_t<T>(x / len, y / len, z / len, w / len);
            }
            else
            {
                return vec2_t<T>((T)0, (T)0, (T)0, (T)0);
            }
        }

        std::string str() const
        {
            std::stringstream ss;
            ss << "V[" << this->x << ", " << this->y << ", " << this->z << ", " << this->w << "]";
            return ss.str();
        }
};

template<typename T>
inline T dot (const vec2_t<T>& va, const vec2_t<T>& vb)
{
    return (va.x*vb.x + va.y*vb.y);
}

template<typename T>
inline T dot (const vec3_t<T>& va, const vec3_t<T>& vb)
{
    return (va.x*vb.x + va.y*vb.y + va.z*vb.z);
}

template<typename T>
inline T dot (const vec4_t<T>& va, const vec4_t<T>& vb)
{
    return (va.x*vb.x + va.y*vb.y + va.z*vb.z + va.w*vb.w);
}

template<typename T>
inline vec3_t<T> cross (const vec3_t<T>& va, const vec3_t<T>& vb)
{
    return vec3_t<T>(
        va.y * vb.z - va.z * vb.y,
        va.z * vb.x - va.x * vb.z,
        va.x * vb.y - va.y * vb.x
    );
}

template<typename T>
inline T fract (const T& x)
{
    return x-(int)x;
}
template<typename T>
inline vec2_t<T> fract (const vec2_t<T>& v)
{
    return vec2_t<T>(fract(v.x), fract(v.y));
}
template<typename T>
inline vec2_t<T> floor (const vec2_t<T>& v)
{
    return vec2_t<T>(std::floor(v.x), std::floor(v.y));
}
template<typename T>
inline vec3_t<T> fract (const vec3_t<T>& v)
{
    return vec3_t<T>(fract(v.x), fract(v.y), fract(v.z));
}
template<typename T>
inline vec3_t<T> floor (const vec3_t<T>& v)
{
    return vec3_t<T>(std::floor(v.x), std::floor(v.y), std::floor(v.z));
}


typedef vec2_t<float> vec2;
typedef vec2_t<int>  ivec2;
typedef vec2_t<unsigned int> uvec2;
typedef vec2_t<float>  fvec2;
typedef vec2_t<double> dvec2;

typedef vec3_t<float> vec3;
typedef vec3_t<int>  ivec3;
typedef vec3_t<unsigned int> uvec3;
typedef vec3_t<float>  fvec3;
typedef vec3_t<double> dvec3;

typedef vec4_t<float> vec4;
typedef vec4_t<int>  ivec4;
typedef vec4_t<unsigned int> uvec4;
typedef vec4_t<float>  fvec4;
typedef vec4_t<double> dvec4;


}
