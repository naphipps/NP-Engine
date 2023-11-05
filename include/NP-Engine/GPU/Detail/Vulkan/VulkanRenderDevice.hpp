//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/5/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_RENDER_DEVICE_HPP
#define NP_ENGINE_VULKAN_RENDER_DEVICE_HPP

#include "NP-Engine/Services/Services.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanRenderTarget.hpp"
#include "VulkanLogicalDevice.hpp"
#include "VulkanCommandPool.hpp"
#include "VulkanQueue.hpp"
#include "VulkanImage.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanFence.hpp"

namespace np::gpu::__detail
{
	class VulkanRenderDevice : public RenderDevice
	{
	private:
		VulkanPhysicalDevice _physical_device;
		ui32 _graphics_family_index;
		ui32 _present_family_index;
		VkSurfaceFormatKHR _surface_format;
		VkPresentModeKHR _present_mode;
		mem::sptr<VulkanLogicalDevice> _device;
		mem::sptr<VulkanCommandPool> _command_pool;
		mem::sptr<VulkanQueue> _graphics_queue;
		mem::sptr<VulkanQueue> _present_queue;

		static siz GetPhysicalDeviceScore(VulkanPhysicalDevice physical_device, mem::sptr<RenderTarget> target)
		{
			siz score = 0;

			if (physical_device && target)
			{
				VulkanRenderTarget& render_target = (VulkanRenderTarget&)(*target);

				VkPhysicalDeviceProperties properties{};
				VkPhysicalDeviceVulkan12Properties properties12{};
				VkPhysicalDeviceProperties2 properties2{};
				properties12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;
				properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
				properties2.pNext = &properties12;
				vkGetPhysicalDeviceProperties(physical_device, &properties);
				vkGetPhysicalDeviceProperties2(physical_device, &properties2);

				VkPhysicalDeviceFeatures features{};
				VkPhysicalDeviceVulkan12Features features12{};
				VkPhysicalDeviceFeatures2 features2{};
				features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
				features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
				features2.pNext = &features12;
				vkGetPhysicalDeviceFeatures(physical_device, &features);
				vkGetPhysicalDeviceFeatures2(physical_device, &features2);

				switch (properties.deviceType)
				{
				case VK_PHYSICAL_DEVICE_TYPE_CPU:
				case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
					score += 10;
					break;
				case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
					score += 1000;
					break;
				case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
					score += 100;
					break;
				default:
					break;
				}

				if (features.samplerAnisotropy == VK_TRUE)
					score += 200;

				bl has_queue_graphics_bit = false; // we require the VK_QUEUE_GRAPHICS_BIT
				bl has_queue_present_bit = false; // we require KHR present support
				bl supports_required_extensions = false;
				bl supports_required_layers = false;

				// check queue families
				{
					con::vector<VkQueueFamilyProperties> queue_family_properties = physical_device.GetQueueFamilyProperties();

					for (siz i = 0; i < queue_family_properties.size(); i++)
					{
						if (!has_queue_graphics_bit && (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
						{
							has_queue_graphics_bit = true;
						}

						if (!has_queue_present_bit)
						{
							VkBool32 supported = false;
							vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, render_target, &supported);
							has_queue_present_bit = (bl)supported;
						}
					}
				}

				// check extensions
				{
					con::vector<str> required_extensions = VulkanLogicalDevice::GetRequiredDeviceExtensionNames();
					if (required_extensions.size() > 0)
					{
						con::uset<str> required_extensions_set(required_extensions.begin(), required_extensions.end());
						con::vector<str> supported_extensions = physical_device.GetSupportedDeviceExtensionNames();
						for (const str& e : supported_extensions)
							required_extensions_set.erase(e);
						supports_required_extensions = required_extensions_set.empty();
					}
					else
					{
						supports_required_extensions = true;
					}
				}

				// check layers
				{
					con::vector<str> required_layers = VulkanLogicalDevice::GetRequiredDeviceLayerNames();
					if (required_layers.size() > 0)
					{
						con::uset<str> required_layers_set(required_layers.begin(), required_layers.end());
						con::vector<str> supported_layers = physical_device.GetSupportedDeviceLayerNames();
						for (const str& l : supported_layers)
							required_layers_set.erase(l);
						supports_required_layers = required_layers_set.empty();
					}
					else
					{
						supports_required_layers = true;
					}
				}

				if (!has_queue_graphics_bit || !has_queue_present_bit || !supports_required_extensions ||
					!supports_required_layers)
				{
					score = 0;
				}
			}

			return score;
		}

		static VulkanPhysicalDevice ChoosePhysicalDevice(mem::sptr<RenderTarget> render_target)
		{
			VulkanPhysicalDevice physical_device;
			if (render_target)
			{
				mem::sptr<DetailInstance> instance = render_target->GetInstance();

				using CandidateType = ::std::pair<siz, VulkanPhysicalDevice>;
				con::vector<VulkanPhysicalDevice> physical_devices = VulkanPhysicalDevice::GetAllDevices(instance);
				con::vector<CandidateType> candidates;

				for (VulkanPhysicalDevice pd : physical_devices)
					candidates.emplace_back(GetPhysicalDeviceScore(pd, render_target), pd);

				::std::sort(candidates.begin(), candidates.end(),
							[](const CandidateType& a, const CandidateType& b)
							{
								return a.first > b.first; // descending sort
							});

				if (!candidates.empty() && candidates.front().first > 0)
					physical_device = candidates.front().second;
			}
			return physical_device;
		}

		static con::vector<VkDeviceQueueCreateInfo> CreateQueueCreateInfos(con::oset<ui32> families)
		{
			con::vector<VkDeviceQueueCreateInfo> infos;
			for (ui32 family : families)
			{
				VkDeviceQueueCreateInfo info{};
				info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				info.queueFamilyIndex = family;
				infos.emplace_back(info);
			}
			return infos;
		}

		static mem::sptr<VulkanLogicalDevice> CreateLogicalDevice(mem::sptr<srvc::Services> services,
																  VulkanPhysicalDevice physical_device,
																  con::vector<VkDeviceQueueCreateInfo> queue_infos)
		{
			return mem::create_sptr<VulkanLogicalDevice>(services->GetAllocator(), services, physical_device, queue_infos);
		}

		static VkCommandPoolCreateInfo CreateCommandPoolInfo(ui32 queue_family_index)
		{
			VkCommandPoolCreateInfo info = VulkanCommandPool::CreateInfo();
			info.queueFamilyIndex = queue_family_index;
			info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			return info;
		}

		static mem::sptr<VulkanCommandPool> CreateCommandPool(mem::sptr<srvc::Services> services,
															  mem::sptr<VulkanLogicalDevice> device,
															  VkCommandPoolCreateInfo info)
		{
			return mem::create_sptr<VulkanCommandPool>(services->GetAllocator(), device, info);
		}

		static mem::sptr<VulkanQueue> CreateQueue(mem::sptr<srvc::Services> services, mem::sptr<VulkanLogicalDevice> device,
												  ui32 queue_family_index, ui32 queue_index)
		{
			return mem::create_sptr<VulkanQueue>(services->GetAllocator(), device, queue_family_index, queue_index);
		}

	public:
		VulkanRenderDevice(mem::sptr<RenderTarget> target):
			RenderDevice(target),
			_physical_device(ChoosePhysicalDevice(target)),
			_graphics_family_index(_physical_device.GetGraphicsFamilyIndex()),
			_present_family_index(_physical_device.GetPresentFamilyIndex(target)),
			_surface_format(_physical_device.ChooseSurfaceFormat(target)),
			_present_mode(_physical_device.ChoosePresentMode(target)),
			_device(CreateLogicalDevice(GetServices(), _physical_device,
										CreateQueueCreateInfos({_graphics_family_index, _present_family_index}))),
			_command_pool(CreateCommandPool(GetServices(), _device, CreateCommandPoolInfo(_graphics_family_index))),
			_graphics_queue(CreateQueue(GetServices(), _device, _graphics_family_index, 0)),
			_present_queue(CreateQueue(GetServices(), _device, _present_family_index, 0))
		{}

		operator VkDevice() const
		{
			return *_device;
		}

		VkSurfaceFormatKHR GetSurfaceFormat() const
		{
			return _surface_format;
		}

		VkPresentModeKHR GetPresentMode() const
		{
			return _present_mode;
		}

		ui32 GetGraphicsFamilyIndex() const
		{
			return _graphics_family_index;
		}

		ui32 GetPresentFamilyIndex() const
		{
			return _present_family_index;
		}

		mem::sptr<VulkanQueue> GetGraphicsQueue() const
		{
			return _graphics_queue;
		}

		mem::sptr<VulkanQueue> GetPresentQueue() const
		{
			return _present_queue;
		}

		str GetPhysicalDeviceName() const
		{
			return _device->GetPhysicalDeviceName();
		}

		VulkanPhysicalDevice GetPhysicalDevice() const
		{
			return _physical_device;
		}

		mem::sptr<VulkanLogicalDevice> GetLogicalDevice() const
		{
			return _device;
		}

		mem::sptr<VulkanCommandPool> GetCommandPool() const
		{
			return _command_pool;
		}

		void BeginCommandBuffer(mem::sptr<VulkanCommandBuffer> command_buffer, VkCommandBufferBeginInfo& begin_info)
		{
			BeginCommandBuffers({ command_buffer }, begin_info);
		}

		void BeginCommandBuffers(const con::vector<mem::sptr<VulkanCommandBuffer>>& command_buffers,
			VkCommandBufferBeginInfo& begin_info)
		{
			for (const mem::sptr<VulkanCommandBuffer>& command_buffer : command_buffers)
				vkBeginCommandBuffer(*command_buffer, &begin_info);
		}

		void BeginCommandBuffers(const con::vector<mem::sptr<VulkanCommandBuffer>>& command_buffers,
			con::vector<VkCommandBufferBeginInfo>& begin_infos)
		{
			NP_ENGINE_ASSERT(command_buffers.size() == begin_infos.size(), "command_buffers size must equal begin_infos size");
			for (siz i = 0; i < command_buffers.size(); i++)
				vkBeginCommandBuffer(*command_buffers[i], &begin_infos[i]);
		}

		void EndCommandBuffer(mem::sptr<VulkanCommandBuffer> command_buffer)
		{
			EndCommandBuffers({ command_buffer });
		}

		void EndCommandBuffers(const con::vector<mem::sptr<VulkanCommandBuffer>>& command_buffers)
		{
			for (const mem::sptr<VulkanCommandBuffer>& command_buffer : command_buffers)
				vkEndCommandBuffer(*command_buffer);
		}

		VkResult AsyncCopy(VulkanBuffer& dst, const VulkanBuffer& src, 
			VkSubmitInfo& submit_info, mem::sptr<VulkanQueue> queue,
			con::vector<mem::sptr<VulkanCommandBuffer>>& command_buffers, VkFence fence = nullptr)
		{
			VkBufferCopy buffer_copy{};
			buffer_copy.size = src.GetSize();
			VulkanCommandCopyBuffers copy_buffers(dst, src, 1, &buffer_copy);

			mem::sptr<VulkanCommandBuffer> buffer = _command_pool->AllocateCommandBuffer();
			VkCommandBufferBeginInfo begin_info = VulkanCommandBuffer::CreateBeginInfo();
			begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			BeginCommandBuffer(buffer, begin_info);
			buffer->Add(copy_buffers);
			EndCommandBuffer(buffer);
			command_buffers.emplace_back(buffer);
			return queue->Submit(buffer, submit_info, fence);
		}

		VkResult SyncCopy(VulkanBuffer& dst, const VulkanBuffer& src, VkSubmitInfo& submit_info, 
			mem::sptr<VulkanQueue> queue)
		{
			con::vector<mem::sptr<VulkanCommandBuffer>> command_buffers;
			VulkanFence fence(GetLogicalDevice());
			fence.Reset();
			VkResult result = AsyncCopy(dst, src, submit_info, queue, command_buffers, fence);
			fence.Wait();
			_command_pool->DeallocateCommandBuffers(command_buffers);
			return result;
		}

		VkResult AsyncCopy(VulkanImage& dst, const VulkanBuffer& src, VkBufferImageCopy buffer_image_copy, 
			VkSubmitInfo& submit_info, mem::sptr<VulkanQueue> queue, 
			con::vector<mem::sptr<VulkanCommandBuffer>>& command_buffers, VkFence fence = nullptr)
		{
			VulkanCommandCopyBufferToImage copy_buffer_to_image(dst, src, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
				&buffer_image_copy);

			mem::sptr<VulkanCommandBuffer> buffer = _command_pool->AllocateCommandBuffer();
			VkCommandBufferBeginInfo begin_info = VulkanCommandBuffer::CreateBeginInfo();
			begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			BeginCommandBuffer(buffer, begin_info);
			buffer->Add(copy_buffer_to_image);
			EndCommandBuffer(buffer);
			command_buffers.emplace_back(buffer);
			return queue->Submit(buffer, submit_info, fence);
		}

		VkResult SyncCopy(VulkanImage& dst, const VulkanBuffer& src, VkBufferImageCopy buffer_image_copy, 
			VkSubmitInfo& submit_info, mem::sptr<VulkanQueue> queue)
		{
			con::vector<mem::sptr<VulkanCommandBuffer>> command_buffers;
			VulkanFence fence(GetLogicalDevice());
			fence.Reset();
			VkResult result = AsyncCopy(dst, src, buffer_image_copy, submit_info, queue, command_buffers, fence);
			fence.Wait();
			_command_pool->DeallocateCommandBuffers(command_buffers);
			return result;
		}

		VkResult AsyncTransition(VulkanImage& image, VkFormat format, VkImageLayout old_image_layout, 
			VkImageLayout new_image_layout, VkSubmitInfo& submit_info, mem::sptr<VulkanQueue> queue,
			con::vector<mem::sptr<VulkanCommandBuffer>>& command_buffers, VkFence fence = nullptr)
		{
			VkImageMemoryBarrier image_memory_barrier{};
			image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			image_memory_barrier.oldLayout = old_image_layout;
			image_memory_barrier.newLayout = new_image_layout;
			image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			image_memory_barrier.image = image;
			image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			image_memory_barrier.subresourceRange.baseMipLevel = 0;
			image_memory_barrier.subresourceRange.levelCount = 1;
			image_memory_barrier.subresourceRange.baseArrayLayer = 0;
			image_memory_barrier.subresourceRange.layerCount = 1;

			VkPipelineStageFlags src_pipeline_stage_flags = 0;
			VkPipelineStageFlags dst_pipeline_stage_flags = 0;

			if (old_image_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			{
				image_memory_barrier.srcAccessMask = 0;
				image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				src_pipeline_stage_flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				dst_pipeline_stage_flags = VK_PIPELINE_STAGE_TRANSFER_BIT;
			}
			else if (old_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
				new_image_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			{
				image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				src_pipeline_stage_flags = VK_PIPELINE_STAGE_TRANSFER_BIT;
				dst_pipeline_stage_flags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			}
			else if (old_image_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
				new_image_layout == VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL)
			{
				image_memory_barrier.srcAccessMask = 0;
				image_memory_barrier.dstAccessMask =
					VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				src_pipeline_stage_flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				dst_pipeline_stage_flags = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			}

			if (new_image_layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
			{
				image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

				if (VulkanHasStencilComponent(format))
					image_memory_barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
			else
			{
				image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			}

			VulkanCommandPipelineBarrier pipeline_barrier(dst_pipeline_stage_flags, src_pipeline_stage_flags, 0, 0, nullptr, 0,
				nullptr, 1, &image_memory_barrier);

			mem::sptr<VulkanCommandBuffer> buffer = _command_pool->AllocateCommandBuffer();
			VkCommandBufferBeginInfo begin_info = VulkanCommandBuffer::CreateBeginInfo();
			begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			BeginCommandBuffer(buffer, begin_info);
			buffer->Add(pipeline_barrier);
			EndCommandBuffer(buffer);
			command_buffers.emplace_back(buffer);
			return queue->Submit(buffer, submit_info, fence);
		}

		VkResult SyncTransition(VulkanImage& image, VkFormat format, VkImageLayout old_image_layout,
			VkImageLayout new_image_layout, VkSubmitInfo& submit_info, mem::sptr<VulkanQueue> queue)
		{
			con::vector<mem::sptr<VulkanCommandBuffer>> command_buffers;
			VulkanFence fence(GetLogicalDevice());
			fence.Reset();
			VkResult result =
				AsyncTransition(image, format, old_image_layout, new_image_layout, submit_info, queue, command_buffers, fence);
			fence.Wait();
			_command_pool->DeallocateCommandBuffers(command_buffers);
			return result;
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_VULKAN_RENDER_DEVICE_HPP */