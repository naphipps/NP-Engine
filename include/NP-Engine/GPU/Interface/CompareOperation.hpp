//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/8/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_COMPARE_OPERATION_HPP
#define NP_ENGINE_GPU_INTERFACE_COMPARE_OPERATION_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::gpu
{
	class CompareOperation : public Enum<ui32>
	{
	public:
		constexpr static ui32 Never = BIT(0);
		constexpr static ui32 Equal = BIT(1);
		constexpr static ui32 NotEqual = BIT(2);
		constexpr static ui32 Less = BIT(3);
		constexpr static ui32 Greater = BIT(4);
		constexpr static ui32 Always = BIT(5);

		CompareOperation(ui32 value) : Enum<ui32>(value) {}
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_COMPARE_OPERATION_HPP */