//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/11/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_RASTERIZATION_HPP
#define NP_ENGINE_GPU_INTERFACE_RASTERIZATION_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::gpu
{
	class RasterizationUsage : public enm_ui32
	{
	public:
		constexpr static ui32 Disable = BIT(0);
		constexpr static ui32 Discard = Disable;

		constexpr static ui32 PolygonFill = BIT(1);
		constexpr static ui32 PolygonLine = BIT(2); //usefull for drawing wireframes (culling still happens)
		constexpr static ui32 PolygonPoint = BIT(3);

		constexpr static ui32 CullFront = BIT(4);
		constexpr static ui32 CullBack = BIT(5);

		constexpr static ui32 FrontFaceClockwise = BIT(6); //default is counter clockwise

		constexpr static ui32 DepthClamp =
			BIT(7); //TODO: this may be resolved via the depthBiasClamp? probably not though (pg 258)

		RasterizationUsage(ui32 value): enm_ui32(value) {}
	};

	struct DepthBias
	{
		dbl constantScalar = 0;
		dbl clamp = 0;
		dbl slopeScalar = 0;
	};

	struct Rasterization
	{
		RasterizationUsage usage = RasterizationUsage::None;
		DepthBias bias{};
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_RASTERIZATION_HPP */