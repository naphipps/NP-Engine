//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/7/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_IMAGE_RESOURCE_VIEW_HPP
#define NP_ENGINE_GPU_INTERFACE_IMAGE_RESOURCE_VIEW_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "Resource.hpp"
#include "ImageResource.hpp"

namespace np::gpu
{
	struct ImageResourceView : public ResourceView //TODO: I don't know if we want to keep views in our interface? depends if a
												   //buffer view becomes useful, which is very well might
	{
		static mem::sptr<ImageResourceView> Create(mem::sptr<Device> device, mem::sptr<ImageResource> image,
												   ImageResourceUsage usage);

		virtual ~ImageResourceView() = default;

		virtual ResourceType GetResourceType() const override
		{
			return ResourceType::Image | ResourceType::View;
		}

		virtual mem::sptr<Device> GetDevice() const = 0;

		virtual mem::sptr<ImageResource> GetImageResource() const = 0;
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_IMAGE_RESOURCE_VIEW_HPP */