//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/18/24
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_STAGE_HPP
#define NP_ENGINE_GPU_INTERFACE_STAGE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::gpu
{
	class Stage : public Enum<ui32>
	{
	public:
        constexpr static ui32 Top = BIT(0);
        constexpr static ui32 VertexInput = BIT(1);
        constexpr static ui32 VertexOutput = BIT(2);
        constexpr static ui32 Vertex = VertexInput | VertexOutput;
        constexpr static ui32 TesselationControl = BIT(3);
        constexpr static ui32 TesselationEvaluation = BIT(4);
        constexpr static ui32 Geometry = BIT(5);
        constexpr static ui32 FragmentInput = BIT(6);
        constexpr static ui32 FragmentOutput = BIT(7);
        constexpr static ui32 Fragment = FragmentInput | FragmentOutput;
        constexpr static ui32 PresentComplete = BIT(8);
		constexpr static ui32 Compute = BIT(9);
        constexpr static ui32 Transfer = BIT(10);
        constexpr static ui32 Bottom = BIT(11);

		Stage(ui32 value): Enum<ui32>(value) {}
	};
}

#endif /* NP_ENGINE_GPU_INTERFACE_STAGE_HPP */