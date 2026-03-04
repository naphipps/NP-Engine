//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/11/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_DEPTH_STENCIL_HPP
#define NP_ENGINE_GPU_INTERFACE_DEPTH_STENCIL_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Math/Math.hpp"

#include "CompareOperation.hpp"

namespace np::gpu
{
	class DepthStencilUsage : public Enum<ui32>
	{
	public:
		constexpr static ui32 DepthTest = BIT(0);
		constexpr static ui32 DepthWrite = BIT(1);
		constexpr static ui32 BoundsTest = BIT(2); //TODO: not every detail supports this -- check with device?
		constexpr static ui32 StencilTest = BIT(3);

		DepthStencilUsage(ui32 value): Enum<ui32>(value) {}
	};

	class StencilOperation : public Enum<ui32>
	{
	public:
		constexpr static ui32 Keep = BIT(0);
		constexpr static ui32 Zero = BIT(1);
		constexpr static ui32 Replace = BIT(2);
		constexpr static ui32 Invert = BIT(3);

		constexpr static ui32 Wrap = BIT(4);
		constexpr static ui32 Clamp = BIT(5);

		constexpr static ui32 Increment = BIT(6);
		constexpr static ui32 Decrement = BIT(7);

		StencilOperation(ui32 value): Enum<ui32>(value) {}
	};

	struct StencilOperationState
	{
		StencilOperation depthFailOperation = StencilOperation::None;
		StencilOperation stencilFailOperation = StencilOperation::None;
		StencilOperation stencilPassOperation = StencilOperation::None;

		CompareOperation compareOperation = CompareOperation::None;

		siz compareMask = 0; //TODO: finish implementing - what are these?
		siz writeMask = 0;
		siz referenceValue = 0; //used in compareOperation and if any StencilOperation is set to Replace
	};

	using DepthBounds = mat::DblRange;

	class StencilFace : public Enum<ui32>
	{
	public:
		constexpr static ui32 Front = BIT(0);
		constexpr static ui32 Back = BIT(1);

		StencilFace(ui32 value): Enum<ui32>(value) {}
	};

	struct DepthStencil
	{
		DepthStencilUsage usage = DepthStencilUsage::None;
		CompareOperation depthCompareOperation = CompareOperation::None;
		StencilOperationState frontState{};
		StencilOperationState backState{};
		DepthBounds depthBounds{};
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_DEPTH_STENCIL_HPP */