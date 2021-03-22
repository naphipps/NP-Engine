//
//  exp_functions.hpp
//  Project Space
//
//  Created by Nathan Phipps on 12/28/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_EXP_FUNCTIONS_HPP
#define NP_ENGINE_EXP_FUNCTIONS_HPP

namespace np
{
    namespace math
    {
        /**
         e ^ n
         */
        static inline flt exp(const flt n)
        {
            return ::std::exp(n);
        }
        
        /**
         e ^ n
         */
        static inline dbl exp(const dbl n)
        {
            return ::std::exp(n);
        }
    }
}

#endif /* NP_ENGINE_EXP_FUNCTIONS_HPP */
