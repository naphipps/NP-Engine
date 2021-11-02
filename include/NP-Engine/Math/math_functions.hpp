//
//  math_functions.hpp
//  Project Space
//
//  Created by Nathan Phipps on 5/16/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

//TODO: refactor all these "math_functions" files to Pascal casing instead of snake casing

#ifndef NP_ENGINE_MATH_FUNCTIONS_HPP
#define NP_ENGINE_MATH_FUNCTIONS_HPP

#include <cmath>
#include <algorithm>

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np
{
    namespace math
    {
        /**
         isnan
         */
        static inline bl isnan(const flt n)
        {
            return ::std::isnan(n);
        }
        
        /**
         isnan
         */
        static inline bl isnan(const dbl n)
        {
            return ::std::isnan(n);
        }
        
        /**
         std abs
         */
        static inline i8 abs(const i8 n)
        {
            return ::std::abs(n);
        }
        
        /**
         std abs
         */
        static inline i16 abs(const i16 n)
        {
            return ::std::abs(n);
        }
        
        /**
         std abs
         */
        static inline i32 abs(const i32 n)
        {
            return ::std::abs(n);
        }
        
        /**
         std abs
         */
        static inline i64 abs(const i64 n)
        {
            return ::std::abs(n);
        }
        
        /**
         std fabsf
         */
        static inline flt abs(const flt n)
        {
            return ::std::fabsf(n);
        }
        
        /**
         std fabs
         */
        static inline dbl abs(const dbl n)
        {
            return ::std::fabs(n);
        }
        
        /**
         standard round
         */
        static inline flt round(const flt n)
        {
            return ::std::round(n);
        }
        
        /**
         standard round
         */
        static inline dbl round(const dbl n)
        {
            return ::std::round(n);
        }
        
        /**
         rounds the given flt to the nearest 1/32 value
         */
        static inline flt roundTo32nd(const flt n)
        {
            return (flt)(round((dbl)n * 32.0) / 32.0);
        }
        
        /**
         rounds the given flt to the nearest 1/16 value
         */
        static inline flt roundTo16th(const flt n)
        {
            return (flt)(round((dbl)n * 16.0) / 16.0);
        }
        
        /**
         floors the given flt to the i32 value
         */
        static inline i32 fastfloor(const flt n)
        {
            i32 i = static_cast<i32>(n);
            return (n < i) ? (i - 1) : (i);
        }
        
        /**
         encloses given n into [min, max]
         outputs (n > max ? max : (n < min ? min : n))
         */
        static inline flt enclose(const flt n, const flt min, const flt max) //TODO: refactor to Clamp when we go to pascal casing
        {
            return n > max ? max : (n < min ? min : n);
        }
        
        /**
         returns the inverse sqrt of n using Quake III's q_rsqrt algorithm
         */
        static inline flt qInvSqrt(const flt n, i32 newton_iteration_count = 2)
        {
            union //keeping union for speed
            {
                flt result;
                i32 i_result;
            };
            
            result = n;
            i_result = 0x5f3759df - (i_result >> 1);
            flt half_n = n * 0.5f;
            
            do
            {
                result *= 1.5f - (half_n * result * result);
            }
            while(--newton_iteration_count > 0);
            
            return result;
        }
        
        /**
         returns the square root of n
         */
        static inline flt sqrt(const flt n)
        {
            return ::std::sqrt(n);
        }
        
        /**
         returns the square root of n
         */
        static inline dbl sqrt(const dbl n)
        {
            return ::std::sqrt(n);
        }
        
        /**
         returns n ^ e
         */
        static inline flt pow(const flt n, const flt e)
        {
            return ::std::pow(n, e);
        }
                
        /**
         computes n^0
         */
        template <typename T>
        static inline T pow0(const T n)
        {
            return 1;
        }

        /**
         computes n^1
         */
        template <typename T>
        static inline T pow1(const T n)
        {
            return n;
        }

        /**
         computes n^2
         */
        template <typename T>
        static inline T pow2(const T n)
        {
            return n * n;
        }

        /**
         computes n^3
         */
        template <typename T>
        static inline T pow3(const T n)
        {
            return n * n * n;
        }

        /**
         computes n^4
         */
        template <typename T>
        static inline T pow4(const T n)
        {
            return n * n * n * n;
        }

        /**
         computes n^5
         */
        template <typename T>
        static inline T pow5(const T n)
        {
            return n * n * n * n * n;
        }

        /**
         computes n^6 given a flt
         */
        template <typename T>
        static inline T pow6(const T n)
        {
            return n * n * n * n * n * n;
        }

        /**
         computes n^7
         */
        template <typename T>
        static inline T pow7(const T n)
        {
            return n * n * n * n * n * n * n;
        }

        /**
         computes n^8 given a flt
         */
        template <typename T>
        static inline T pow8(const T n)
        {
            return n * n * n * n * n * n * n * n;
        }
        
        /**
         gets the distance squared value from point a to b
         */
        static inline dbl distanceSquared(const flt ax, const flt ay, const flt bx, const flt by)
        {
            dbl x_diff = ax < bx ? bx - ax : ax - bx;
            dbl y_diff = ay < by ? by - ay : ay - by;
            
            return x_diff * x_diff + y_diff * y_diff;
        }
        
        /**
         gets the distance squared value from point a to b
         */
        static inline dbl distanceSquared(const dbl ax, const dbl ay, const dbl bx, const dbl by)
        {
            dbl x_diff = ax < bx ? bx - ax : ax - bx;
            dbl y_diff = ay < by ? by - ay : ay - by;
            
            return x_diff * x_diff + y_diff * y_diff;
        }
        
        /**
         gets the distance squared value from point a to b
         */
        template <typename T>
        static inline ui64 distanceSquared(const T ax, const T ay, const T bx, const T by)
        {
            ui64 x_diff = ax < bx ? bx - ax : ax - bx;
            ui64 y_diff = ay < by ? by - ay : ay - by;
            
            return x_diff * x_diff + y_diff * y_diff;
        }
        
        /**
         rounds the given flt to the nearest 1/powOfTwo value
         */
        static inline flt roundToInvPowOfTwo(const flt n, const ui8 powOfTwo)
        {
            dbl scalar = (dbl)pow(2, powOfTwo);
            return (flt)(round((dbl)n * scalar) / scalar);
        }
        
        /**
         returns the max of given a and b
         */
        template <typename T>
        static inline T max(T a, T b)
        {
            return ::std::max(a, b);
        }
        
        /**
         returns the min of given a and b
         */
        template <typename T>
        static inline T min(T a, T b)
        {
            return ::std::min(a, b);
        }
    }
}

#endif /* NP_ENGINE_MATH_FUNCTIONS_HPP */
