//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/25/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_FENCE_HPP
#define NP_ENGINE_GPU_INTERFACE_FENCE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Time/Time.hpp"

#include "Detail.hpp"
#include "Result.hpp"

namespace np::gpu
{
	struct Fence : public DetailObject
	{
		/*
			aka: Wait(tim::milliseconds{ DBL_MAX });
		*/
		virtual Result Wait() const
		{
			return Wait(tim::milliseconds{ DBL_MAX });
		}

		virtual Result Wait(tim::milliseconds timeout) const = 0;

		virtual Result GetStatus() const = 0;

		virtual Result Reset() = 0;
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_FENCE_HPP */