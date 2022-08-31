//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/25/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_THREAD_POOL_HPP
#define NP_ENGINE_THREAD_POOL_HPP

#include <utility>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Insight/Insight.hpp"

#include "ThreadImpl.hpp"

namespace np::thr
{
	using ThreadPool = mem::ObjectPool<Thread>;
} // namespace np::thr

#endif /* NP_ENGINE_THREAD_POOL_HPP */
