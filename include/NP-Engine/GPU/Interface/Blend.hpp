//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/11/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_BLEND_HPP
#define NP_ENGINE_GPU_INTERFACE_BLEND_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "Logic.hpp"
#include "Color.hpp"

namespace np::gpu
{
	class BlendScalar : public Enum<ui32>
	{
	public:
		constexpr static ui32 Zeroed = BIT(0); //Zero already exists ergo past tense
		constexpr static ui32 Oned = BIT(1);

		constexpr static ui32 Src = BIT(2);
		constexpr static ui32 Dst = BIT(3);

		constexpr static ui32 OneMinus = BIT(4);
		constexpr static ui32 Color = BIT(5);
		constexpr static ui32 Alpha = BIT(6);
		constexpr static ui32 Constant = BIT(7);
		constexpr static ui32 Saturate = BIT(8);

		//constexpr static ui32 Src1 = BIT(0); //TODO: investigate

		BlendScalar(ui32 value): Enum<ui32>(value) {}
	};

	class BlendOperation : public Enum<ui32>
	{
	public:
		constexpr static ui32 Add = BIT(0);
		constexpr static ui32 Subtract = BIT(1);
		constexpr static ui32 Reverse = BIT(2);
		constexpr static ui32 Min = BIT(3);
		constexpr static ui32 Max = BIT(4);

		BlendOperation(ui32 value): Enum<ui32>(value) {}
	};

	struct Blending
	{
		BlendScalar srcScalar = BlendScalar::None;
		BlendScalar dstScalar = BlendScalar::None;
		BlendOperation op = BlendOperation::None;
	};

	struct ResourceBlending
	{
		bl enable = false; //TODO: surely we can get rid of this? This might become BlendingUsage?
		Blending color{};
		Blending alpha{};
		ColorChannel colorChannel = ColorChannel::None;

		bl IsEnabled() const
		{
			return color.srcScalar != BlendScalar::None || color.dstScalar != BlendScalar::None ||
				color.op != BlendOperation::None || alpha.srcScalar != BlendScalar::None ||
				alpha.dstScalar != BlendScalar::None || alpha.op != BlendOperation::None;
		}
	};

	struct BlendConstants
	{
		//TODO: these need to be dbl here
		flt a; //TODO: whatever these need to be
		flt b;
		flt c;
		flt d;
	};

	struct Blend
	{
		LogicOperation logicOperation = LogicOperation::None;
		con::vector<ResourceBlending> resourceBlendings{};
		BlendConstants constants{};
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_BLEND_HPP */