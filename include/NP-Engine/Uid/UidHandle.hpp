//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/1/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_UID_HANDLE_HPP
#define NP_ENGINE_UID_HANDLE_HPP

#ifndef NP_ENGINE_UID_HANDLE_INVALID_KEY
	#define NP_ENGINE_UID_HANDLE_INVALID_KEY 0
#endif

#ifndef NP_ENGINE_UID_HANDLE_INVALID_GENERATION
	#define NP_ENGINE_UID_HANDLE_INVALID_GENERATION 0
#endif

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"

namespace np::uid
{
	struct UidHandle
	{
		using KeyType = ui32;
		using GenerationType = ui32;

		KeyType Key = NP_ENGINE_UID_HANDLE_INVALID_KEY;
		GenerationType Generation = NP_ENGINE_UID_HANDLE_INVALID_GENERATION;

		bl IsValid() const
		{
			return Key != NP_ENGINE_UID_HANDLE_INVALID_KEY && Generation != NP_ENGINE_UID_HANDLE_INVALID_GENERATION;
		}

		operator bl() const
		{
			return IsValid();
		}

		void Invalidate()
		{
			Key = NP_ENGINE_UID_HANDLE_INVALID_KEY;
			Generation = NP_ENGINE_UID_HANDLE_INVALID_GENERATION;
		}
	};

	using UidHandlePool = mem::AccumulatingPool<UidHandle>;
} // namespace np::uid

#endif /* NP_ENGINE_UID_HANDLE_HPP */