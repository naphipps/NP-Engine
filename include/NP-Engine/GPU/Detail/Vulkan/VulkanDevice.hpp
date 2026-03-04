//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/5/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_DEVICE_HPP
#define NP_ENGINE_GPU_VULKAN_DEVICE_HPP

#include "NP-Engine/Services/Services.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanInstance.hpp"
#include "VulkanPresentTarget.hpp"
#include "VulkanLogicalDevice.hpp"
#include "VulkanPipelineCache.hpp"
#include "VulkanDeviceMemory.hpp"
#include "VulkanFence.hpp"
#include "VulkanSemaphore.hpp"

namespace np::gpu::__detail
{
	class VulkanDevice : public Device
	{
	private:
		mem::sptr<VulkanPresentTarget> _target;
		VkSurfaceFormatKHR _surface_format;
		VkPresentModeKHR _present_mode;
		mem::sptr<VulkanLogicalDevice> _logical_device;

		static ::std::optional<VulkanPhysicalDevice> ChoosePhysicalDevice(mem::sptr<VulkanInstance> instance, DeviceUsage usage, mem::sptr<VulkanPresentTarget> target)
		{
			::std::optional<VulkanPhysicalDevice> device;
			if (target)
			{
				using Candidate = ::std::pair<siz, VulkanPhysicalDevice>; // <score, physical_device>
				con::vector<VulkanPhysicalDevice> devices = VulkanPhysicalDevice::GetAllVulkanPhysicalDevices(instance);
				con::vector<Candidate> candidates;

				for (VulkanPhysicalDevice d : devices)
					candidates.emplace_back(d.GetUsageScore(usage, target), d);

				::std::sort(candidates.begin(), candidates.end(),
							[](const Candidate& a, const Candidate& b)
							{
								return a.first > b.first; // descending sort
							});

				if (!candidates.empty() && candidates.front().first > 0)
					device = candidates.front().second;
			}
			return device;
		}

		static con::vector<VkDeviceQueueCreateInfo> CreateVkQueueCreateInfos(VulkanPhysicalDevice physical_device, DeviceUsage usage, mem::sptr<PresentTarget> target)
		{
			const con::vector<VkQueueFamilyProperties> properties = physical_device.GetVkQueueFamilyProperties();
			con::oset<ui32> indices{};

			for (siz i = 0; i < properties.size(); i++)
			{
				if (usage.Contains(DeviceUsage::Present) && physical_device.QueueFamilySupportsPresent(properties, i, target))
					indices.emplace((ui32)i);

				if (usage.Contains(DeviceUsage::Graphics) && physical_device.QueueFamilySupportsGraphics(properties, i))
					indices.emplace((ui32)i);

				if (usage.Contains(DeviceUsage::Compute) && physical_device.QueueFamilySupportsCompute(properties, i))
					indices.emplace((ui32)i);
			}

			con::vector<VkDeviceQueueCreateInfo> infos;
			for (ui32 index : indices)
			{
				VkDeviceQueueCreateInfo info = VulkanLogicalDevice::CreateVkDeviceQueueInfo();
				info.queueFamilyIndex = index;
				infos.emplace_back(info);
			}
			return infos;
		}

	public:
		VulkanDevice(mem::sptr<DetailInstance> instance, DeviceUsage usage, mem::sptr<PresentTarget> target) :
			_target(target),
			_surface_format(),
			_present_mode(),
			_logical_device(nullptr)
		{
			mem::sptr<VulkanInstance> vulkan_instance = DetailObject::EnsureIsDetailType(instance, DetailType::Vulkan);
			if (vulkan_instance)
			{
				const ::std::optional<VulkanPhysicalDevice> physical_device_optional = ChoosePhysicalDevice(vulkan_instance, usage, _target);
				if (physical_device_optional.has_value())
				{
					const VulkanPhysicalDevice physical_device = physical_device_optional.value();
					_surface_format = physical_device.ChooseVkSurfaceFormat(_target);
					_present_mode = physical_device.ChooseVkPresentMode(_target);
					mem::sptr<srvc::Services> services = physical_device.GetServices();
					_logical_device = mem::create_sptr<VulkanLogicalDevice>(services->GetAllocator(),
						physical_device, CreateVkQueueCreateInfos(physical_device, usage, _target));
				}
			}
		}

		virtual ~VulkanDevice() = default;

		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual mem::sptr<srvc::Services> GetServices() const override
		{
			return _logical_device->GetServices();
		}

		virtual mem::sptr<DetailInstance> GetDetailInstance() const override
		{
			return _logical_device->GetPhysicalDevice().GetDetailInstance();
		}

		virtual mem::sptr<PresentTarget> GetPresentTarget() const override
		{
			return _target;
		}

		virtual con::vector<DeviceQueueFamily> GetDeviceQueueFamilies() const override
		{
			const VulkanPhysicalDevice physical_device = _logical_device->GetPhysicalDevice();
			const con::vector<VkQueueFamilyProperties> properties = physical_device.GetVkQueueFamilyProperties();
			con::vector<DeviceQueueFamily> families{};

			for (siz i=0; i<properties.size(); i++)
			{
				DeviceQueueUsage usage = DeviceQueueUsage::None;

				if (physical_device.QueueFamilySupportsPresent(properties, i, _target))
					usage |= DeviceQueueUsage::Present;

				if (physical_device.QueueFamilySupportsGraphics(properties, i))
					usage |= DeviceQueueUsage::Graphics;

				if (physical_device.QueueFamilySupportsCompute(properties, i))
					usage |= DeviceQueueUsage::Compute;

				families.emplace_back(DeviceQueueFamily{ i, properties[i].queueCount, usage });
			}

			return families;
		}

		virtual mem::sptr<PipelineCache> GetPipelineCache() const override
		{
			return mem::create_sptr<VulkanPipelineCache>(GetServices()->GetAllocator(), GetLogicalDevice(), con::vector<ui8>{});
		}

		virtual mem::sptr<Fence> CreateFence() override
		{
			return mem::create_sptr<VulkanFence>(GetServices()->GetAllocator(), GetLogicalDevice());
		}

		virtual mem::sptr<Semaphore> CreateSemaphore() override
		{
			return mem::create_sptr<VulkanSemaphore>(GetServices()->GetAllocator(), GetLogicalDevice());
		}





		VkExtent2D ChooseVkExtent2D() const
		{
			const VulkanPhysicalDevice physical_device = GetLogicalDevice()->GetPhysicalDevice();
			const VkSurfaceCapabilitiesKHR capabilities = physical_device.GetVkSurfaceCapabilities(_target);
			VkExtent2D extent{};

			if (capabilities.currentExtent.width != UI32_MAX)
			{
				extent = capabilities.currentExtent;
			}
			else
			{
				const VkExtent2D framebuffer_extent = _target->GetFramebufferExtent();
				const VkExtent2D min_extent = capabilities.minImageExtent;
				const VkExtent2D max_extent = capabilities.maxImageExtent;

				extent = { ::std::clamp(framebuffer_extent.width, min_extent.width, max_extent.width),
						  ::std::clamp(framebuffer_extent.height, min_extent.height, max_extent.height) };
			}

			return extent;
		}




		mem::sptr<VulkanLogicalDevice> GetLogicalDevice() const
		{
			return _logical_device;
		}

		VkSurfaceFormatKHR GetVkSurfaceFormat() const
		{
			return _surface_format;
		}

		VkPresentModeKHR GetVkPresentMode() const
		{
			return _present_mode;
		}

		mem::sptr<VulkanDeviceMemory> CreateDeviceMemory(VkMemoryRequirements requirements, VkMemoryPropertyFlags flags)
		{
			return mem::create_sptr<VulkanDeviceMemory>(GetServices()->GetAllocator(), _logical_device, requirements, flags);
		}













		//VkResult AsyncCopy(mem::sptr<VulkanBuffer> dst, mem::sptr<VulkanBuffer> src, VkSubmitInfo& submit_info, mem::sptr<VulkanQueue> queue,
		//					mem::sptr<VulkanCommandPool> pool, con::vector<mem::sptr<VulkanCommandBuffer>>& buffers, VkFence fence)
		//{
		//	NP_ENGINE_ASSERT(queue->Has(pool), "command pool must pertain to queue");
		//	VulkanCopyBufferCommand copy_buffer{ dst, src, {{0, 0, src->GetSize()}} };

		//	mem::sptr<VulkanCommandBuffer> buffer = pool->AllocateCommandBuffer();
		//	buffer->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		//	buffer->Add(copy_buffer);
		//	buffer->End();
		//	buffers.emplace_back(buffer);
		//	return queue->Submit({ buffer }, submit_info, fence);
		//}

		//VkResult SyncCopy(mem::sptr<VulkanBuffer> dst, mem::sptr<VulkanBuffer> src, VkSubmitInfo& submit_info, mem::sptr<VulkanQueue> queue, mem::sptr<VulkanCommandPool> pool)
		//{
		//	con::vector<mem::sptr<VulkanCommandBuffer>> buffers;
		//	VulkanFence fence{ GetLogicalDevice() };
		//	fence.Reset();
		//	VkResult result = AsyncCopy(dst, src, submit_info, queue, pool, buffers, fence);
		//	fence.Wait();
		//	pool->DeallocateCommandBuffers(buffers);
		//	return result;
		//}

		//VkResult AsyncCopy(VulkanImage& dst, const VulkanBuffer& src, VkBufferImageCopy buffer_image_copy,
		//				   VkSubmitInfo& submit_info, mem::sptr<VulkanQueue> queue, mem::sptr<VulkanCommandPool> pool,
		//				   con::vector<mem::sptr<VulkanCommandBuffer>>& buffers, VkFence fence)
		//{
		//	NP_ENGINE_ASSERT(queue->Has(pool), "command pool must pertain to queue");

		//	VulkanCopyBufferToImageCommand copy_buffer_to_image{ dst, src, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, {buffer_image_copy} };

		//	mem::sptr<VulkanCommandBuffer> buffer = pool->AllocateCommandBuffer();
		//	buffer->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		//	buffer->Add(copy_buffer_to_image);
		//	buffer->End();
		//	buffers.emplace_back(buffer);
		//	return queue->Submit({ buffer }, submit_info, fence);
		//}

		//VkResult SyncCopy(VulkanImage& dst, const VulkanBuffer& src, VkBufferImageCopy buffer_image_copy,
		//				  VkSubmitInfo& submit_info, mem::sptr<VulkanQueue> queue, mem::sptr<VulkanCommandPool> pool)
		//{
		//	con::vector<mem::sptr<VulkanCommandBuffer>> buffers;
		//	VulkanFence fence(GetLogicalDevice());
		//	fence.Reset();
		//	VkResult result = AsyncCopy(dst, src, buffer_image_copy, submit_info, queue, pool, buffers, fence);
		//	fence.Wait();
		//	pool->DeallocateCommandBuffers(buffers);
		//	return result;
		//}

		//VkResult AsyncTransition(VulkanImage& image, VkImageLayout old_image_layout,
		//						 VkImageLayout new_image_layout, VkSubmitInfo& submit_info, mem::sptr<VulkanQueue> queue, mem::sptr<VulkanCommandPool> pool,
		//						 con::vector<mem::sptr<VulkanCommandBuffer>>& buffers, VkFence fence)
		//{
		//	NP_ENGINE_ASSERT(queue->Has(pool), "command pool must pertain to queue");

		//	VkImageMemoryBarrier image_memory_barrier{};
		//	image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		//	image_memory_barrier.oldLayout = old_image_layout;
		//	image_memory_barrier.newLayout = new_image_layout;
		//	image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; //TODO: should we use queue->GetFamilyIndex() here??
		//	image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; //TODO: should we use queue->GetFamilyIndex() here??
		//	image_memory_barrier.image = image;
		//	image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		//	image_memory_barrier.subresourceRange.baseMipLevel = 0;
		//	image_memory_barrier.subresourceRange.levelCount = 1;
		//	image_memory_barrier.subresourceRange.baseArrayLayer = 0;
		//	image_memory_barrier.subresourceRange.layerCount = 1;

		//	VulkanStage src_stage = VulkanStage::None;
		//	VulkanStage dst_stage = VulkanStage::None;

		//	if (old_image_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		//	{
		//		image_memory_barrier.srcAccessMask = VK_ACCESS_NONE;
		//		image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		//		src_stage = VulkanStage::Top;
		//		dst_stage = VulkanStage::Transfer;
		//	}
		//	else if (old_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
		//			 new_image_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		//	{
		//		image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		//		image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		//		src_stage = VulkanStage::Transfer;
		//		dst_stage = VulkanStage::FragmentInput;
		//	}
		//	else if (old_image_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
		//			 new_image_layout == VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL)
		//	{
		//		image_memory_barrier.srcAccessMask = VK_ACCESS_NONE;
		//		image_memory_barrier.dstAccessMask =
		//			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		//		src_stage = VulkanStage::Top;
		//		dst_stage = VulkanStage::FragmentInput;
		//	}

		//	if (new_image_layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
		//	{
		//		image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		//		if (VulkanHasStencilComponent(image.GetFormat()))
		//			image_memory_barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		//	}
		//	else
		//	{
		//		image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		//	}

		//	VulkanPipelineBarrierCommand pipeline_barrier{ dst_stage, src_stage, 0, {}, {}, { image_memory_barrier } };

		//	mem::sptr<VulkanCommandBuffer> buffer = pool->AllocateCommandBuffer();
		//	buffer->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		//	buffer->Add(pipeline_barrier);
		//	buffer->End();
		//	buffers.emplace_back(buffer);
		//	return queue->Submit({ buffer }, submit_info, fence);
		//}

		//VkResult SyncTransition(VulkanImage& image, VkImageLayout old_image_layout,
		//						VkImageLayout new_image_layout, VkSubmitInfo& submit_info, mem::sptr<VulkanQueue> queue, mem::sptr<VulkanCommandPool> pool)
		//{
		//	con::vector<mem::sptr<VulkanCommandBuffer>> buffers;
		//	VulkanFence fence(GetLogicalDevice());
		//	fence.Reset();
		//	VkResult result = AsyncTransition(image, old_image_layout, new_image_layout, submit_info, queue, pool, buffers, fence);
		//	fence.Wait();
		//	pool->DeallocateCommandBuffers(buffers);
		//	return result;
		//}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_VULKAN_RENDER_DEVICE_HPP */