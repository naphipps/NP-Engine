//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/4/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_FLAG_HPP
#define NP_ENGINE_GPU_INTERFACE_FLAG_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Time/Time.hpp"

#include "Detail.hpp"
#include "Result.hpp"

namespace np::gpu
{
	struct Flag : public DetailObject
	{
		virtual Result Set() = 0;

		virtual Result GetStatus() const = 0;

		virtual Result Reset() = 0;
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_FLAG_HPP */