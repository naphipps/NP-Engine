//
//  RandomDistribution.hpp //TODO: rename file to SampleDistribution
//  Project Space
//
//  Created by Nathan Phipps on 12/16/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_RANDOM_DISTRIBUTION_HPP
#define NP_ENGINE_RANDOM_DISTRIBUTION_HPP

#include "Container/Container.hpp"
#include "Primitive/Primitive.hpp"
#include "Math/Math.hpp"
#include "Insight/Insight.hpp"

namespace np
{
    namespace random
    {
        /**
         a distribution whose samples are based on a cdf
         */
        template <ui32 RESOLUTION>
        class CdfSampleDistribution
        {
        public:
            
            using CdfFunctionType = flt(*)(const flt); //TODO: should we make this dbl??
            
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
            container::array<SampleType, RESOLUTION + 1> _sample; //"RESOLUTION + 1" ensures sample [0, 1]
            CdfFunctionType _cdf_function;
            flt _min_result;
            flt _max_result;
            bl _enable_lerp;
            
            /**
             init
             */
            inline void Init()
            {
                NP_ASSERT(_min_result < _max_result, "min must always be < max");
                i32 powOfTwo = math::fastfloor(math::sqrt((flt)_sample.size()));
                
                for (ui64 i=0; i<_sample.size(); i++)
                {
                    _sample[i].value = (flt)i / (flt)_resolution * (_max_result - _min_result) + _min_result;
                    _sample[i].probability = math::roundToInvPowOfTwo(math::abs(_cdf_function(_sample[i].value)), powOfTwo);
                    
                    if (i > 0)
                    {
                        NP_ASSERT(_sample[i-1].probability <= _sample[i].probability,
                                  "our probability must always increase");
                    }
                }
            }
            
        public:
            
            /**
             constructor
             */
            CdfSampleDistribution(CdfFunctionType cdf, flt min, flt max, bl enable_lerp = true):
            _cdf_function(cdf),
            _min_result(min),
            _max_result(max),
            _enable_lerp(enable_lerp)
            {
                Init();
            }
            
            /**
             deconstructor
             */
            ~CdfSampleDistribution()
            {}
            
            /**
             sets the cdf function then calls init
             */
            inline void SetCdf(const CdfFunctionType cdf)
            {
                _cdf_function = cdf;
                Init();
            }
            
            /**
             sets the min result then calls init
             */
            inline void SetMin(const flt min)
            {
                _min_result = min;
                Init();
            }
            
            /**
             sets the max restult then calls init
             */
            inline void SetMax(const flt max)
            {
                _max_result = max;
                Init();
            }
            
            /**
             gets the min result
             */
            inline flt GetMin() const
            {
                return _min_result;
            }
            
            /**
             gets the max result
             */
            inline flt GetMax() const
            {
                return _max_result;
            }
            
            /**
             enables lerp when getting value
             */
            inline void SetEnableLerp(const bl enable = true)
            {
                _enable_lerp = enable;
            }
            
            /**
             gets the resolution
             */
            inline ui32 GetResolution() const
            {
                return _resolution;
            }
            
            /**
             generates a random value [min, max] with resolution
             */
            inline flt operator()(Random32& rng, ui32 resolution = RESOLUTION * 100)
            {
                //generate normalized uniform probability
                ui32 rng_range = resolution + 1;
                ui32 uniform_lemire = rng.GetLemireWithinRange(rng_range); //[0, rng_range)
                flt norm_uniform_result_f = (flt)uniform_lemire / (flt)(rng_range - 1); //[0, 1]
                flt chosen_probability = norm_uniform_result_f;
                
                //extract sample value from chosen probability above
                using SampleIterator = typename container::array<SampleType, RESOLUTION>::iterator;//sample_type*;
                SampleIterator bound = ::std::upper_bound(_sample.begin(), _sample.end(), chosen_probability);
                flt value = _min_result - 1; //sets an invalid value
                
                if (bound == _sample.begin()) //get value at the begining
                {
                    value = bound->value;
                }
                else if (bound == _sample.end()) //get value at the end
                {
                    value = (bound-1)->value;
                }
                else
                {
                    SampleIterator prev = bound - 1;
                    SampleIterator next = bound;
                    
                    if (_enable_lerp)
                    {
                        flt t = (chosen_probability - prev->probability) / (next->probability - prev->probability);
                        value = math::lerp(prev->value, next->value, t);
                    }
                    else
                    {
                        value = chosen_probability >= 0.5f ? next->value : prev->value;
                    }
                }
                
                NP_ASSERT(value >= _min_result && value <= _max_result,
                          "our result must be within ["+to_str(_min_result)+", "+to_str(_max_result)+"]");
                                
                return value;
            }
        };
        
        /**
         the standard cdf distribution
         */
        template <ui32 RESOLUTION>
        struct StandardSampledDistribution : public CdfSampleDistribution<RESOLUTION>
        {
            StandardSampledDistribution():
            CdfSampleDistribution<RESOLUTION>(math::standard_cdf, math::MIN_STANDARD_CDF, math::MAX_STANDARD_CDF)
            {}
        };
        
        using CdfSampledDistribution100 = CdfSampleDistribution<100>;
        using CdfSampledDistribution200 = CdfSampleDistribution<200>;
        
        using StandardSampledDistribution100 = StandardSampledDistribution<100>;
        using StandardSampledDistribution200 = StandardSampledDistribution<200>;
    }
}

#endif /* NP_ENGINE_RANDOM_DISTRIBUTION_HPP */
