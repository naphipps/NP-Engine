//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/18/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MAT_STATISTIC_FUNCTION_HPP
#define NP_ENGINE_MAT_STATISTIC_FUNCTION_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "CMathInclude.hpp"
#include "MathImpl.hpp"
#include "Owen.hpp"

namespace np::mat
{
	constexpr static flt MIN_STANDARD_CDF = -3.5f;
	constexpr static flt MAX_STANDARD_CDF = 3.5f;

	static inline flt standard_pdf(const flt x)
	{
		return pow(M_E, -pow_2(x) / 2.f) / ::std::sqrt(M_PI * 2.f);
	}

	static inline flt standard_cdf(const flt x)
	{
		return (1.f + ::std::erf(x / M_SQRT2)) / 2.f;
	}

	static inline flt standard_pdf_skewed(const flt x, const flt scale, const flt location, const flt skew = 0.f)
	{
		return (2.f / scale) * standard_pdf((x - location) / scale) * standard_cdf(skew * (x - location) / scale);
	}

	static inline flt calc_skew_standard_mean(const flt scale, const flt location, const flt skew = 0.f)
	{
		return location + ::std::sqrt(2.f / M_PI) * scale * (skew / (::std::sqrt(1.f + pow_2(skew))));
	}

	static inline flt standard_cdf_skewed(const flt x, const flt scale, const flt location, const flt skew = 0.f)
	{
		return standard_cdf((x - location) / scale) - 2.f * owen_t((x - location) / scale, skew);
	}

	static inline flt calc_skew_denom(const flt scale, const flt location, const flt mean = 0.f)
	{
		return ((2.f * pow_2(scale)) / (M_PI * pow_2(mean - location))) - 1.f;
	}

	static inline bl is_skew_real(const flt scale, const flt location, const flt mean = 0.f)
	{
		return calc_skew_denom(scale, location, mean) >= 0.f;
	}

	static inline flt calc_skew(const flt scale, const flt location, const flt mean = 0.f)
	{
		flt skew;
		if (is_skew_real(scale, location, mean))
			skew =
				(mean < 0.f ? -1.f : 1.f) * ::std::sqrt(1.f / (((2.f * pow_2(scale)) / (M_PI * pow_2(mean - location))) - 1.f));
		else
			skew = ::std::nanf("skew is not real");
		return skew;
	}

	static inline dbl standard_pdf(const dbl x)
	{
		return ::std::pow(M_E, -pow_2(x) / 2.0) / ::std::sqrt(M_PI * 2.0);
	}

	static inline dbl standard_cdf(const dbl x)
	{
		return (1.0 + ::std::erf(x / M_SQRT2)) / 2.0;
	}

	static inline dbl standard_pdf_skewed(const dbl x, const dbl scale, const dbl location, const dbl skew = 0.0)
	{
		return (2.0 / scale) * standard_pdf((x - location) / scale) * standard_cdf(skew * (x - location) / scale);
	}

	static inline dbl calc_skew_standard_mean(const dbl scale, const dbl location, const dbl skew = 0.0)
	{
		return location + ::std::sqrt(2.0 / M_PI) * scale * (skew / (::std::sqrt(1.0 + pow_2(skew))));
	}

	static inline dbl standard_cdf_skewed(const dbl x, const dbl scale, const dbl location, const dbl skew = 0.0)
	{
		return standard_cdf((x - location) / scale) - 2.0 * owen_t((x - location) / scale, skew);
	}

	static inline dbl calc_skew_denom(const dbl scale, const dbl location, const dbl mean = 0.0)
	{
		return ((2.0 * pow_2(scale)) / (M_PI * pow_2(mean - location))) - 1.0;
	}

	static inline bl is_skew_real(const dbl scale, const dbl location, const dbl mean = 0.0)
	{
		return calc_skew_denom(scale, location, mean) >= 0.0;
	}

	static inline dbl calc_skew(const dbl scale, const dbl location, const dbl mean = 0.0)
	{
		dbl skew;
		if (is_skew_real(scale, location, mean))
			skew =
				(mean < 0.0 ? -1.0 : 1.0) * ::std::sqrt(1.0 / (((2.0 * pow_2(scale)) / (M_PI * pow_2(mean - location))) - 1.0));
		else
			skew = ::std::nan("skew is not real");
		return skew;
	}
} // namespace np::mat

#endif /* NP_ENGINE_MAT_STATISTIC_FUNCTION_HPP */
