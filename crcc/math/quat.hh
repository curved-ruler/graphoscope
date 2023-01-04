#pragma once

#include <cmath>
#include <string>
#include <sstream>

#include "mconst.hh"
#include "utils.hh"
#include "vec.hh"
#include "mat.hh"



namespace cr {

template <typename T>
class quat_t
{
    public:
        T r, i,j,k;
        
    public:
        quat_t()
        {
            nill();
        }
        quat_t(T a, vec3_t<T> t)
        {
            r   = std::cos(a/2);
            T S = std::sin(a/2);
            i = t.x * S;
            j = t.y * S;
            k = t.z * S;
        }
        quat_t(T rr, T ii, T jj, T kk) : r(rr), i(ii), j(jj), k(kk) {}
        
        void set(T a, T b, T c, T d)
        {
            r = a;
            i = b;
            j = c;
            k = d;
        }
        
        void nill()
        {
            r = i = j = k = (T)0;
        }
        
        quat_t<T> operator+ (const quat_t<T>& q2) const
        {
            return quat_t<T>(r + q2.r, i + q2.i, j + q2.j, k + q2.k);
        }
        
        void operator+= (const quat_t<T>& q2)
        {
            r += q2.r;
            i += q2.i;
            j += q2.j;
            k += q2.k;
        }
        
        quat_t<T> operator- () const
        {
            return quat_t<T>(-r, -i, -j, -k);
        }
        
        quat_t<T> operator- (const quat_t<T>& q2) const
        {
            return quat_t<T>(r - q2.r, i - q2.i, j - q2.j, k - q2.k);
        }
        
        void operator-= (const quat_t<T>& q2)
        {
            r -= q2.r;
            i -= q2.i;
            j -= q2.j;
            k -= q2.k;
        }
        
        quat_t<T> operator* (T d) const
        {
            return quat_t<T>(r*d, i*d, j*d, k*d);
        }
        
        void operator*= (T d)
        {
            r *= d;
            i *= d;
            j *= d;
            k *= d;
        }
        
        quat_t<T> operator* (const quat_t<T>& q2) const
        {
            return quat_t<T>(
                r*q2.r - i*q2.i - j*q2.j - k*q2.k,
                r*q2.i + i*q2.r + j*q2.k - k*q2.j,
                r*q2.j - i*q2.k + j*q2.r + k*q2.i,
                r*q2.k + i*q2.j - j*q2.i + k*q2.r
            );
        }
        
        void operator*= (const quat_t<T>& q2)
        {
            vec3_t<T> v = *this * q2;
            *this = v;
        }
        
        quat_t<T> conj () const
        {
            return quat_t<T>(r, -i, -j, -k);
        }
        
        T norm() const
        {
            return sqrt(r*r + i*i + j*j + k*k);
        }
        
        T normn() const
        {
            return (r*r + i*i + j*j + k*k);
        }
        
        void normalize()
        {
            T len = norm();
            if (len > cr::eps) {
                r /= len;
                i /= len;
                j /= len;
                k /= len;
            }
        }
        
        mat4 rot_mat() const
        {
            return {
                r*r+i*i-j*j-k*k,   2*i*j-2*r*k,    2*i*k+2*r*j,   0,
                 2*i*j + 2*r*k,  r*r-i*i+j*j-k*k,  2*j*k-2*r*i,   0,
                 2*i*k-2*r*j,      2*j*k+2*r*i,  r*r-i*i-j*j+k*k, 0,
                      0,                0,              0,        1
            };
        }
        
        std::string str() const
        {
            std::stringstream ss;
            ss << "Q[" << r << ", " << i << ", " << j << ", " << k << "]";
            return ss.str();
        }
};

typedef quat_t<real> quat;
typedef quat_t<int>  iquat;         // ??
typedef quat_t<unsigned int> uquat; // ??
typedef quat_t<float> fquat;
typedef quat_t<double> dquat;

template <typename T>
quat_t<T> axistoquat(vec3_t<T> euleraxis)
{
    T len = euleraxis.length();
    T C = std::cos(len/(T)2);
    T S = std::sin(len/(T)2);
    vec3_t<T> e = (euleraxis.normal()) * S;
    return quat_t<T>(C, e.x, e.y, e.z);
}

}
