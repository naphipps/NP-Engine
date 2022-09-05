//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/1/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_UID_HANDLE_HPP
#define NP_ENGINE_UID_HANDLE_HPP

#ifndef NP_ENGINE_UID_INVALID_KEY
	#define NP_ENGINE_UID_INVALID_KEY 0
#endif

#ifndef NP_ENGINE_UID_INVALID_GENERATION
	#define NP_ENGINE_UID_INVALID_GENERATION 0
#endif

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::uid
{
	struct UidHandle
	{
		using KeyType = siz;
		using GenerationType = siz;

		KeyType Key = NP_ENGINE_UID_INVALID_KEY;
		GenerationType Generation = NP_ENGINE_UID_INVALID_GENERATION;

		bl IsValid() const
		{
			return Key != NP_ENGINE_UID_INVALID_KEY && Generation != NP_ENGINE_UID_INVALID_GENERATION;
		}

		operator bl() const
		{
			return IsValid();
		}

		void Invalidate()
		{
			Key = NP_ENGINE_UID_INVALID_KEY;
			Generation = NP_ENGINE_UID_INVALID_GENERATION;
		}
	};
} // namespace np::uid

#endif /* NP_ENGINE_UID_HANDLE_HPP */