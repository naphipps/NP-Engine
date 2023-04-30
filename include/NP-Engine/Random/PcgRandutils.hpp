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
	using seed_seq_fe64 = randutils::seed_seq_fe<2, ::np::ui32>;
	using auto_seed_64 = randutils::auto_seeded<seed_seq_fe64>;
} // namespace randutils

using pcg32_fe = randutils::random_generator<pcg32, randutils::auto_seed_64>;
using pcg64_fe = randutils::random_generator<pcg64, randutils::auto_seed_128>;

#endif /* NP_ENGINE_PCG_RANDUTILS_HPP */
