//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/16/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_SAMPLE_DISTRIBUTION_HPP
#define NP_ENGINE_SAMPLE_DISTRIBUTION_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Math/Math.hpp"

namespace np::rng
{
	template <ui32 RESOLUTION>
	class CdfSampleDistribution
	{
	public:
		using CdfFunctionType = flt (*)(const flt); // keep flt since our resolution is ui32

		struct SampleType
		{
			flt probability;
			flt value;
			friend bl operator<(flt probability, const SampleType& other)
			{
				return probability < other.probability;
			}
		};

	private:
		ui32 _resolution = RESOLUTION;
		con::array<SampleType, RESOLUTION + 1> _sample; //"RESOLUTION + 1" ensures sample [0, 1]
		CdfFunctionType _cdf_function;
		flt _min_result;
		flt _max_result;
		bl _enable_lerp;

		inline void Init()
		{
			NP_ENGINE_ASSERT(_min_result < _max_result, "min must always be < max");
			i32 powOfTwo = mat::FastFloor(::std::sqrt((flt)_sample.size()));

			for (ui64 i = 0; i < _sample.size(); i++)
			{
				_sample[i].value = (flt)i / (flt)_resolution * (_max_result - _min_result) + _min_result;
				_sample[i].probability = mat::RoundToInvPowOfTwo(::std::abs(_cdf_function(_sample[i].value)), powOfTwo);

				if (i > 0)
				{
					NP_ENGINE_ASSERT(_sample[i - 1].probability <= _sample[i].probability,
									 "our probability must always increase");
				}
			}
		}

	public:
		CdfSampleDistribution(CdfFunctionType cdf, flt min, flt max, bl enable_lerp = true):
			_cdf_function(cdf),
			_min_result(min),
			_max_result(max),
			_enable_lerp(enable_lerp)
		{
			Init();
		}

		inline void SetCdf(const CdfFunctionType cdf)
		{
			_cdf_function = cdf;
			Init();
		}

		inline void SetMin(const flt min)
		{
			_min_result = min;
			Init();
		}

		inline void SetMax(const flt max)
		{
			_max_result = max;
			Init();
		}

		inline flt GetMin() const
		{
			return _min_result;
		}

		inline flt GetMax() const
		{
			return _max_result;
		}

		/*
			enables lerp when getting value, allowing for accurate value retrieval between samples
		*/
		inline void SetEnableLerp(const bl enable = true)
		{
			_enable_lerp = enable;
		}

		inline ui32 GetResolution() const
		{
			return _resolution;
		}

		inline flt operator()(Random32& rng, ui32 resolution = RESOLUTION * 100)
		{
			// generate normalized uniform probability
			ui32 rng_range = resolution + 1;
			ui32 uniform_lemire = rng.GetLemireWithinRange(rng_range); //[0, rng_range)
			flt norm_uniform_result_f = (flt)uniform_lemire / (flt)(rng_range - 1); //[0, 1]
			flt chosen_probability = norm_uniform_result_f;

			// extract sample value from chosen probability above
			using SampleIterator = typename con::array<SampleType, RESOLUTION>::iterator; // sample_type*;
			SampleIterator bound = ::std::upper_bound(_sample.begin(), _sample.end(), chosen_probability);
			flt value = _min_result - 1; // sets an invalid value

			if (bound == _sample.begin()) // get value at the begining
			{
				value = bound->value;
			}
			else if (bound == _sample.end()) // get value at the end
			{
				value = (bound - 1)->value;
			}
			else
			{
				SampleIterator prev = bound - 1;
				SampleIterator next = bound;

				if (_enable_lerp)
				{
					flt t = (chosen_probability - prev->probability) / (next->probability - prev->probability);
					value = ::glm::lerp(prev->value, next->value, t);
				}
				else
				{
					value = chosen_probability >= 0.5f ? next->value : prev->value;
				}
			}

			NP_ENGINE_ASSERT(value >= _min_result && value <= _max_result,
							 "our result must be within [" + to_str(_min_result) + ", " + to_str(_max_result) + "]");

			return value;
		}
	};

	template <ui32 RESOLUTION>
	struct StandardSampledDistribution : public CdfSampleDistribution<RESOLUTION>
	{
		StandardSampledDistribution():
			CdfSampleDistribution<RESOLUTION>(mat::StandardCdf, mat::MIN_STANDARD_CDF, mat::MAX_STANDARD_CDF)
		{}
	};

	using CdfSampledDistribution100 = CdfSampleDistribution<100>;
	using CdfSampledDistribution200 = CdfSampleDistribution<200>;

	using StandardSampledDistribution100 = StandardSampledDistribution<100>;
	using StandardSampledDistribution200 = StandardSampledDistribution<200>;
} // namespace np::rng

#endif /* NP_ENGINE_SAMPLE_DISTRIBUTION_HPP */
