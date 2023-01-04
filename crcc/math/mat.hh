#pragma once

#include <initializer_list>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "../base.hh"
#include "mconst.hh"
#include "utils.hh"
#include "vec.hh"


namespace cr {

template <typename T, unsigned int N>
class gaussjordan
{
    private:
        T** data;
        bool singular;
        
    public:
        template <typename T2>
        gaussjordan(T2** coeffs)
        {
            singular = false;
            space2();
            for (unsigned int x=0 ; x<N ; ++x) {
                for (unsigned int y=0 ; y<N ; ++y) {
                    data[x][y] = coeffs[x][y];
                }
            }
            for (unsigned int x=0 ; x<N ; ++x) {
                for (unsigned int y=N ; y<2*N ; ++y) {
                    if (x==(y-N)) data[x][y] = 1.0;
                    else          data[x][y] = 0.0;
                }
            }
        }
        
        ~gaussjordan()
        {
            for (unsigned int i=0 ; i<N ; ++i) delete[] data[i];
            delete[] data;
        }
        
        void space2()
        {
            data = new T*[N];
            for (unsigned int i=0 ; i<N ; ++i) data[i] = new T[2*N];
        }
        
        unsigned int findnonzero(unsigned int i) const
        {
            for (unsigned int x=i ; x<N ; ++x) {
                if (nonz(data[x][i])) return x;
            }
            return -1;
        }
        
        void swap(unsigned int i, unsigned int j)
        {
            T tmp;
            for (unsigned int y=0 ; y<2*N ; ++y) {
                tmp = data[i][y];
                data[i][y] = data[j][y];
                data[j][y] = tmp;
            }
        }
        
        void mult(unsigned int i, T lambda)
        {
            for (unsigned int y=0 ; y<2*N ; ++y) {
                data[i][y] = lambda*data[i][y];
            }
        }
        
        void mix(unsigned int i, unsigned int j, T lambda)
        {
            for (unsigned int y=0 ; y<2*N ; ++y) {
                data[i][y] = data[i][y] - lambda*data[j][y];
            }
        }
        
        void inv()
        {
            for (unsigned int i=0 ; i<N ; ++i) {
                unsigned int nz = findnonzero(i);
                if (nz == -1) {
                    singular = true;
                    return;
                } else if (nz != i) {
                    swap(i, nz);
                }
                
                mult(i, 1.0/data[i][i]);
                
                for (unsigned int j=i+1 ; j<N ; ++j) {
                    if (nonz(data[j][i])) {
                        mix(j, i, data[j][i]);
                    }
                }
            }
            
            for (unsigned int i=0 ; i<(N-1) ; ++i) {
                for (unsigned int j=i+1 ; j<N ; ++j) {
                    mix(i, j, data[i][j]);
                }
            }
        }
        
        bool isSingular() const
        {
            return singular;
        }
        
        T get(unsigned int i, unsigned int j)
        {
            if (i<N && j<N) return data[i][j+N];
            else return 0;
        }
        
        std::string str() const
        {
            std::stringstream ss("");
            for (unsigned int x=0 ; x<N ; ++x) {
                ss << "[";
                for (unsigned int y=0 ; y<2*N ; ++y) {
                    ss << data[x][y];
                    if (y == 2*N-1) {
                        ss << "]";
                    } else {
                        ss << ", ";
                    }
                }
                if (x < N-1) {
                    ss << std::endl;
                }
            }
            return ss.str();
        }
};


template <typename T, unsigned int N, unsigned int M>
class mat
{
    private:
        T** data;
    public:
        mat()
        {
            space();
        }
        
        mat(const mat<T, N, M>& other)
        {
            space();
            *this = other;
        }
        
        mat(std::initializer_list<std::initializer_list<T>> il)
        {
            space();
            
            int i=0;
            int j=0;
            for (auto x=il.begin() ; x!=il.end() ; ++x) {
                if (i>=N) break;
                j = 0;
                for (auto y=x->begin() ; y!=x->end() ; ++y) {
                    if (j>=M) break;
                    data[i][j] = *y;
                    ++j;
                }
                ++i;
            }
        }
        
        virtual ~mat()
        {
            for (unsigned int i=0 ; i<N ; ++i) delete[] data[i];
            delete[] data;
        }
        
        void space()
        {
            data = new T*[N];
            for (unsigned int i=0 ; i<N ; ++i) data[i] = new T[M];
            
            for (unsigned int x=0 ; x<N ; ++x) {
                for (unsigned int y=0 ; y<M ; ++y) {
                    data[x][y] = 0;
                }
            }
        }
        
        mat<T, N, M>& operator= (const mat<T, N, M>& other)
        {
            if ((void*)this == (void*)&other) return *this;
            for (unsigned int x=0 ; x<N ; ++x) {
                for (unsigned int y=0 ; y<M ; ++y) {
                    data[x][y] = other(x,y);
                }
            }
            return *this;
        }
        
        template <typename T2, unsigned int N2>
        mat<T, N, N2> operator* (const mat<T2, M, N2>& other) const
        {
            mat<T, N, N2> ret;
            for (unsigned int x=0 ; x<N ; ++x) {
                for (unsigned int y=0 ; y<N2 ; ++y) {
                    T c = 0;
                    for (unsigned int i=0 ; i<M ; ++i) {
                        c += data[x][i] * other(i, y);
                    }
                    ret[x][y] = c;
                }
            }
            return ret;
        }
        
        T* operator[] (unsigned int i)
        {
            if (i<N) return data[i];
            else return 0;
        }
        
        T operator() (unsigned int i, unsigned int j) const
        {
            return data[i][j];
        }
        
        mat<T, N, N> inverse() const
        {
            gaussjordan<T, N> migj(data);
            migj.inv();
            if (migj.isSingular()) throw std::runtime_error("SINGULAR");
            mat<T, N, N> mi;
            for (unsigned int x=0 ; x<N ; ++x) {
                for (unsigned int y=0 ; y<N ; ++y) {
                    mi[x][y] = migj.get(x,y);
                }
            }
            return mi;
        }
        
        std::string str() const
        {
            std::stringstream ss("");
            for (unsigned int x=0 ; x<N ; ++x) {
                ss << "[";
                for (unsigned int y=0 ; y<M ; ++y) {
                    ss << data[x][y];
                    if (y == M-1) {
                        ss << "]";
                    } else {
                        ss << ", ";
                    }
                }
                if (x < N-1) {
                    ss << std::endl;
                }
            }
            return ss.str();
        }
};




template <typename T>
class mat4_t
{
    public:
        T data[16];
        
    public:
        mat4_t()
        {
            identity();
        }
        
        mat4_t(std::initializer_list<T> il)
        {
            int i=0;
            for (auto x=il.begin() ; x!=il.end() ; ++x) {
                if (i >= 16) break;
                data[i] = *x;
                ++i;
            }
        }
        
        mat4_t(const mat4_t<T>& other)
        {
            set(other.data);
        }
        
        void identity()
        {
            T tmp[16] = {
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
            };
            set(tmp);
        }
        
        // elems must be at least 16 item array
        void set(const T elems[])
        {
            for (int i=0 ; i<16 ; ++i) this->data[i] = elems[i];
        }
        
        bool eq (const mat4_t<T>& other)
        {
            for (int i=0 ; i<16 ; ++i) if (!zero(data[i]-other.data[i])) return false;
            return true;
        }
        
        
        mat4_t<T>& operator= (const mat4_t<T>& other)
        {
            if (this == &other) return *this;
            set(other.data);
            return *this;
        }
        
        
        T operator[] (int pos) const
        {
            // TODO exception
            if (pos<0 || pos>15) return -1.0f;
            return data[pos];
        }
        
        mat4_t<T> operator+ (const mat4_t<T>& other) const
        {
            mat4_t<T> ret;
            for (int i=0 ; i<16 ; ++i) {
                ret.data[i] = this->data[i] + other.data[i];
            }
            return ret;
        }
        
        mat4_t<T> operator- () const
        {
            mat4_t<T> ret;
            for (int i=0 ; i<16 ; ++i) {
                ret.data[i] = -(this->data[i]);
            }
            return ret;
        }
        
        mat4_t<T> operator- (const mat4_t<T>& other) const
        {
            mat4_t<T> ret;
            for (int i=0 ; i<16 ; ++i) {
                ret.data[i] = this->data[i] - other.data[i];
            }
            return ret;
        }
        
        mat4_t<T> operator* (const mat4_t<T>& other) const
        {
            mat4_t<T> ret;
            float sum;
            for (int i=0 ; i<4 ; ++i) {
                for (int j=0 ; j<4 ; ++j) {
                    sum = 0;
                    for (int k=0 ; k<4 ; ++k) {
                        sum += this->data[j*4 + k] * other.data[k*4 + i];
                    }
                    ret.data[j*4 + i] = sum;
                }
            }
            return ret;
        }
        
        void operator+= (const mat4_t<T>& other)
        {
            for (int i=0 ; i<16 ; ++i) {
                this->data[i] += other.data[i];
            }
        }
        
        void operator-= (const mat4_t<T>& other)
        {
            for (int i=0 ; i<16 ; ++i) {
                this->data[i] -= other.data[i];
            }
        }
        
        void operator*= (const mat4_t<T>& other)
        {
            *this = *this * other;
        }
        
        
        // matrix: row major order, column vector
        vec3_t<T> operator* (const vec3_t<T>& v) const
        {
            vec3_t<T> ret;
            ret.x = v.x*data[0] + v.y*data[1] + v.z*data[2]  + data[3];
            ret.y = v.x*data[4] + v.y*data[5] + v.z*data[6]  + data[7];
            ret.z = v.x*data[8] + v.y*data[9] + v.z*data[10] + data[11];
            T h   = v.x*data[12] + v.y*data[13] + v.z*data[14] + data[15];
            if (cr::zero(h))
            {
                // exception??
                ret.x = 0.0;
                ret.y = 0.0;
                ret.z = 0.0;
            } else {
                ret *= (T(1.0)/h);
            }
            return ret;
        }
        
        void operator*= (const vec3_t<T>& v)
        {
            vec3_t<T> ret = *this * v;
            *this = ret;
        }
        
        mat4_t<T> inverse() const
        {
            T inv[16], det;
            const T* m = data;
            int i;

            inv[0] = m[5]  * m[10] * m[15] -
                     m[5]  * m[11] * m[14] -
                     m[9]  * m[6]  * m[15] +
                     m[9]  * m[7]  * m[14] +
                     m[13] * m[6]  * m[11] -
                     m[13] * m[7]  * m[10];

            inv[4] = -m[4]  * m[10] * m[15] +
                      m[4]  * m[11] * m[14] +
                      m[8]  * m[6]  * m[15] -
                      m[8]  * m[7]  * m[14] -
                      m[12] * m[6]  * m[11] +
                      m[12] * m[7]  * m[10];

            inv[8] = m[4]  * m[9]  * m[15] -
                     m[4]  * m[11] * m[13] -
                     m[8]  * m[5]  * m[15] +
                     m[8]  * m[7]  * m[13] +
                     m[12] * m[5]  * m[11] -
                     m[12] * m[7]  * m[9];

            inv[12] = -m[4]  * m[9]  * m[14] +
                       m[4]  * m[10] * m[13] +
                       m[8]  * m[5]  * m[14] -
                       m[8]  * m[6]  * m[13] -
                       m[12] * m[5]  * m[10] +
                       m[12] * m[6]  * m[9];

            inv[1] = -m[1]  * m[10] * m[15] +
                      m[1]  * m[11] * m[14] +
                      m[9]  * m[2]  * m[15] -
                      m[9]  * m[3]  * m[14] -
                      m[13] * m[2]  * m[11] +
                      m[13] * m[3]  * m[10];

            inv[5] = m[0]  * m[10] * m[15] -
                     m[0]  * m[11] * m[14] -
                     m[8]  * m[2]  * m[15] +
                     m[8]  * m[3]  * m[14] +
                     m[12] * m[2]  * m[11] -
                     m[12] * m[3]  * m[10];

            inv[9] = -m[0]  * m[9]  * m[15] +
                      m[0]  * m[11] * m[13] +
                      m[8]  * m[1]  * m[15] -
                      m[8]  * m[3]  * m[13] -
                      m[12] * m[1]  * m[11] +
                      m[12] * m[3]  * m[9];

            inv[13] = m[0]  * m[9] * m[14] -
                      m[0]  * m[10] * m[13] -
                      m[8]  * m[1] * m[14] +
                      m[8]  * m[2] * m[13] +
                      m[12] * m[1] * m[10] -
                      m[12] * m[2] * m[9];

            inv[2] = m[1]  * m[6] * m[15] -
                     m[1]  * m[7] * m[14] -
                     m[5]  * m[2] * m[15] +
                     m[5]  * m[3] * m[14] +
                     m[13] * m[2] * m[7] -
                     m[13] * m[3] * m[6];

            inv[6] = -m[0]  * m[6] * m[15] +
                      m[0]  * m[7] * m[14] +
                      m[4]  * m[2] * m[15] -
                      m[4]  * m[3] * m[14] -
                      m[12] * m[2] * m[7] +
                      m[12] * m[3] * m[6];

            inv[10] = m[0]  * m[5] * m[15] -
                      m[0]  * m[7] * m[13] -
                      m[4]  * m[1] * m[15] +
                      m[4]  * m[3] * m[13] +
                      m[12] * m[1] * m[7] -
                      m[12] * m[3] * m[5];

            inv[14] = -m[0]  * m[5] * m[14] +
                       m[0]  * m[6] * m[13] +
                       m[4]  * m[1] * m[14] -
                       m[4]  * m[2] * m[13] -
                       m[12] * m[1] * m[6] +
                       m[12] * m[2] * m[5];

            inv[3] = -m[1] * m[6] * m[11] +
                      m[1] * m[7] * m[10] +
                      m[5] * m[2] * m[11] -
                      m[5] * m[3] * m[10] -
                      m[9] * m[2] * m[7] +
                      m[9] * m[3] * m[6];

            inv[7] = m[0] * m[6] * m[11] -
                     m[0] * m[7] * m[10] -
                     m[4] * m[2] * m[11] +
                     m[4] * m[3] * m[10] +
                     m[8] * m[2] * m[7] -
                     m[8] * m[3] * m[6];

            inv[11] = -m[0] * m[5] * m[11] +
                       m[0] * m[7] * m[9] +
                       m[4] * m[1] * m[11] -
                       m[4] * m[3] * m[9] -
                       m[8] * m[1] * m[7] +
                       m[8] * m[3] * m[5];

            inv[15] = m[0] * m[5] * m[10] -
                      m[0] * m[6] * m[9] -
                      m[4] * m[1] * m[10] +
                      m[4] * m[2] * m[9] +
                      m[8] * m[1] * m[6] -
                      m[8] * m[2] * m[5];

            det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

            if (!zero(det))
            {
                mat4_t<T> ret;
                det = 1.0f / det;
                for (i = 0; i < 16; i++) inv[i] *= det;
                ret.set(inv);
                return ret;
                
            } else {
                throw std::runtime_error("SINGULAR");
            }

        }
        
        T det() const
        {
            const T* m = data;
            return (m[3]*m[6]*m[9]*m[12]  - m[2]*m[7]*m[9]*m[12]  - m[3]*m[5]*m[10]*m[12] + m[1]*m[7]*m[10]*m[12] +
                    m[2]*m[5]*m[11]*m[12] - m[1]*m[6]*m[11]*m[12] - m[3]*m[6]*m[8]*m[13]  + m[2]*m[7]*m[8]*m[13] +
                    m[3]*m[4]*m[10]*m[13] - m[0]*m[7]*m[10]*m[13] - m[2]*m[4]*m[11]*m[13] + m[0]*m[6]*m[11]*m[13] +
                    m[3]*m[5]*m[8]*m[14]  - m[1]*m[7]*m[8]*m[14]  - m[3]*m[4]*m[9]*m[14]  + m[0]*m[7]*m[9]*m[14] +
                    m[1]*m[4]*m[11]*m[14] - m[0]*m[5]*m[11]*m[14] - m[2]*m[5]*m[8]*m[15]  + m[1]*m[6]*m[8]*m[15] +
                    m[2]*m[4]*m[9]*m[15]  - m[0]*m[6]*m[9]*m[15]  - m[1]*m[4]*m[10]*m[15] + m[0]*m[5]*m[10]*m[15]);
        }
        
        std::string str ()
        {
            std::stringstream ss;
            for (int i=0 ; i<4 ; ++i) {
                ss << "[";
                ss << this->data[i*4 + 0] << ", ";
                ss << this->data[i*4 + 1] << ", ";
                ss << this->data[i*4 + 2] << ", ";
                ss << this->data[i*4 + 3] << "]";
                ss << std::endl;
            }
            return ss.str();
        }
};

typedef mat4_t<real> mat4;
typedef mat4_t<int>  imat4;
typedef mat4_t<unsigned int> umat4;
typedef mat4_t<float>  fmat4;
typedef mat4_t<double> dmat4;


}
