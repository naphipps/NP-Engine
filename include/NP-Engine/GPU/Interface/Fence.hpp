//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/25/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_FENCE_HPP
#define NP_ENGINE_GPU_INTERFACE_FENCE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "Detail.hpp"

namespace np::gpu
{
	struct Fence : public DetailObject
	{
		/*
			aka: Wait(SIZ_MAX);
		*/
		virtual void Wait() const
		{
			Wait(SIZ_MAX);
		}

		/*
			TODO: what units is timeout??
		*/
		virtual void Wait(siz timeout) const = 0;

		//TODO: do we want to support resetting fences? feels like we could just create new ones just as easily....
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_FENCE_HPP */