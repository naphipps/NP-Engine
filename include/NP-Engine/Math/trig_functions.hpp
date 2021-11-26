//
//  trig_functions.hpp
//  Project Space
//
//  Created by Nathan Phipps on 12/28/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

// TODO: I don't think we need this file... or at least support for the std functions

#ifndef NP_ENGINE_TRIG_FUNCTIONS_HPP
#define NP_ENGINE_TRIG_FUNCTIONS_HPP

#include "cmath_include.hpp"

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
			return ::std::cosl(n); // TODO: I'm not sure this is correct
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
			return ::std::sinl(n); // TODO: I'm not sure this is correct
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
			return ::std::acosl(n); // TODO: I'm not sure this is correct
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
			return ::std::asinl(n); // TODO: I'm not sure this is correct
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
			return ::std::atanl(n); // TODO: I'm not sure this is correct
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
			return ::std::atan2l(y, x);
		}
	} // namespace math
} // namespace np

#endif /* NP_ENGINE_TRIG_FUNCTIONS_HPP */
