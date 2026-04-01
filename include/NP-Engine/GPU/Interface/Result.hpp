//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/29/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_RESULT_HPP
#define NP_ENGINE_GPU_INTERFACE_RESULT_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::gpu
{
	class Result : public enm_ui32
	{
	public:
		constexpr static ui32 Error = Zero;
		constexpr static ui32 Success = BIT(0);
		constexpr static ui32 Suboptimal = BIT(1);
		constexpr static ui32 OutOfDate = BIT(2);

		Result(ui32 value = None) : enm_ui32(value) {}
	};
} //namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_RESULT_HPP */