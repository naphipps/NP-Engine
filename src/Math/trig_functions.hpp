//
//  trig_functions.hpp
//  Project Space
//
//  Created by Nathan Phipps on 12/28/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_TRIG_FUNCTIONS_HPP
#define NP_ENGINE_TRIG_FUNCTIONS_HPP

namespace np
{
    namespace math
    {
        /**
         cosine
         */
        static inline flt cos(const flt n)
        {
            return ::std::cosf(n);
        }
        
        /**
         cosine
         */
        static inline dbl cos(const dbl n)
        {
            return ::std::cos(n);
        }
        
        /**
         sine
         */
        static inline flt sin(const flt n)
        {
            return ::std::sinf(n);
        }
        
        /**
         sine
         */
        static inline dbl sin(const dbl n)
        {
            return ::std::sin(n);
        }
        
        /**
         arccosine
         */
        static inline flt acos(const flt n)
        {
            return ::std::acosf(n);
        }
        
        /**
         arccosine
         */
        static inline dbl acos(const dbl n)
        {
            return ::std::acos(n);
        }
        
        /**
         arcsine
         */
        static inline flt asin(const flt n)
        {
            return ::std::asinf(n);
        }
        
        /**
         arcsine
         */
        static inline dbl asin(const dbl n)
        {
            return ::std::asin(n);
        }
        
        /**
         atan
         */
        static inline flt atan(const flt n)
        {
            return ::std::atanf(n);
        }
        
        /**
         atan
         */
        static inline dbl atan(const dbl n)
        {
            return ::std::atan(n);
        }
        
        /**
         atan2
         */
        static inline flt atan2(const flt y, const flt x)
        {
            return ::std::atan2f(y, x);
        }
        
        /**
         atan2
         */
        static inline dbl atan2(const dbl y, const dbl x)
        {
            return ::std::atan2(y, x);
        }
    }
}

#endif /* NP_ENGINE_TRIG_FUNCTIONS_HPP */
