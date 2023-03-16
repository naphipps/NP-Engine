//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/1/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_UID_IMPL_HPP
#define NP_ENGINE_UID_IMPL_HPP

#include "NP-Engine/Memory/Memory.hpp"

#include "NP-Engine/Vendor/StduuidInclude.hpp"

namespace np::uid
{
	using Uid = ::uuids::uuid;
	using UidPool = mem::AccumulatingPool<Uid>;
} // namespace np::uid

#endif /* NP_ENGINE_UID_IMPL_HPP */