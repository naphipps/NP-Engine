//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/1/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_UID_POOL_HPP
#define NP_ENGINE_UID_POOL_HPP

#include "NP-Engine/Memory/Memory.hpp"

#include "UidImpl.hpp"

namespace np::uid
{
	using UidPool = mem::ObjectPool<Uid>;
} // namespace np::uid

#endif /* NP_ENGINE_UID_POOL_HPP */