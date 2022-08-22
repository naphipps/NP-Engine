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
	using JobPool = memory::ObjectPool<Job>;
} // namespace np::js

#endif /* NP_ENGINE_JOB_POOL_HPP */
