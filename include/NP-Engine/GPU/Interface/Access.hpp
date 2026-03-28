//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 1/26/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_ACCESS_HPP
#define NP_ENGINE_GPU_INTERFACE_ACCESS_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::gpu
{
	class Access : public enm_ui32
	{
	public:
		constexpr static ui32 Read = BIT(0);
		constexpr static ui32 Write = BIT(1);

		constexpr static ui32 Indirect = BIT(2);
		constexpr static ui32 Index = BIT(3);
		constexpr static ui32 Vertex = BIT(4);
		constexpr static ui32 Uniform = BIT(5);
		constexpr static ui32 Input = BIT(6);
		constexpr static ui32 Shader = BIT(7);
		constexpr static ui32 Image = BIT(8);
		constexpr static ui32 Depth = BIT(9);
		constexpr static ui32 Stencil = BIT(10);
		constexpr static ui32 Transfer = BIT(11);
		constexpr static ui32 Host = BIT(12);

		Access(ui32 value): enm_ui32(value) {}
	};
} //namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_ACCESS_HPP */