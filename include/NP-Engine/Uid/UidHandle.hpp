//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/1/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_UID_HANDLE_HPP
#define NP_ENGINE_UID_HANDLE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::uid
{
	struct UidHandle
	{
		void* Key = nullptr;
		siz Generation = 0;
	};
}

#endif /* NP_ENGINE_UID_HANDLE_HPP */