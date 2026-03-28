//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/11/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_LOGIC_HPP
#define NP_ENGINE_GPU_INTERFACE_LOGIC_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::gpu
{
	class LogicOperation : public enm_ui32
	{
	public:
		constexpr static ui32 NoOp = BIT(0);
		constexpr static ui32 Set = BIT(1);
		constexpr static ui32 Clear = BIT(2);
		constexpr static ui32 Copy = BIT(3);

		constexpr static ui32 And = BIT(4);
		constexpr static ui32 Xor = BIT(5);
		constexpr static ui32 Or = BIT(6);
		constexpr static ui32 Equal = BIT(7);

		constexpr static ui32 Reverse = BIT(8);
		constexpr static ui32 Invert = BIT(9);
		constexpr static ui32 Not = BIT(10);

		LogicOperation(ui32 value): enm_ui32(value) {}
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_LOGIC_HPP */