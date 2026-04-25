//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/25/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_FENCE_HPP
#define NP_ENGINE_GPU_INTERFACE_FENCE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "Detail.hpp"
#include "Result.hpp"

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

		virtual Result GetStatus() const = 0;

		virtual Result Reset() = 0;
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_FENCE_HPP */