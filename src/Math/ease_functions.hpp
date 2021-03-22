//
//  ease_functions.hpp
//  Project Space
//
//  Created by Nathan Phipps on 12/28/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_EASE_FUNCTIONS_HPP
#define NP_ENGINE_EASE_FUNCTIONS_HPP

namespace np
{
    namespace math
    {
        /**
         input n = [0, 1]
         returns 1 - n
         */
        static inline flt flip(const flt n)
        {
            return 1.f - n;
        }
        
        /**
         input n = [0, 1]
         returns 1 - n
         */
        static inline dbl flip(const dbl n)
        {
            return 1.0 - n;
        }
        
        /**
         linearly interpolates from a to b given t = [0, 1]
         computes (1.0f - t) * a + t * b;
         */
        static inline flt lerp(const flt a, const flt b, const flt t)
        {
            return flip(t) * a + t * b;
        }
        
        /**
         linearly interpolates from a to b given t = [0, 1]
         computes (1.0f - t) * a + t * b;
         */
        static inline dbl lerp(const dbl a, const dbl b, const dbl t)
        {
            return flip(t) * a + t * b;
        }
    }
}

#endif /* NP_ENGINE_EASE_FUNCTIONS_HPP */
