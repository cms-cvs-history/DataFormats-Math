#ifndef DataFormat_Math_SSEVec_H
#define DataFormat_Math_SSEVec_H

#if defined(__GNUC__) && (__GNUC__ == 4) && (__GNUC_MINOR__ > 4)
#include <x86intrin.h>
#define CMS_USE_SSE
#ifdef __AVX__
#define CMS_USE_AVX
#endif
#else

#ifdef __SSE2__
#define CMS_USE_SSE

#include <mmintrin.h>
#include <emmintrin.h>
#endif
#ifdef __SSE3__
#include <pmmintrin.h>
#endif
#ifdef __SSE4_1__
#include <smmintrin.h>
#endif

#endif

#include<cmath>

namespace mathSSE {
  template<typename T> inline T sqrt(T t) { return std::sqrt(t);}
}

namespace mathSSE {
  //
  template<typename T> inline bool samesign(T rh, T lh);

  template<>
  inline bool
  __attribute__((always_inline)) __attribute__ ((pure)) samesign<int>(int rh, int lh) {
    int const mask= 0x80000000;
    return ((rh^lh)&mask) == 0;
  }

  template<>
  inline bool
  __attribute__((always_inline)) __attribute__ ((pure)) samesign<long long>(long long rh, long long lh) {
    long long const mask= 0x8000000000000000LL;
    return ((rh^lh)&mask) == 0;
  }

  template<>
  inline bool
  __attribute__((always_inline)) __attribute__ ((pure)) samesign<float>(float rh, float lh) {
    union { int i; float f; } a, b;
    a.f=rh; b.f=lh;
    return samesign<int>(a.i,b.i);
  }

  template<>
  inline bool
  __attribute__((always_inline)) __attribute__ ((pure)) samesign<double>(double rh, double lh) {
    union { long long i; double f; } a, b;
    a.f=rh; b.f=lh;
    return samesign<long long>(a.i,b.i);
  }
}


namespace mathSSE {
#ifdef  CMS_USE_SSE
  //dot
  inline __m128 _mm_dot_ps(__m128 v1, __m128 v2) {
#ifdef __SSE4_1__
    return _mm_dp_ps(v1, v2, 0xff);
#else
    __m128 mul = _mm_mul_ps(v1, v2);
#ifdef __SSE3__
    mul = _mm_hadd_ps(mul,mul);
    return _mm_hadd_ps(mul,mul);
#else
    __m128 swp = _mm_shuffle_ps(mul, mul, _MM_SHUFFLE(1, 0, 3, 2));
    mul = _mm_add_ps(mul, swp);
    swp = _mm_shuffle_ps(mul, mul, _MM_SHUFFLE(2, 3, 0, 1));
    return _mm_add_ps(mul, swp);
#endif
#endif
  }
  

  // cross (just 3x3) 
  inline __m128 _mm_cross_ps(__m128 v1, __m128 v2) {
    // same order is  _MM_SHUFFLE(3,2,1,0)
    //                                               x2, z1,z1
    __m128 v3 = _mm_shuffle_ps(v2, v1, _MM_SHUFFLE(3, 0, 2, 2));
    //                                               y1, x2,y1
    __m128 v4 = _mm_shuffle_ps(v1, v2, _MM_SHUFFLE(3, 1, 0, 1));
    
    __m128 v5 = _mm_mul_ps(v3, v4);
    
    //                                         x1, z2,z2
    v3 = _mm_shuffle_ps(v1, v2, _MM_SHUFFLE(3, 0, 2, 2));
    //                                        y2, x1,y1
    v4 = _mm_shuffle_ps(v2, v1, _MM_SHUFFLE(3, 1, 0, 1));
    
    v3 = _mm_mul_ps(v3, v4);
    const  __m128 neg = _mm_set_ps(0.0f,0.0f,-0.0f,0.0f);
    return _mm_xor_ps(_mm_sub_ps(v5, v3), neg);
  }
#endif // CMS_USE_SSE

#ifdef  CMS_USE_AVX
  inline __m256d _mm256_dot_pd(__m256d v1, __m256d v2) {
    __m256d mul = _mm256_mul_pd(v1, v2);
    mul = _mm256_hadd_pd(mul,mul);
    return _mm256_hadd_pd(mul,mul);
  }

  inline __m256d _mm256_cross_pd(__m256d v1, __m256d v2) {
    
    __m256d v3 = _mm256_permute2f128_pd(v2, v1, (2<<4)+1);
    v3 = _mm256_permute_pd(v3,0);
    
    __m256d v4 = _mm256_permute2f128_pd(v1, v2, (2<<4));
    v4 = _mm256_permute_pd(v4,5);
    
    __m256d v5 = _mm256_mul_pd(v3, v4);
    
    v3 = _mm256_permute2f128_pd(v1, v2, (2<<4)+1);
    v3 = _mm256_permute_pd(v3,0);
    
    v4 = _mm256_permute2f128_pd(v2, v1, (2<<4));
    v4 = _mm256_permute_pd(v4,5);
    
    v3 = _mm256_mul_pd(v3, v4);
    const  __m256d neg = _mm256_set_pd(0.0,0.0,-0.0,0.0);
    return _mm256_xor_pd(_mm256_sub_pd(v5, v3), neg);
 
  }

#endif //  CMS_USE_AVX



  template<typename T>
  struct OldVec { T  theX; T  theY; T  theZ; T  theW;}  __attribute__ ((aligned (16)));
#ifdef  CMS_USE_AVX
  template<>
  struct OldVec<double> { double  theX; double  theY; double  theZ; double  theW;}  __attribute__ ((aligned (32)));
#endif
  
  template<typename T> union Vec4;

  template<typename T> union Vec2{
    Vec2() {
      arr[0] = 0; arr[1] = 0;
    }
    Vec2(T f1, T f2) {
      arr[0] = f1; arr[1] = f2;
    }
    explicit Vec2(T f1) {
      arr[0] = f1; arr[1] = f1;
    }

    void set(T f1, T f2) {
      arr[0] = f1; arr[1] = f2;
    }
    Vec2 get1(unsigned int n) const {
      return Vec2(arr[n],arr[n]);
    }

    template<typename U> 
    Vec2(Vec2<U> v) {
      arr[0] = v[0]; arr[1] = v[1];

    }

    inline Vec2(Vec4<T> v4);

    T & operator[](unsigned int n) {
      return arr[n];
    }

    T operator[](unsigned int n) const {
      return arr[n];
    }


    T arr[2];
  };


  template<typename T> union Vec4{
    Vec4() {
      arr[0] = 0; arr[1] = 0; arr[2] = 0; arr[3]=0;
    }
    Vec4(float f1, float f2, float f3, float f4=0) {
      arr[0] = f1; arr[1] = f2; arr[2] = f3; arr[3]=f4;
    }
    explicit Vec4(float f1) {
      set1(f1);
    }
    void set(float f1, float f2, float f3, float f4=0) {
      arr[0] = f1; arr[1] = f2; arr[2] = f3; arr[3]=f4;
    }
    void set1(float f1) {
      arr[0] = f1; arr[1] = f1; arr[2] = f1; arr[3]=f1;
    }
    Vec4 get1(unsigned int n) const {
      return Vec4(arr[n],arr[n],arr[n],arr[n]);
    }

    Vec2<T> xy() const { return  Vec2<T>(arr[0],arr[1]);}
    Vec2<T> zw() const { return  Vec2<T>(arr[2],arr[3]);}



    T __attribute__ ((aligned(16))) arr[4];
    OldVec<T> o;
  };

  template<typename T>
  inline Vec2<T>::Vec2(Vec4<T> v4) {
    arr[0]=v4[0];arr[1]=v4[1];
  }


#ifdef CMS_USE_SSE

  template<>
  union Vec2<double>;
  template<>
  union Vec4<double>;

  template<typename T> union Mask2{};
  template<typename T> union Mask4{};

  template<>
  union Mask4<float> {
    __m128 vec;
    unsigned int __attribute__ ((aligned(16))) mask[4];
    Mask4() {vec = _mm_setzero_ps();}
    Mask4(unsigned int m1, unsigned int m2, unsigned int m3, unsigned int m4) {
      mask[0]=m1;  mask[1]=m2;  mask[2]=m3;  mask[3]=m4; 
    }
  };
  
#ifdef  CMS_USE_AVX
  template<>
  union Mask4<double> {
    __m256d vec;
    unsigned long long __attribute__ ((aligned(32))) mask[4];
    Mask4() {
      vec = _mm256_setzero_pd();
     }
    Mask4(unsigned long long m1, unsigned long long m2, unsigned long long m3, unsigned long long m4) {
      mask[0]=m1;  mask[1]=m2;  mask[2]=m3;  mask[3]=m4; 
    }
  };
#else
  template<>
  union Mask4<double> {
    __m128d vec[2];
    unsigned long long __attribute__ ((aligned(16))) mask[4];
    Mask4() {
      vec[0] = _mm_setzero_pd();
      vec[1] = _mm_setzero_pd();
    }
    Mask4(unsigned long long m1, unsigned long long m2, unsigned long long m3, unsigned long long m4) {
      mask[0]=m1;  mask[1]=m2;  mask[2]=m3;  mask[3]=m4; 
    }
  };
#endif

  template<>
  union Mask2<double> {
    __m128d vec;
    unsigned long long __attribute__ ((aligned(16))) mask[2];
    Mask2() {
      vec = _mm_setzero_pd();
    }
    Mask2(unsigned long long m1, unsigned long long m2) {
      mask[0]=m1;  mask[1]=m2; 
    }
  };

  template<>
  union Vec4<float> {
    typedef  __m128 nativeType;
    __m128 vec;
    float __attribute__ ((aligned(16))) arr[4];
    OldVec<float> o;
    
    Vec4(__m128 ivec) : vec(ivec) {}

    Vec4(OldVec<float> const & ivec) : o(ivec) {}
    
    Vec4() {
      vec = _mm_setzero_ps();
    }


    inline Vec4(Vec4<double> ivec);

    explicit Vec4(float f1) {
      set1(f1);
    }

    Vec4(float f1, float f2, float f3, float f4=0) {
      arr[0] = f1; arr[1] = f2; arr[2] = f3; arr[3]=f4;
    }


    Vec4( Vec2<float> ivec0,   Vec2<float> ivec1) {
      arr[0] = ivec0.arr[0]; arr[1]=ivec0.arr[1];
      arr[2] = ivec1.arr[0]; arr[3]=ivec1.arr[1];
    }
    
    Vec4( Vec2<float> ivec0,  float f3, float f4=0) {
      arr[0] = ivec0.arr[0]; arr[1]=ivec0.arr[1];
      arr[2] = f3; arr[3] = f4;
    }

   Vec4( Vec2<float> ivec0) {
     vec = _mm_setzero_ps();
     arr[0] = ivec0.arr[0]; arr[1]=ivec0.arr[1];
    }


    // for masking
    void setMask(unsigned int m1, unsigned int m2, unsigned int m3, unsigned int m4) {
      Mask4<float> mask(m1,m2,m3,m4); vec=mask.vec; 
    }

    void set(float f1, float f2, float f3, float f4=0) {
      vec = _mm_set_ps(f4, f3, f2, f1);
    }

    void set1(float f1) {
     vec =  _mm_set1_ps(f1);
    }

    Vec4 get1(unsigned int n) const { 
      return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(n, n, n, n)); 
    }

    float & operator[](unsigned int n) {
      return arr[n];
    }

    float operator[](unsigned int n) const {
      return arr[n];
    }
    
    Vec2<float> xy() const { return  Vec2<float>(arr[0],arr[1]);}
    Vec2<float> zw() const { return  Vec2<float>(arr[2],arr[3]);}

  };
  

  template<>
  union Vec2<double> {
    typedef  __m128d nativeType;
    __m128d vec;
    double __attribute__ ((aligned(16))) arr[2];
        
    Vec2(__m128d ivec) : vec(ivec) {}
    
    Vec2() {
      vec = _mm_setzero_pd();
    }


    inline Vec2(Vec2<float> ivec);


    Vec2(double f1, double f2) {
      arr[0] = f1; arr[1] = f2;
    }

    explicit Vec2(double f1) {
      set1(f1);
    }
    
    inline Vec2(Vec4<double> v4); 

   // for masking
   void setMask(unsigned long long m1, unsigned long long m2) {
     Mask2<double> mask(m1,m2); vec=mask.vec; 
   }


    void set(double f1, double f2) {
      arr[0] = f1; arr[1] = f2;
    }

    void set1(double f1) {
      vec = _mm_set1_pd(f1);
    }

    Vec2 get1(unsigned int n) const {
      return Vec2(arr[n],arr[n]);
    }
   
    double operator[](unsigned int n) const {
      return arr[n];
    }
  };
 



#ifdef  CMS_USE_AVX
}// namespace mathSSE
#include "AVXVec.h"

namespace mathSSE {
#else
  template<>
  union Vec4<double> {
    __m128d vec[2];
    double __attribute__ ((aligned(16))) arr[4];
    OldVec<double> o;
    
    Vec4(__m128d ivec[]) {
      vec[0] = ivec[0];
      vec[1] = ivec[1];
    }
    
    Vec4(__m128d ivec0, __m128d ivec1) {
      vec[0] = ivec0;
      vec[1] = ivec1;
    }
    
    Vec4() {
      vec[0] = _mm_setzero_pd();
      vec[1] = _mm_setzero_pd();
    }

    Vec4( Vec4<float> ivec) {
      vec[0] = _mm_cvtps_pd(ivec.vec);
      vec[1] = _mm_cvtps_pd(_mm_shuffle_ps(ivec.vec, ivec.vec, _MM_SHUFFLE(1, 0, 3, 2)));
    }

    explicit Vec4(double f1) {
      set1(f1);
    }

    Vec4(double f1, double f2, double f3, double f4=0) {
      arr[0] = f1; arr[1] = f2; arr[2] = f3; arr[3]=f4;
    }
    
   Vec4( Vec2<double> ivec0,   Vec2<double> ivec1) {
      vec[0] = ivec0.vec;
      vec[1] = ivec1.vec;
    }
    
    Vec4( Vec2<double> ivec0,  double f3, double f4=0) {
      vec[0] = ivec0.vec;
      arr[2] = f3; arr[3] = f4;
    }

   Vec4( Vec2<double> ivec0) {
      vec[0] = ivec0.vec;
      vec[1] =  _mm_setzero_pd();
    }


    Vec4(OldVec<double> const & ivec) : o(ivec) {}

    // for masking
    void setMask(unsigned long long m1, unsigned long long m2, unsigned long long m3, unsigned long long m4) {
      Mask4<double> mask(m1,m2,m3,m4); vec[0]=mask.vec[0]; vec[1]=mask.vec[1]; 
    }


    void set(double f1, double f2, double f3, double f4=0) {
      arr[0] = f1; arr[1] = f2; arr[2] = f3; arr[3]=f4;
    }

    void set1(double f1) {
      vec[0] = vec[1]= _mm_set1_pd(f1);
    }


    Vec4 get1(unsigned int n) const {
      return Vec4(arr[n],arr[n],arr[n],arr[n]);
    }

    double & operator[](unsigned int n) {
      return arr[n];
    }

    double operator[](unsigned int n) const {
      return arr[n];
    }
  
    Vec2<double> xy() const { return vec[0];}
    Vec2<double> zw() const { return vec[1];}

  };

  
  inline Vec4<float>::Vec4(Vec4<double> ivec) {
    vec = _mm_cvtpd_ps(ivec.vec[0]);
    __m128 v2 = _mm_cvtpd_ps(ivec.vec[1]);
    vec = _mm_shuffle_ps(vec, v2, _MM_SHUFFLE(1, 0, 1, 0));
  }
#endif  // CMS_USE_AVX


#endif // CMS_USE_SSE
  
  typedef Vec2<float> Vec2F;
  typedef Vec4<float> Vec4F;
  typedef Vec4<float> Vec3F;
  typedef Vec2<double> Vec2D;
  typedef Vec4<double> Vec3D;
  typedef Vec4<double> Vec4D;

  template<typename T>
  struct As3D {
    Vec4<T> const & v;
    As3D(Vec4<T> const &iv ) : v(iv){}
  };

  template<typename T>
  inline As3D<T> as3D(Vec4<T> const &v ) { return v;}

} // namespace mathSSE

#ifdef CMS_USE_SSE


//float op

inline float dot(mathSSE::Vec4F a, mathSSE::Vec4F b) {
  using  mathSSE::_mm_dot_ps;
  float s;
  _mm_store_ss(&s,_mm_dot_ps(a.vec,b.vec));
  return s;
}

inline mathSSE::Vec4F cross(mathSSE::Vec4F a, mathSSE::Vec4F b) {
  using  mathSSE::_mm_cross_ps;
  return _mm_cross_ps(a.vec,b.vec);
}


inline bool operator==(mathSSE::Vec4F a, mathSSE::Vec4F b) {
  return _mm_movemask_ps(_mm_cmpeq_ps(a.vec,b.vec))==0xf;
}

inline mathSSE::Vec4F cmpeq(mathSSE::Vec4F a, mathSSE::Vec4F b) {
  return _mm_cmpeq_ps(a.vec,b.vec);
}

inline mathSSE::Vec4F cmpgt(mathSSE::Vec4F a, mathSSE::Vec4F b) {
  return _mm_cmpgt_ps(a.vec,b.vec);
}

#ifdef __SSE3__
inline mathSSE::Vec4F hadd(mathSSE::Vec4F a, mathSSE::Vec4F b) {
  return _mm_hadd_ps(a.vec,b.vec);
}
#endif


inline mathSSE::Vec4F operator-(mathSSE::Vec4F a) {
  const __m128 neg = _mm_set_ps ( -0.0 , -0.0 , -0.0, -0.0);
  return _mm_xor_ps(a.vec,neg);
}

inline mathSSE::Vec4F operator&(mathSSE::Vec4F a, mathSSE::Vec4F b) {
  return  _mm_and_ps(a.vec,b.vec);
}
inline mathSSE::Vec4F operator|(mathSSE::Vec4F a, mathSSE::Vec4F b) {
  return  _mm_or_ps(a.vec,b.vec);
}
inline mathSSE::Vec4F operator^(mathSSE::Vec4F a, mathSSE::Vec4F b) {
  return  _mm_xor_ps(a.vec,b.vec);
}
inline mathSSE::Vec4F andnot(mathSSE::Vec4F a, mathSSE::Vec4F b) {
  return  _mm_andnot_ps(a.vec,b.vec);
}


inline mathSSE::Vec4F operator+(mathSSE::Vec4F a, mathSSE::Vec4F b) {
  return  _mm_add_ps(a.vec,b.vec);
}

inline mathSSE::Vec4F operator-(mathSSE::Vec4F a, mathSSE::Vec4F b) {
  return  _mm_sub_ps(a.vec,b.vec);
}

inline mathSSE::Vec4F operator*(mathSSE::Vec4F a, mathSSE::Vec4F b) {
  return  _mm_mul_ps(a.vec,b.vec);
}

inline mathSSE::Vec4F operator/(mathSSE::Vec4F a, mathSSE::Vec4F b) {
  return  _mm_div_ps(a.vec,b.vec);
}

inline mathSSE::Vec4F operator*(float a, mathSSE::Vec4F b) {
  return  _mm_mul_ps(_mm_set1_ps(a),b.vec);
}

inline mathSSE::Vec4F operator*(mathSSE::Vec4F b,float a) {
  return  _mm_mul_ps(_mm_set1_ps(a),b.vec);
}

//
// float op 2d (use the 4d one...)
//
inline mathSSE::Vec4F operator-(mathSSE::Vec2F a) {
  return -mathSSE::Vec4F(a);
}


inline mathSSE::Vec4F operator+(mathSSE::Vec2F a, mathSSE::Vec2F b) {
  return  mathSSE::Vec4F(a)+mathSSE::Vec4F(b);
}

inline mathSSE::Vec4F operator-(mathSSE::Vec2F a, mathSSE::Vec2F b) {
  return  mathSSE::Vec4F(a)-mathSSE::Vec4F(b);
}

inline mathSSE::Vec4F operator*(mathSSE::Vec2F a, mathSSE::Vec2F b) {
  return  mathSSE::Vec4F(a)*mathSSE::Vec4F(b);
}

inline mathSSE::Vec4F operator/(mathSSE::Vec2F a, mathSSE::Vec2F b) {
  return  mathSSE::Vec4F(a)/mathSSE::Vec4F(b);
}


inline mathSSE::Vec4F operator*(mathSSE::Vec2F a, float s) {
  return  s*mathSSE::Vec4F(a);
}

inline mathSSE::Vec4F operator*(float s,mathSSE::Vec2F a) {
  return  s*mathSSE::Vec4F(a);
}


inline float dot(mathSSE::Vec2F a, mathSSE::Vec2F b)  __attribute__((always_inline)) __attribute__ ((pure));

inline float dot(mathSSE::Vec2F a, mathSSE::Vec2F b){
  return a.arr[0]*b.arr[0] + a.arr[1]*b.arr[1];
}

inline float cross(mathSSE::Vec2F a, mathSSE::Vec2F b)  __attribute__((always_inline)) __attribute__ ((pure));

inline float cross(mathSSE::Vec2F a, mathSSE::Vec2F b) {
  return a.arr[0]*b.arr[1] - a.arr[1]*b.arr[0];
}


///
// double op 2d
//

inline  mathSSE::Vec2D::Vec2(Vec4D v4) {
#ifdef  CMS_USE_AVX
  vec = _mm256_castpd256_pd128(v4.vec));
#else
  vec = v4.arr[0];
#endif
}

inline  mathSSE::Vec2D::Vec2(Vec2F ivec) {
  arr[0] = ivec.arr[0]; arr[1] = ivec.arr[1];
}

inline mathSSE::Vec2D operator-(mathSSE::Vec2D a) {
  const __m128d neg = _mm_set_pd ( -0.0 , -0.0);
  return _mm_xor_pd(a.vec,neg);
}


inline mathSSE::Vec2D operator&(mathSSE::Vec2D a, mathSSE::Vec2D b) {
  return  _mm_and_pd(a.vec,b.vec);
}
inline mathSSE::Vec2D operator|(mathSSE::Vec2D a, mathSSE::Vec2D b) {
  return  _mm_or_pd(a.vec,b.vec);
}
inline mathSSE::Vec2D operator^(mathSSE::Vec2D a, mathSSE::Vec2D b) {
  return  _mm_xor_pd(a.vec,b.vec);
}
inline mathSSE::Vec2D andnot(mathSSE::Vec2D a, mathSSE::Vec2D b) {
  return  _mm_andnot_pd(a.vec,b.vec);
}


inline mathSSE::Vec2D operator+(mathSSE::Vec2D a, mathSSE::Vec2D b) {
  return  _mm_add_pd(a.vec,b.vec);
}

inline mathSSE::Vec2D operator-(mathSSE::Vec2D a, mathSSE::Vec2D b) {
  return  _mm_sub_pd(a.vec,b.vec);
}

inline mathSSE::Vec2D operator*(mathSSE::Vec2D a, mathSSE::Vec2D b) {
  return  _mm_mul_pd(a.vec,b.vec);
}

inline mathSSE::Vec2D operator/(mathSSE::Vec2D a, mathSSE::Vec2D b) {
  return  _mm_div_pd(a.vec,b.vec);
}

inline mathSSE::Vec2D operator*(double a, mathSSE::Vec2D b) {
  return  _mm_mul_pd(_mm_set1_pd(a),b.vec);
}

inline mathSSE::Vec2D operator*(mathSSE::Vec2D b,double a) {
  return  _mm_mul_pd(_mm_set1_pd(a),b.vec);
}

inline double dot(mathSSE::Vec2D a, mathSSE::Vec2D b)  __attribute__((always_inline)) __attribute__ ((pure));

inline double dot(mathSSE::Vec2D a, mathSSE::Vec2D b){
  __m128d res = _mm_mul_pd ( a.vec, b.vec);
  res = _mm_add_sd (  _mm_shuffle_pd ( res , res, 1 ), res );
  double s;
  _mm_store_sd(&s,res);
  return s;
}

inline double cross(mathSSE::Vec2D a, mathSSE::Vec2D b)  __attribute__((always_inline)) __attribute__ ((pure));

inline double cross(mathSSE::Vec2D a, mathSSE::Vec2D b) {
  __m128d res =  _mm_shuffle_pd ( b.vec, b.vec, 1);
  res = _mm_mul_pd (  a.vec , res );
  res = _mm_sub_sd (res, _mm_shuffle_pd ( res , res, 1 ));
  double s;
  _mm_store_sd(&s,res);
  return s;
}


#ifndef  CMS_USE_AVX
// double op 3d

inline bool operator==(mathSSE::Vec4D a, mathSSE::Vec4D b) {
  return 
    _mm_movemask_pd(_mm_cmpeq_pd(a.vec[0],b.vec[0]))==0x3 && 
    _mm_movemask_pd(_mm_cmpeq_pd(a.vec[1],b.vec[1]))==0x3 ;
}

inline mathSSE::Vec4D operator-(mathSSE::Vec4D a) {
  const __m128d neg = _mm_set_pd ( -0.0 , -0.0);
  return mathSSE::Vec4D(_mm_xor_pd(a.vec[0],neg),_mm_xor_pd(a.vec[1],neg));
}


inline mathSSE::Vec4D operator+(mathSSE::Vec4D a, mathSSE::Vec4D b) {
  return  mathSSE::Vec4D(_mm_add_pd(a.vec[0],b.vec[0]),_mm_add_pd(a.vec[1],b.vec[1]));
}


inline mathSSE::Vec4D operator-(mathSSE::Vec4D a, mathSSE::Vec4D b) {
  return  mathSSE::Vec4D(_mm_sub_pd(a.vec[0],b.vec[0]),_mm_sub_pd(a.vec[1],b.vec[1]));
}
inline mathSSE::Vec4D operator*(mathSSE::Vec4D a, mathSSE::Vec4D b) {
  return  mathSSE::Vec4D(_mm_mul_pd(a.vec[0],b.vec[0]),_mm_mul_pd(a.vec[1],b.vec[1]));
}
inline mathSSE::Vec4D operator/(mathSSE::Vec4D a, mathSSE::Vec4D b) {
  return  mathSSE::Vec4D(_mm_div_pd(a.vec[0],b.vec[0]),_mm_div_pd(a.vec[1],b.vec[1]));
}

inline mathSSE::Vec4D operator*(double a, mathSSE::Vec4D b) {
  __m128d res = _mm_set1_pd(a);
  return  mathSSE::Vec4D(_mm_mul_pd(res,b.vec[0]),_mm_mul_pd(res,b.vec[1]));
}

inline mathSSE::Vec4D operator*(mathSSE::Vec4D b, double a) {
  __m128d res = _mm_set1_pd(a);
  return  mathSSE::Vec4D(_mm_mul_pd(res,b.vec[0]),_mm_mul_pd(res,b.vec[1]));
}

// mix algebra (creates ambiguities...)
/*
inline mathSSE::Vec4D operator+(mathSSE::Vec4D a, mathSSE::Vec4F b) {
  return a + mathSSE::Vec4D(b);
}
inline mathSSE::Vec4D operator+(mathSSE::Vec4D b, mathSSE::Vec4F a) {
  return a + mathSSE::Vec4D(b);
}
inline mathSSE::Vec4D operator-(mathSSE::Vec4D a, mathSSE::Vec4F b) {
  return a - mathSSE::Vec4D(b);
}
inline mathSSE::Vec4D operator-(mathSSE::Vec4D b, mathSSE::Vec4F a) {
  return a - mathSSE::Vec4D(b);
}
*/

inline double dot(mathSSE::Vec4D a, mathSSE::Vec4D b) __attribute__((always_inline)) __attribute__ ((pure));

inline double dot(mathSSE::Vec4D a, mathSSE::Vec4D b) {
  __m128d res = _mm_add_sd ( _mm_mul_pd ( a.vec[0], b.vec[0]),
			     _mm_mul_sd ( a.vec[1], b.vec[1]) 
			     );
  res = _mm_add_sd ( _mm_unpackhi_pd ( res , res ), res );
  double s;
  _mm_store_sd(&s,res);
  return s;
}

inline mathSSE::Vec4D cross(mathSSE::Vec4D a, mathSSE::Vec4D b) __attribute__((always_inline)) __attribute__ ((pure));
 
inline mathSSE::Vec4D cross(mathSSE::Vec4D a, mathSSE::Vec4D b) {
  const __m128d neg = _mm_set_pd ( 0.0 , -0.0 );
  // lh .z * rh .x , lh .z * rh .y
  __m128d l1 = _mm_mul_pd ( _mm_unpacklo_pd ( a.vec[1] , a.vec[1] ), b.vec[0] );
  // rh .z * lh .x , rh .z * lh .y
  __m128d l2 = _mm_mul_pd ( _mm_unpacklo_pd (  b.vec[1],  b.vec[1] ),  a.vec[0] );
  __m128d m1 = _mm_sub_pd ( l1 , l2 ); // l1 - l2
  m1 = _mm_shuffle_pd ( m1 , m1 , 1 ); // switch the elements
  m1 = _mm_xor_pd ( m1 , neg ); // change the sign of the first element
  // lh .x * rh .y , lh .y * rh .x
  l1 = _mm_mul_pd (  a.vec[0] , _mm_shuffle_pd (  b.vec[0] ,  b.vec[0] , 1 ) );
  // lh .x * rh .y - lh .y * rh .x
  __m128d m2 = _mm_sub_sd ( l1 , _mm_unpackhi_pd ( l1 , l1 ) );

  return  mathSSE::Vec4D( m1 , m2 );
}

#endif   // CMS_USE_AVX


// sqrt
namespace mathSSE {
  template<> inline Vec4F sqrt(Vec4F v) { return _mm_sqrt_ps(v.vec);}
  template<> inline Vec2F sqrt(Vec2F v) { return sqrt(Vec4F(v));}
  template<> inline Vec2D sqrt(Vec2D v) { return _mm_sqrt_pd(v.vec);}
#ifdef  CMS_USE_AVX
  template<> inline Vec4D sqrt(Vec4D v) { return _mm256_sqrt_pd(v.vec);}
#else
  template<> inline Vec4D sqrt(Vec4D v) { 
    return Vec4D(_mm_sqrt_pd(v.vec[0]),_mm_sqrt_pd(v.vec[1]));
  }
#endif
}

// chephes func
#include "DataFormats/Math/interface/sse_mathfun.h"
namespace mathSSE {
  inline Vec4F log(Vec4F v) { return log_ps(v.vec);}
  inline Vec4F exp(Vec4F v) { return exp_ps(v.vec);}
  inline Vec4F sin(Vec4F v) { return sin_ps(v.vec);}
  inline Vec4F cos(Vec4F v) { return cos_ps(v.vec);}
  inline void sincos(Vec4F v, Vec4F & s, Vec4F & c) { sincos_ps(v.vec,&s.vec, &c.vec);}

  inline float log(float f) { float s; _mm_store_ss(&s,log_ps(_mm_load_ss(&f))); return s;}
  inline float exp(float f) { float s; _mm_store_ss(&s,exp_ps(_mm_load_ss(&f))); return s;}
  inline float sin(float f) { float s; _mm_store_ss(&s,sin_ps(_mm_load_ss(&f))); return s;}
  inline float cos(float f) { float s; _mm_store_ss(&s,log_ps(_mm_load_ss(&f))); return s;}
  inline void sincos(float f, float & s, float & c) { 
    __m128 vs, vc; 
    sincos_ps(_mm_load_ss(&f),&vs, &vc);   
    _mm_store_ss(&s,vs);_mm_store_ss(&c,vc);   
  }
}
#endif // CMS_USE_SSE


#include <iosfwd>
std::ostream & operator<<(std::ostream & out, mathSSE::Vec2D const & v);
std::ostream & operator<<(std::ostream & out, mathSSE::Vec2F const & v);
std::ostream & operator<<(std::ostream & out, mathSSE::Vec4F const & v);
std::ostream & operator<<(std::ostream & out, mathSSE::Vec4D const & v);

std::ostream & operator<<(std::ostream & out, mathSSE::As3D<float> const & v);
std::ostream & operator<<(std::ostream & out, mathSSE::As3D<double> const & v);


#endif // DataFormat_Math_SSEVec_H
