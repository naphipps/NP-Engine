//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/5/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_SAMPLER_RESOURCE_HPP
#define NP_ENGINE_GPU_INTERFACE_SAMPLER_RESOURCE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "Device.hpp"
#include "Resource.hpp"
#include "CompareOperation.hpp"

namespace np::gpu
{
	class SamplerBorder : public enm_ui32
	{
	public:
		constexpr static ui32 Transparent = BIT(0);
		constexpr static ui32 Opaque = BIT(1);
		constexpr static ui32 Black = BIT(2);
		constexpr static ui32 White = BIT(3);

		SamplerBorder(ui32 value): enm_ui32(value) {}
	};

	class SamplerAddressMode : public enm_ui32
	{
	public:
		constexpr static ui32 Repeat = BIT(0);
		constexpr static ui32 Mirror = BIT(1);
		constexpr static ui32 Clamp = BIT(2);
		constexpr static ui32 Edge = BIT(3);
		constexpr static ui32 Border = BIT(4);

		SamplerAddressMode(ui32 value): enm_ui32(value) {}
	};

	struct SamplerAddressModes
	{
		SamplerAddressMode u = SamplerAddressMode::None;
		SamplerAddressMode v = SamplerAddressMode::None;
		SamplerAddressMode w = SamplerAddressMode::None;
	};

	class SamplerResourceUsage : public ResourceUsage
	{
	public:
		constexpr static ui32 MagnifyNearest = BIT(0);
		constexpr static ui32 MagnifyLinear = BIT(1);
		constexpr static ui32 MinimizeNearest = BIT(2);
		constexpr static ui32 MinimizeLinear = BIT(3);
		constexpr static ui32 MipmapNearest = BIT(4);
		constexpr static ui32 MipmapLinear = BIT(5);
		constexpr static ui32 UnnormalizedCoordinates = BIT(6);

		SamplerResourceUsage(ui32 value): ResourceUsage(value) {}
	};

	using LodBounds = mat::DblRange;

	struct SamplerResource : public Resource
	{
		static mem::sptr<SamplerResource> Create(mem::sptr<Device> device, SamplerResourceUsage usage, dbl anisotrophy,
												 CompareOperation op, LodBounds lod_bounds, SamplerBorder border,
												 SamplerAddressModes address_modes); //TODO: accept formats? memory flags?

		virtual ~SamplerResource() = default;

		virtual mem::sptr<Device> GetDevice() const = 0;

		virtual ResourceType GetResourceType() const override
		{
			return ResourceType::Sampler;
		}

		virtual dbl GetAnisotrophy() const = 0;

		virtual CompareOperation GetCompareOperation() const = 0;

		virtual LodBounds GetLodBounds() const = 0;

		virtual SamplerBorder GetBorder() const = 0;

		virtual SamplerAddressModes GetAddressModes() const = 0;
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_SAMPLER_RESOURCE_HPP */