//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/26/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_JOB_POOL_HPP
#define NP_ENGINE_JOB_POOL_HPP

#include "NP-Engine/Memory/Memory.hpp"

#include "Job.hpp"

namespace np::jsys
{
	using JobPool = mem::AccumulatingPool<Job>;
} // namespace np::jsys

#endif /* NP_ENGINE_JOB_POOL_HPP */
