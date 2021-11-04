#include <cmath>

#ifndef NP_ENGINE_CMATH_INCLUDE
#define NP_ENGINE_CMATH_INCLUDE

#if NP_ENGINE_PLATFORM_IS_LINUX

namespace std
{

//TODO: cleanup this file
//content from https://github.com/thisusernameonlyhasthirtyninecharacters/libstdc-math-functions-update
// and https://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-html-USERS-4.4/a01108.html

// cmath patch libstdc++

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// std::modff and std::modfl

//////////////////////////////////////////////////////////////////////////////// OLD

// #ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
//   inline float
//   modf(float __x, float* __iptr)
//   { return __builtin_modff(__x, __iptr); }

//   inline long double
//   modf(long double __x, long double* __iptr)
//   { return __builtin_modfl(__x, __iptr); }
// #endif

//////////////////////////////////////////////////////////////////////////////// NEW

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline float //TODO: _GLIBCXX_CONSTEXPR??
  modff(float __x, float* __iptr)
  { return __builtin_modff(__x, __iptr); }

  inline long double //TODO: _GLIBCXX_CONSTEXPR??
  modfl(long double __x, long double* __iptr)
  { return __builtin_modfl(__x, __iptr); }
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// std::acosf and std::acosl

//////////////////////////////////////////////////////////////////////////////// OLD

// #ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
//   inline _GLIBCXX_CONSTEXPR float
//   acos(float __x)
//   { return __builtin_acosf(__x); }

//   inline _GLIBCXX_CONSTEXPR long double
//   acos(long double __x)
//   { return __builtin_acosl(__x); }
// #endif

//////////////////////////////////////////////////////////////////////////////// NEW

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  acosf(float __x)
  { return __builtin_acosf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  acosl(long double __x)
  { return __builtin_acosl(__x); }
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// std::asinf and std::asinl

//////////////////////////////////////////////////////////////////////////////// OLD

// #ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
// inline _GLIBCXX_CONSTEXPR float
// asin(float __x)
// { return __builtin_asinf(__x); }

// inline _GLIBCXX_CONSTEXPR long double
// asin(long double __x)
// { return __builtin_asinl(__x); }
// #endif

//////////////////////////////////////////////////////////////////////////////// NEW

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
inline _GLIBCXX_CONSTEXPR float
asinf(float __x)
{ return __builtin_asinf(__x); }

inline _GLIBCXX_CONSTEXPR long double
asinl(long double __x)
{ return __builtin_asinl(__x); }
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// std::atanf and std::atanl -- by nathanphipps

//////////////////////////////////////////////////////////////////////////////// NEW

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
inline _GLIBCXX_CONSTEXPR float
atanf(float __x)
{ return __builtin_atanf(__x); }

inline _GLIBCXX_CONSTEXPR long double
atanl(long double __x)
{ return __builtin_atanl(__x); }
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// std::atan2f and std::atan2l

//////////////////////////////////////////////////////////////////////////////// OLD

// #ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
//   inline _GLIBCXX_CONSTEXPR float
//   atan2(float __y, float __x)
//   { return __builtin_atan2f(__y, __x); }

//   inline _GLIBCXX_CONSTEXPR long double
//   atan2(long double __y, long double __x)
//   { return __builtin_atan2l(__y, __x); }
// #endif

//////////////////////////////////////////////////////////////////////////////// NEW

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  atan2f(float __y, float __x)
  { return __builtin_atan2f(__y, __x); }

  inline _GLIBCXX_CONSTEXPR long double
  atan2l(long double __y, long double __x)
  { return __builtin_atan2l(__y, __x); }
#endif
  
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  
// std::cosf and std::cosl

//////////////////////////////////////////////////////////////////////////////// OLD

// #ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
//   inline _GLIBCXX_CONSTEXPR float
//   cos(float __x)
//   { return __builtin_cosf(__x); }

//   inline _GLIBCXX_CONSTEXPR long double
//   cos(long double __x)
//   { return __builtin_cosl(__x); }
// #endif

//////////////////////////////////////////////////////////////////////////////// NEW

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  cosf(float __x)
  { return __builtin_cosf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  cosl(long double __x)
  { return __builtin_cosl(__x); }
#endif
  
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// std::sinf and std::sinl

//////////////////////////////////////////////////////////////////////////////// OLD

// #ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
//   inline _GLIBCXX_CONSTEXPR float
//   sin(float __x)
//   { return __builtin_sinf(__x); }

//   inline _GLIBCXX_CONSTEXPR long double
//   sin(long double __x)
//   { return __builtin_sinl(__x); }
// #endif

//////////////////////////////////////////////////////////////////////////////// NEW

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  sinf(float __x)
  { return __builtin_sinf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  sinl(long double __x)
  { return __builtin_sinl(__x); }
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// std::tanf and std::tanl

//////////////////////////////////////////////////////////////////////////////// OLD

// #ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
//   inline _GLIBCXX_CONSTEXPR float
//   tan(float __x)
//   { return __builtin_tanf(__x); }

//   inline _GLIBCXX_CONSTEXPR long double
//   tan(long double __x)
//   { return __builtin_tanl(__x); }
// #endif

//////////////////////////////////////////////////////////////////////////////// NEW

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  tanf(float __x)
  { return __builtin_tanf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  tanl(long double __x)
  { return __builtin_tanl(__x); }
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  
  
// std::coshf and std::coshl

//////////////////////////////////////////////////////////////////////////////// OLD

// #ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
//   inline _GLIBCXX_CONSTEXPR float
//   cosh(float __x)
//   { return __builtin_coshf(__x); }

//   inline _GLIBCXX_CONSTEXPR long double
//   cosh(long double __x)
//   { return __builtin_coshl(__x); }
// #endif

//////////////////////////////////////////////////////////////////////////////// NEW

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  coshf(float __x)
  { return __builtin_coshf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  coshl(long double __x)
  { return __builtin_coshl(__x); }
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  
  
// std::sinhf and std::sinhl

//////////////////////////////////////////////////////////////////////////////// OLD

// #ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
//   inline _GLIBCXX_CONSTEXPR float
//   sinh(float __x)
//   { return __builtin_sinhf(__x); }

//   inline _GLIBCXX_CONSTEXPR long double
//   sinh(long double __x)
//   { return __builtin_sinhl(__x); }
// #endif

//////////////////////////////////////////////////////////////////////////////// NEW

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  sinhf(float __x)
  { return __builtin_sinhf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  sinhl(long double __x)
  { return __builtin_sinhl(__x); }
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  
  
// std::tanhf and std::tanhl

//////////////////////////////////////////////////////////////////////////////// OLD

// #ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
//   inline _GLIBCXX_CONSTEXPR float
//   tanh(float __x)
//   { return __builtin_tanhf(__x); }

//   inline _GLIBCXX_CONSTEXPR long double
//   tanh(long double __x)
//   { return __builtin_tanhl(__x); }
// #endif

//////////////////////////////////////////////////////////////////////////////// NEW

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  tanhf(float __x)
  { return __builtin_tanhf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  tanhl(long double __x)
  { return __builtin_tanhl(__x); }
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// std::expf and std::expl

//////////////////////////////////////////////////////////////////////////////// OLD

// #ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
//   inline _GLIBCXX_CONSTEXPR float
//   exp(float __x)
//   { return __builtin_expf(__x); }

//   inline _GLIBCXX_CONSTEXPR long double
//   exp(long double __x)
//   { return __builtin_expl(__x); }
// #endif

//////////////////////////////////////////////////////////////////////////////// NEW

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  expf(float __x)
  { return __builtin_expf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  expl(long double __x)
  { return __builtin_expl(__x); }
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// std::fabsf and std::fabsl

//////////////////////////////////////////////////////////////////////////////// OLD

// #ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
//   inline _GLIBCXX_CONSTEXPR float
//   fabs(float __x)
//   { return __builtin_fabsf(__x); }

//   inline _GLIBCXX_CONSTEXPR long double
//   fabs(long double __x)
//   { return __builtin_fabsl(__x); }
// #endif

//////////////////////////////////////////////////////////////////////////////// NEW

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  fabsf(float __x)
  { return __builtin_fabsf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  fabsl(long double __x)
  { return __builtin_fabsl(__x); }
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// std::frexpf and std::frexpl

//////////////////////////////////////////////////////////////////////////////// OLD

// #ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
//   inline _GLIBCXX_CONSTEXPR float
//   frexp(float __x, int* __exp)
//   { return __builtin_frexpf(__x, __exp); }

//   inline _GLIBCXX_CONSTEXPR long double
//   frexp(long double __x, int* __exp)
//   { return __builtin_frexpl(__x, __exp); }
// #endif

//////////////////////////////////////////////////////////////////////////////// NEW

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  frexpf(float __x, int* __exp)
  { return __builtin_frexpf(__x, __exp); }

  inline _GLIBCXX_CONSTEXPR long double
  frexpl(long double __x, int* __exp)
  { return __builtin_frexpl(__x, __exp); }
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// std::ldexpf and std::ldexpl

//////////////////////////////////////////////////////////////////////////////// OLD

// #ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
//   inline _GLIBCXX_CONSTEXPR float
//   ldexp(float __x, int __exp)
//   { return __builtin_ldexpf(__x, __exp); }

//   inline _GLIBCXX_CONSTEXPR long double
//   ldexp(long double __x, int __exp)
//   { return __builtin_ldexpl(__x, __exp); }
// #endif

//////////////////////////////////////////////////////////////////////////////// NEW

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  ldexpf(float __x, int __exp)
  { return __builtin_ldexpf(__x, __exp); }

  inline _GLIBCXX_CONSTEXPR long double
  ldexpl(long double __x, int __exp)
  { return __builtin_ldexpl(__x, __exp); }
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// std::logf and std::logl

//////////////////////////////////////////////////////////////////////////////// OLD

// #ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
//   inline _GLIBCXX_CONSTEXPR float
//   log(float __x)
//   { return __builtin_logf(__x); }

//   inline _GLIBCXX_CONSTEXPR long double
//   log(long double __x)
//   { return __builtin_logl(__x); }
// #endif

//////////////////////////////////////////////////////////////////////////////// NEW

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  logf(float __x)
  { return __builtin_logf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  logl(long double __x)
  { return __builtin_logl(__x); }
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// std::log10f and std::log10l

//////////////////////////////////////////////////////////////////////////////// OLD

// #ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
//   inline _GLIBCXX_CONSTEXPR float
//   log10(float __x)
//   { return __builtin_log10f(__x); }

//   inline _GLIBCXX_CONSTEXPR long double
//   log10(long double __x)
//   { return __builtin_log10l(__x); }
// #endif

//////////////////////////////////////////////////////////////////////////////// NEW

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  log10f(float __x)
  { return __builtin_log10f(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  log10l(long double __x)
  { return __builtin_log10l(__x); }
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// std::powf and std::powl

//////////////////////////////////////////////////////////////////////////////// OLD

// #ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
//   inline _GLIBCXX_CONSTEXPR float
//   pow(float __x, float __y)
//   { return __builtin_powf(__x, __y); }

//   inline _GLIBCXX_CONSTEXPR long double
//   pow(long double __x, long double __y)
//   { return __builtin_powl(__x, __y); }

// #elif __cplusplus < 201103L
//   // _GLIBCXX_RESOLVE_LIB_DEFECTS
//   // DR 550. What should the return type of pow(float,int) be?
//   inline double
//   pow(double __x, int __i)
//   { return __builtin_powi(__x, __i); }

//   inline float
//   pow(float __x, int __n)
//   { return __builtin_powif(__x, __n); }

//   inline long double
//   pow(long double __x, int __n)
//   { return __builtin_powil(__x, __n); }
// #endif

//////////////////////////////////////////////////////////////////////////////// NEW

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  powf(float __x, float __y)
  { return __builtin_powf(__x, __y); }

  inline _GLIBCXX_CONSTEXPR long double
  powl(long double __x, long double __y)
  { return __builtin_powl(__x, __y); }

#elif __cplusplus < 201103L
  // _GLIBCXX_RESOLVE_LIB_DEFECTS
  // DR 550. What should the return type of pow(float,int) be?
  inline double
  powi(double __x, int __i)
  { return __builtin_powi(__x, __i); }

  inline float
  powif(float __x, int __n)
  { return __builtin_powif(__x, __n); }

  inline long double
  powil(long double __x, int __n)
  { return __builtin_powil(__x, __n); }
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// std::sqrtf and std::sqrtl

//////////////////////////////////////////////////////////////////////////////// OLD

// #ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
//   inline _GLIBCXX_CONSTEXPR float
//   sqrt(float __x)
//   { return __builtin_sqrtf(__x); }

//   inline _GLIBCXX_CONSTEXPR long double
//   sqrt(long double __x)
//   { return __builtin_sqrtl(__x); }
// #endif

//////////////////////////////////////////////////////////////////////////////// NEW

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  sqrtf(float __x)
  { return __builtin_sqrtf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  sqrtl(long double __x)
  { return __builtin_sqrtl(__x); }
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// std::ceilf and std::ceill

//////////////////////////////////////////////////////////////////////////////// OLD

// #ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
//   inline _GLIBCXX_CONSTEXPR float
//   ceil(float __x)
//   { return __builtin_ceilf(__x); }

//   inline _GLIBCXX_CONSTEXPR long double
//   ceil(long double __x)
//   { return __builtin_ceill(__x); }
// #endif

//////////////////////////////////////////////////////////////////////////////// NEW

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  ceilf(float __x)
  { return __builtin_ceilf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  ceill(long double __x)
  { return __builtin_ceill(__x); }
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// std::floorf and std::floorl

//////////////////////////////////////////////////////////////////////////////// OLD

// #ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
//   inline _GLIBCXX_CONSTEXPR float
//   floor(float __x)
//   { return __builtin_floorf(__x); }

//   inline _GLIBCXX_CONSTEXPR long double
//   floor(long double __x)
//   { return __builtin_floorl(__x); }
// #endif

//////////////////////////////////////////////////////////////////////////////// NEW

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  floorf(float __x)
  { return __builtin_floorf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  floorl(long double __x)
  { return __builtin_floorl(__x); }
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// std::fmodf and std::fmodl

//////////////////////////////////////////////////////////////////////////////// OLD

// #ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
//   inline _GLIBCXX_CONSTEXPR float
//   fmod(float __x, float __y)
//   { return __builtin_fmodf(__x, __y); }

//   inline _GLIBCXX_CONSTEXPR long double
//   fmod(long double __x, long double __y)
//   { return __builtin_fmodl(__x, __y); }
// #endif

//////////////////////////////////////////////////////////////////////////////// NEW

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  fmodf(float __x, float __y)
  { return __builtin_fmodf(__x, __y); }

  inline _GLIBCXX_CONSTEXPR long double
  fmodl(long double __x, long double __y)
  { return __builtin_fmodl(__x, __y); }
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

}

#endif
#endif