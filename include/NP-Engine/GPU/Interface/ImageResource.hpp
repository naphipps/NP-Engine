//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/6/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_IMAGE_RESOURCE_HPP
#define NP_ENGINE_GPU_INTERFACE_IMAGE_RESOURCE_HPP

#include <utility>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Math/Math.hpp"

#include "NP-Engine/Vendor/LodePngInclude.hpp"
// TODO: I feel like we could add back stb_image, and try lodepng when stb_image fails on pngs

#include "Color.hpp"
#include "Resource.hpp"
#include "Format.hpp"
#include "Device.hpp"

namespace np::gpu
{
	class ImageResourceUsage : public ResourceUsage
	{
	public:
		constexpr static ui32 Read = BIT(0);
		constexpr static ui32 Write = BIT(1);

		constexpr static ui32 HostAccessible = BIT(2);
		constexpr static ui32 Sparse = BIT(3);
		constexpr static ui32 Dynamic = BIT(4); //TODO: are these dynamic images? I don't think so
		constexpr static ui32 Cube = BIT(5);
		constexpr static ui32 Present = BIT(6);

		constexpr static ui32 General = BIT(7);
		constexpr static ui32 Color = BIT(8);
		constexpr static ui32 Storage = BIT(9);
		constexpr static ui32 Depth = BIT(10);
		constexpr static ui32 Stencil = BIT(11);
		constexpr static ui32 Transfer = BIT(12);
		constexpr static ui32 Shader = BIT(13);

		ImageResourceUsage(ui32 value): ResourceUsage(value) {}
	};

	struct ImageResourceDescription
	{
		Format format = Format::None;
		siz sampleCount = 1; //if not multisampling then use one
		ResourceOperation readOperation = ResourceOperation::None;
		ResourceOperation writeOperation = ResourceOperation::None;
		ResourceOperation stencilReadOperation = ResourceOperation::None;
		ResourceOperation stencilWriteOperation = ResourceOperation::None;
		ImageResourceUsage initialUsage = ImageResourceUsage::None; //TODO: maybe rename this? This is what the render pass
																	//shall expect the image's usage/layout to be
		ImageResourceUsage finalUsage = ImageResourceUsage::None; //TODO: maybe rename this? this is the usage/layout that the
																  //render pass shall move the image to at the end
	};

	struct ImageResource : public Resource
	{
		using Point = ::glm::uvec3;

		static mem::sptr<ImageResource> Create(mem::sptr<Device> device, ImageResourceUsage usage, Format format, siz mip_count,
											   siz layer_count, siz sample_count, siz width, siz height, siz depth,
											   const con::vector<DeviceQueueFamily>& queue_families);

		virtual ~ImageResource() = default;

		virtual ResourceType GetResourceType() const override
		{
			return ResourceType::Image;
		}

		virtual mem::sptr<Device> GetDevice() const = 0;

		virtual Format GetFormat() const = 0;

		virtual siz GetMipCount() const = 0;

		virtual siz GetLayerCount() const = 0;

		virtual siz GetSampleCount() const = 0;

		virtual siz GetWidth() const = 0;

		virtual siz GetHeight() const = 0;

		virtual siz GetDepth() const = 0;

		virtual con::vector<DeviceQueueFamily> GetDeviceQueueFamilies() const = 0;
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_IMAGE_RESOURCE_HPP */