//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/18/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_STATISTIC_FUNCTION_HPP
#define NP_ENGINE_STATISTIC_FUNCTION_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "CMathInclude.hpp"
#include "MathImpl.hpp"
#include "owen.hpp"

namespace np::mat
{
	constexpr static flt MIN_STANDARD_CDF = -3.5f;
	constexpr static flt MAX_STANDARD_CDF = 3.5f;

	static inline flt StandardPdf(const flt x)
	{
		return pow(M_E, -Pow2(x) / 2.f) / ::std::sqrt(M_PI * 2.f);
	}

	static inline flt StandardCdf(const flt x)
	{
		return (1.f + ::std::erf(x / M_SQRT2)) / 2.f;
	}

	static inline flt StandardPdfSkewed(const flt x, const flt scale, const flt location, const flt skew = 0.f)
	{
		return (2.f / scale) * StandardPdf((x - location) / scale) * StandardCdf(skew * (x - location) / scale);
	}

	static inline flt CalcSkewStandardMean(const flt scale, const flt location, const flt skew = 0.f)
	{
		return location + ::std::sqrt(2.f / M_PI) * scale * (skew / (::std::sqrt(1.f + Pow2(skew))));
	}

	static inline flt StandardCdfSkewed(const flt x, const flt scale, const flt location, const flt skew = 0.f)
	{
		return StandardCdf((x - location) / scale) - 2.f * owen_t((x - location) / scale, skew);
	}

	static inline flt CalcSkewDenom(const flt scale, const flt location, const flt mean = 0.f)
	{
		return ((2.f * Pow2(scale)) / (M_PI * Pow2(mean - location))) - 1.f;
	}

	static inline bl IsSkewReal(const flt scale, const flt location, const flt mean = 0.f)
	{
		return CalcSkewDenom(scale, location, mean) >= 0.f;
	}

	static inline flt CalcSkew(const flt scale, const flt location, const flt mean = 0.f)
	{
		flt skew;
		if (IsSkewReal(scale, location, mean))
			skew =
				(mean < 0.f ? -1.f : 1.f) * ::std::sqrt(1.f / (((2.f * Pow2(scale)) / (M_PI * Pow2(mean - location))) - 1.f));
		else
			skew = ::std::nanf("skew is not real");
		return skew;
	}

	static inline dbl StandardPdf(const dbl x)
	{
		return ::std::pow(M_E, -Pow2(x) / 2.0) / ::std::sqrt(M_PI * 2.0);
	}

	static inline dbl StandardCdf(const dbl x)
	{
		return (1.0 + ::std::erf(x / M_SQRT2)) / 2.0;
	}

	static inline dbl StandardPdfSkewed(const dbl x, const dbl scale, const dbl location, const dbl skew = 0.0)
	{
		return (2.0 / scale) * StandardPdf((x - location) / scale) * StandardCdf(skew * (x - location) / scale);
	}

	static inline dbl CalcSkewStandardMean(const dbl scale, const dbl location, const dbl skew = 0.0)
	{
		return location + ::std::sqrt(2.0 / M_PI) * scale * (skew / (::std::sqrt(1.0 + Pow2(skew))));
	}

	static inline dbl StandardCdfSkewed(const dbl x, const dbl scale, const dbl location, const dbl skew = 0.0)
	{
		return StandardCdf((x - location) / scale) - 2.0 * owen_t((x - location) / scale, skew);
	}

	static inline dbl CalcSkewDenom(const dbl scale, const dbl location, const dbl mean = 0.0)
	{
		return ((2.0 * Pow2(scale)) / (M_PI * Pow2(mean - location))) - 1.0;
	}

	static inline bl IsSkewReal(const dbl scale, const dbl location, const dbl mean = 0.0)
	{
		return CalcSkewDenom(scale, location, mean) >= 0.0;
	}

	static inline dbl CalcSkew(const dbl scale, const dbl location, const dbl mean = 0.0)
	{
		dbl skew;
		if (IsSkewReal(scale, location, mean))
			skew =
				(mean < 0.0 ? -1.0 : 1.0) * ::std::sqrt(1.0 / (((2.0 * Pow2(scale)) / (M_PI * Pow2(mean - location))) - 1.0));
		else
			skew = ::std::nan("skew is not real");
		return skew;
	}
} // namespace np::mat

#endif /* NP_ENGINE_STATISTIC_FUNCTION_HPP */
