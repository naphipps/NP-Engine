//
//  statistic_functions.hpp
//  Project Space
//
//  Created by Nathan Phipps on 12/18/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_STATISTIC_FUNCTION_HPP
#define NP_ENGINE_STATISTIC_FUNCTION_HPP

#include "Insight/Insight.hpp"
#include "Primitive/Primitive.hpp"

#include "math_functions.hpp"
#include "owen.hpp"

namespace np
{
    namespace math
    {
        /**
         error function from cmath
         */
        static inline flt erf(const flt n)
        {
            return ::std::erf(n);
        }
        
        /**
         error function from cmath
         */
        static inline dbl erf(const dbl n)
        {
            return ::std::erf(n);
        }
        
        /**
         complementary error function from cmath
         */
        static inline flt erfc(const flt n)
        {
            return ::std::erfc(n);
        }
        
        /**
         complementary error function from cmath
         */
        static inline dbl erfc(const dbl n)
        {
            return ::std::erfc(n);
        }
        
        constexpr static flt MIN_STANDARD_CDF = -3.5f;
        constexpr static flt MAX_STANDARD_CDF = 3.5f;
        
        /**
         standard normal distribution pdf
         */
        static inline flt standard_pdf(const flt x)
        {
            return pow(M_E, -pow2(x) / 2.f) / sqrt(M_PI * 2.f);
        }
        
        /**
         standard normal distribution cdf
         */
        static inline flt standard_cdf(const flt x)
        {
            return (1.f + erf(x / M_SQRT2)) / 2.f;
        }
        
        /**
         skewable standard normal pdf
         */
        static inline flt skew_standard_pdf(const flt x, const flt scale, const flt location, const flt skew = 0.f)
        {
            return (2.f / scale) * standard_pdf((x - location) / scale) * standard_cdf(skew * (x - location) / scale);
        }
        
        /**
         calculates the mean of the standard distribution
         */
        static inline flt calc_skew_standard_mean(const flt scale, const flt location, const flt skew = 0.f)
        {
            return location + sqrt(2.f / M_PI) * scale * (skew / (sqrt(1.f + pow2(skew))));
        }
        
        /**
         skewable standard normal cdf
         */
        static inline flt skew_standard_cdf(const flt x, const flt scale, const flt location, const flt skew = 0.f)
        {
            return standard_cdf((x - location) / scale) - 2.f * owen_t((x - location) / scale, skew);
        }
        
        /**
         calculates the denom portion of the calc_skew calculation
         
         you can use this to indicate if the given scale, location, and mean allow a real skew
         */
        static inline flt calc_skew_denom(const flt scale, const flt location, const flt mean = 0.f)
        {
            return ((2.f * pow2(scale)) / (M_PI * pow2(mean - location))) - 1.f;
        }
        
        /**
         checks if the given scale, location, and mean allow for a real skew by checking if the denom is positive
         */
        static inline bl is_calc_skew_real(const flt scale, const flt location, const flt mean = 0.f)
        {
            return calc_skew_denom(scale, location, mean) >= 0.f;
        }
        
        /**
         calculates the skew of the standard ditribution
         */
        static inline flt calc_skew(const flt scale, const flt location, const flt mean = 0.f)
        {
            NP_ASSERT(is_calc_skew_real(scale, location, mean),
                      "skew must be real - s:"+to_str(scale)+", l:"+
                      to_str(location)+", m:"+to_str(mean));
            
            return (mean < 0.f ? -1.f : 1.f) * sqrt(1.f / (((2.f * pow2(scale)) / (M_PI * pow2(mean - location))) - 1.f));
        }
        
        /**
         standard normal distribution pdf
         */
        static inline dbl standard_pdf(const dbl x)
        {
            return pow(M_E, -pow2(x) / 2.0) / sqrt(M_PI * 2.0);
        }
        
        /**
         standard normal distribution cdf
         */
        static inline dbl standard_cdf(const dbl x)
        {
            return (1.0 + erf(x / M_SQRT2)) / 2.0;
        }
        
        /**
         skewable standard normal pdf
         */
        static inline dbl skew_standard_pdf(const dbl x, const dbl scale, const dbl location, const dbl skew = 0.0)
        {
            return (2.0 / scale) * standard_pdf((x - location) / scale) * standard_cdf(skew * (x - location) / scale);
        }
        
        /**
         calculates the mean of the standard distribution
         */
        static inline dbl calc_skew_standard_mean(const dbl scale, const dbl location, const dbl skew = 0.0)
        {
            return location + sqrt(2.0 / M_PI) * scale * (skew / (sqrt(1.0 + pow2(skew))));
        }
        
        /**
         skewable standard normal cdf
         */
        static inline dbl skew_standard_cdf(const dbl x, const dbl scale, const dbl location, const dbl skew = 0.0)
        {
            return standard_cdf((x - location) / scale) - 2.0 * owen_t((x - location) / scale, skew);
        }
        
        /**
         calculates the denom portion of the calc_skew calculation
         
         you can use this to indicate if the given scale, location, and mean allow a real skew
         */
        static inline dbl calc_skew_denom(const dbl scale, const dbl location, const dbl mean = 0.0)
        {
            return ((2.0 * pow2(scale)) / (M_PI * pow2(mean - location))) - 1.0;
        }
        
        /**
         checks if the given scale, location, and mean allow for a real skew by checking if the denom is positive
         */
        static inline bl is_calc_skew_real(const dbl scale, const dbl location, const dbl mean = 0.0)
        {
            return calc_skew_denom(scale, location, mean) >= 0.0;
        }
        
        /**
         calculates the skew of the standard ditribution
         */
        static inline dbl calc_skew(const dbl scale, const dbl location, const dbl mean = 0.0)
        {
            NP_ASSERT(is_calc_skew_real(scale, location, mean),
                      "skew must be real - s:"+to_str(scale)+", l:"+
                      to_str(location)+", m:"+to_str(mean));
            
            return (mean < 0.0 ? -1.0 : 1.0) * sqrt(1.0 / (((2.0 * pow2(scale)) / (M_PI * pow2(mean - location))) - 1.0));
        }
    }
}

#endif /* NP_ENGINE_STATISTIC_FUNCTION_HPP */
