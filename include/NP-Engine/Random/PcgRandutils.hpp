//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/16/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_PCG_RANDUTILS_HPP
#define NP_ENGINE_PCG_RANDUTILS_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/PcgCppInclude.hpp"
#include "NP-Engine/Vendor/RandutilsInclude.hpp"

/*
	this file holds the mixing of randutils and pcg
*/

namespace randutils
{
	typedef randutils::seed_seq_fe<2, ui32> seed_seq_fe64;
	typedef randutils::auto_seeded<seed_seq_fe64> auto_seed_64;
} // namespace randutils

typedef randutils::random_generator<pcg32, randutils::auto_seed_64> pcg32_fe;
typedef randutils::random_generator<pcg64, randutils::auto_seed_128> pcg64_fe;

#endif /* NP_ENGINE_PCG_RANDUTILS_HPP */
