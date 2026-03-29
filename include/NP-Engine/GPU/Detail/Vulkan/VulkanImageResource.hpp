//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/1/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_IMAGE_RESOURCE_HPP
#define NP_ENGINE_GPU_VULKAN_IMAGE_RESOURCE_HPP

#include <utility>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanDevice.hpp"
#include "VulkanDeviceMemory.hpp"
#include "VulkanFormat.hpp"
#include "VulkanSampleCount.hpp"
#include "VulkanResource.hpp"

namespace np::gpu::__detail
{
	class VulkanImageResourceUsage : public ImageResourceUsage
	{
	public:
		VulkanImageResourceUsage(ui32 value): ImageResourceUsage(value) {}

		VkImageLayout GetVkImageLayout() const
		{
			VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;

			switch (_value)
			{
			case Present:
				layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
				break;
			case Transfer | Read:
				layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				break;
			case Transfer | Write:
				layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				break;
			case Color:
				layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				break;
			case General:
				layout = VK_IMAGE_LAYOUT_GENERAL;
				break;
			case Shader:
				layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				break;
			case Depth | Stencil | Read:
			case Depth | Read:
			case Stencil | Read:
				layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
				break;
			case Depth | Stencil | Write:
			case Depth | Write:
			case Stencil | Write:
				layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				break;
			}

			return layout;
		}

		VkImageCreateFlags GetVkImageCreateFlags() const
		{
			/*
				typedef enum VkImageCreateFlagBits {
					VK_IMAGE_CREATE_SPARSE_BINDING_BIT = 0x00000001,
					VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT = 0x00000002,
					VK_IMAGE_CREATE_SPARSE_ALIASED_BIT = 0x00000004,
					VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT = 0x00000008,
					VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT = 0x00000010,
					VK_IMAGE_CREATE_ALIAS_BIT = 0x00000400,
					VK_IMAGE_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT = 0x00000040,
					VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT = 0x00000020,
					VK_IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE_BIT = 0x00000080,
					VK_IMAGE_CREATE_EXTENDED_USAGE_BIT = 0x00000100,
					VK_IMAGE_CREATE_PROTECTED_BIT = 0x00000800,
					VK_IMAGE_CREATE_DISJOINT_BIT = 0x00000200,

					VK_IMAGE_CREATE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
				} VkImageCreateFlagBits;
				typedef VkFlags VkImageCreateFlags;
			*/

			VkImageCreateFlags flags = 0;
			//TODO: implement this
			return flags;
		}

		VkImageUsageFlags GetVkImageUsageFlags() const
		{
			//TODO: I have seen where our memory property flag value may/can affect our vkBindImageMemory, and our usage might
			//need to consider HostAccessible
			//TODO: ^ <https://www.reddit.com/r/vulkan/comments/a4rx16/what_does_vkbindimagememory_really_does/>

			const bl contains_read = Contains(Read);
			const bl contains_write = Contains(Write);
			VkImageUsageFlags flags = 0;

			if (Contains(Transfer))
			{
				if (contains_read || !contains_write)
					flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
				if (contains_write || !contains_read)
					flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			}

			if (Contains(Storage))
				flags |= VK_IMAGE_USAGE_STORAGE_BIT;
			if (Contains(Color))
				flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			if (ContainsAny(Depth | Stencil))
				flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			if (Contains(Shader))
				flags |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;

			//TODO: resolve VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT?
			//TODO: resolve VK_IMAGE_USAGE_SAMPLED_BIT?

			return flags;
		}

		VkMemoryPropertyFlags GetVkMemoryPropertyFlags() const
		{
			return Contains(HostAccessible) ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
											: VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		}

		VkImageTiling GetVkImageTiling() const
		{
			return Contains(HostAccessible) ? VK_IMAGE_TILING_LINEAR : VK_IMAGE_TILING_OPTIMAL;
		}

		VkImageAspectFlags GetVkAspectFlags() const
		{
			VkImageAspectFlags flags = VK_IMAGE_ASPECT_NONE;

			if (Contains(Depth))
				flags |= VK_IMAGE_ASPECT_DEPTH_BIT;
			if (Contains(Stencil))
				flags |= VK_IMAGE_ASPECT_STENCIL_BIT;
			if (Contains(Color))
				flags |= VK_IMAGE_ASPECT_COLOR_BIT;

			return flags;
		}
	};

	struct VulkanImageResourceDescription
	{
		VulkanFormat format = Format::None;
		ui32 sampleCount = 1; //if not multisampling then use one
		VulkanResourceOperation readOperation = VulkanResourceOperation::None;
		VulkanResourceOperation writeOperation = VulkanResourceOperation::None;
		VulkanResourceOperation stencilReadOperation = VulkanResourceOperation::None;
		VulkanResourceOperation stencilWriteOperation = VulkanResourceOperation::None;
		VulkanImageResourceUsage initialUsage = VulkanImageResourceUsage::None;
		VulkanImageResourceUsage finalUsage = VulkanImageResourceUsage::None;

		VulkanImageResourceDescription(const ImageResourceDescription& description):
			format(description.format),
			sampleCount(description.sampleCount),
			readOperation(description.readOperation),
			writeOperation(description.writeOperation),
			stencilReadOperation(description.stencilReadOperation),
			stencilWriteOperation(description.stencilWriteOperation),
			initialUsage(description.initialUsage),
			finalUsage(description.finalUsage)
		{}

		operator ImageResourceDescription() const
		{
			return {format,		  sampleCount, readOperation, writeOperation, stencilReadOperation, stencilWriteOperation,
					initialUsage, finalUsage};
		}

		VkAttachmentDescription GetVkAttachmentDescription() const
		{
			VkAttachmentDescription description{};
			description.format = format.GetVkFormat();
			description.samples = VulkanSampleCount{sampleCount}.GetVkSampleCountFlagBits();
			description.loadOp = readOperation.GetVkAttachmentLoadOp();
			description.storeOp = writeOperation.GetVkAttachmentStoreOp();
			description.stencilLoadOp = stencilReadOperation.GetVkAttachmentLoadOp();
			description.stencilStoreOp = stencilWriteOperation.GetVkAttachmentStoreOp();
			description.initialLayout = initialUsage.GetVkImageLayout();
			description.finalLayout = finalUsage.GetVkImageLayout();
			return description;
		}
	};

	class VulkanImageResource : public ImageResource
	{
	private:
		mem::sptr<VulkanDevice> _device;
		VulkanFormat _format;
		ui32 _mip_count;
		ui32 _layer_count;
		ui32 _width;
		ui32 _height;
		ui32 _depth;
		VulkanSampleCount _sample_count;
		con::vector<DeviceQueueFamily> _queue_families;
		VkImage _image;
		mem::sptr<VulkanDeviceMemory> _memory; //TODO: not a good idea to allocate this per image, use a device-memory-allocator
											   //(guide pg85 mentions this - source code not available)
		const bl _enable_destroy;

		static VkImageType GetVkImageType(VkExtent3D extent)
		{
			return extent.depth == 1 ? (extent.height == 1 ? VK_IMAGE_TYPE_1D : VK_IMAGE_TYPE_2D) : VK_IMAGE_TYPE_3D;
		}

		static VkExtent3D GetVkExtent3D(ui32 width, ui32 height, ui32 depth)
		{
			return {width, height, depth};
		}

		static VkImageCreateInfo CreateVkInfo(VulkanImageResourceUsage usage, VulkanFormat format, ui32 mip_count,
											  ui32 layer_count, ui32 width, ui32 height, ui32 depth)
		{
			VkImageCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

			info.tiling = usage.GetVkImageTiling();
			info.extent = GetVkExtent3D(width, height, depth);
			info.format = format.GetVkFormat();
			info.mipLevels = mip_count;
			info.arrayLayers = layer_count;
			info.usage = usage.GetVkImageUsageFlags();
			info.flags = usage.GetVkImageCreateFlags();

			info.imageType = GetVkImageType(info.extent);
			return info;
		}

		static VkImage CreateVkImage(mem::sptr<VulkanDevice> device, VulkanImageResourceUsage usage, VulkanFormat format,
									 ui32 mip_count, ui32 layer_count, VulkanSampleCount sample_count, ui32 width, ui32 height,
									 ui32 depth, const con::vector<DeviceQueueFamily>& queue_families)
		{
			con::vector<ui32> family_indices(queue_families.size());
			for (siz i = 0; i < family_indices.size(); i++)
				family_indices[i] = queue_families[i].index;

			VkImageCreateInfo info = CreateVkInfo(usage, format, mip_count, layer_count, width, height, depth);
			info.sharingMode = family_indices.size() > 1 ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
			info.queueFamilyIndexCount = family_indices.size();
			info.pQueueFamilyIndices = family_indices.empty() ? nullptr : family_indices.data();
			info.samples = sample_count.GetVkSampleCountFlagBits();

			mem::sptr<VulkanInstance> instance = device->GetDetailInstance();
			VkImage image = nullptr;
			VkResult result = vkCreateImage(*device->GetLogicalDevice(), &info, instance->GetVulkanAllocationCallbacks(), &image);
			return result == VK_SUCCESS ? image : nullptr;
		}

		static VkMemoryRequirements GetVkMemoryRequirements(mem::sptr<VulkanDevice> device, VkImage image)
		{
			VkMemoryRequirements requirements{};
			vkGetImageMemoryRequirements(*device->GetLogicalDevice(), image, &requirements);
			//vkGetImageSparseMemoryRequirements(*device, image, &count, nullptr); //TODO: support sparse images (guide pg85)
			//(don't think there are sparse buffers
			return requirements;
		}

		static VulkanSampleCount SanitizeSampleCount(mem::sptr<VulkanDevice> device, VulkanImageResourceUsage usage,
													 VulkanFormat format, VulkanSampleCount sample_count, ui32 width,
													 ui32 height, ui32 depth)
		{
			const VulkanPhysicalDevice physical_device = device->GetLogicalDevice()->GetPhysicalDevice();
			const VkSampleCountFlagBits bits = physical_device.DetermineVkSampleCountFlagBits(
				sample_count, format.GetVkFormat(), GetVkImageType(GetVkExtent3D(width, height, depth)),
				usage.GetVkImageTiling(), usage.GetVkImageUsageFlags(), usage.GetVkImageCreateFlags());

			siz index = SIZ_MAX;
			for (siz i = 0; index == SIZ_MAX && i < VulkanSampleCount::AllVkSampleCountFlagBitsArray.size(); i++)
				if (bits & VulkanSampleCount::AllVkSampleCountFlagBitsArray[i])
					index = i;
			return BIT(index);
		}

		void BindMemory()
		{
			vkBindImageMemory(*_device->GetLogicalDevice(), _image, *_memory, 0);
		}

	public:
		static mem::sptr<VulkanImageResource> Create(mem::sptr<Device> device, ImageResourceUsage usage, VkImage image,
													 Format format, siz mip_count, siz layer_count, siz sample_count, siz width,
													 siz height, siz depth,
													 const con::vector<DeviceQueueFamily>& queue_families)
		{
			return mem::create_sptr<VulkanImageResource>(device->GetServices()->GetAllocator(), device, usage, image, format,
														 mip_count, layer_count, sample_count, width, height, depth,
														 queue_families);
		}

		VulkanImageResource(mem::sptr<Device> device, ImageResourceUsage usage, VkImage image, Format format, siz mip_count,
							siz layer_count, siz sample_count, siz width, siz height, siz depth,
							const con::vector<DeviceQueueFamily>& queue_families):
			_device(device),
			_format(format),
			_mip_count(mip_count),
			_layer_count(layer_count),
			_sample_count(sample_count),
			_width(width),
			_height(height),
			_depth(depth),
			_queue_families{queue_families.begin(), queue_families.end()},
			_image(image),
			_enable_destroy(false)
		{
			const VulkanImageResourceUsage vulkan_usage = (ui32)usage;
			_memory =
				_device->CreateDeviceMemory(GetVkMemoryRequirements(_device, _image), vulkan_usage.GetVkMemoryPropertyFlags());
			//BindMemory();
		}

		VulkanImageResource(mem::sptr<Device> device, ImageResourceUsage usage, Format format, siz mip_count, siz layer_count,
							siz sample_count, siz width, siz height, siz depth,
							const con::vector<DeviceQueueFamily>& queue_families):
			_format(VulkanFormat::None),
			_enable_destroy(true)
		{
			const VulkanImageResourceUsage vulkan_usage = (ui32)usage;

			if (vulkan_usage.Contains(VulkanImageResourceUsage::Cube)) //TODO: consider VulkanImageUsage::Cube. Is there
																	   //anything else we need to do?
			{
				layer_count = 6;
				width = ::std::min(width, height);
				height = width;
				depth = ::std::min((siz)1, depth); //overkill, but keeps the caller's value if invalid hehehe >:3
			}

			_device = device;
			_format = (ui32)format;
			_mip_count = mip_count;
			_layer_count = layer_count;
			_width = width;
			_height = height;
			_depth = depth;
			_sample_count = SanitizeSampleCount(_device, vulkan_usage, _format, sample_count, _width, _height, _depth);
			_queue_families = {queue_families.begin(), queue_families.end()};

			_image = CreateVkImage(_device, vulkan_usage, _format, _mip_count, _layer_count, _sample_count, _width, _height,
								   _depth, queue_families);
			_memory =
				_device->CreateDeviceMemory(GetVkMemoryRequirements(_device, _image), vulkan_usage.GetVkMemoryPropertyFlags());
			BindMemory();
		}

		virtual ~VulkanImageResource()
		{
			_memory.reset();

			if (_enable_destroy && _image)
			{
				mem::sptr<VulkanInstance> instance = _device->GetDetailInstance();
				vkDestroyImage(*_device->GetLogicalDevice(), _image, instance->GetVulkanAllocationCallbacks());
				_image = nullptr;
			}
		}

		operator VkImage() const
		{
			return _image;
		}

		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual mem::sptr<srvc::Services> GetServices() const override
		{
			return _device->GetServices();
		}

		virtual ResourceType GetResourceType() const override
		{
			return ResourceType::Image;
		}

		virtual mem::sptr<Device> GetDevice() const override
		{
			return _device;
		}

		virtual Format GetFormat() const override
		{
			return _format;
		}

		virtual siz GetMipCount() const override
		{
			return _mip_count;
		}

		virtual siz GetLayerCount() const override
		{
			return _layer_count;
		}

		virtual siz GetSampleCount() const override
		{
			return _sample_count;
		}

		virtual siz GetWidth() const override
		{
			return _width;
		}

		virtual siz GetHeight() const override
		{
			return _height;
		}

		virtual siz GetDepth() const override
		{
			return _depth;
		}

		virtual con::vector<DeviceQueueFamily> GetDeviceQueueFamilies() const override
		{
			return _queue_families;
		}

		const mem::sptr<VulkanDeviceMemory> GetDeviceMemory() const
		{
			return _memory;
		}

		VkExtent3D GetVkExtent3D() const
		{
			return GetVkExtent3D(_width, _height, _depth);
		}

		VkImageType GetVkImageType() const
		{
			return GetVkImageType(GetVkExtent3D());
		}

		VkImageViewType GetVkImageViewType() const
		{
			VkImageViewType view_type = VK_IMAGE_VIEW_TYPE_MAX_ENUM;

			//TODO: support cube types
			/*if (_usage.Contains(VulkanImageResourceUsage::Cube))
			{
				view_type = VK_IMAGE_VIEW_TYPE_CUBE;
			}*/

			switch (GetVkImageType())
			{
			case VK_IMAGE_TYPE_1D:
				view_type = VK_IMAGE_VIEW_TYPE_1D;
				break;
			case VK_IMAGE_TYPE_2D:
				view_type = VK_IMAGE_VIEW_TYPE_2D;
				break;
			case VK_IMAGE_TYPE_3D:
				view_type = VK_IMAGE_VIEW_TYPE_3D;
				break;
			}

			return view_type;
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_IMAGE_RESOURCE_HPP */