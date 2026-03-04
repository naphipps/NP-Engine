//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/26/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_FRAME_CONTEXT_HPP
#define NP_ENGINE_GPU_VULKAN_FRAME_CONTEXT_HPP

#include "NP-Engine/Services/Services.hpp"
#include "NP-Engine/Math/Math.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanDevice.hpp"
#include "VulkanFence.hpp"
#include "VulkanImageResource.hpp"
#include "VulkanImageResourceView.hpp"
#include "VulkanSemaphore.hpp"

namespace np::gpu::__detail
{
	struct VulkanFrame : public Frame
	{
		mem::sptr<srvc::Services> services = nullptr;
		siz index = 0;
		mem::sptr<VulkanImageResource> image = nullptr;
		mem::sptr<VulkanImageResourceView> view = nullptr;

		mem::sptr<VulkanSemaphore> readySemaphore = nullptr;
		mem::sptr<VulkanFence> readyFence = nullptr;

		mem::sptr<VulkanSemaphore> completedSemaphore = nullptr;

		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual mem::sptr<srvc::Services> GetServices() const override
		{
			return services;
		}

		virtual mem::sptr<ImageResourceView> GetImageResourceView() const override
		{
			return view;
		}

		virtual mem::sptr<Semaphore> GetReadySemaphore() const override
		{
			return readySemaphore;
		}

		virtual mem::sptr<Fence> GetReadyFence() const override
		{
			return readyFence;
		}

		virtual mem::sptr<Semaphore> GetCompletedSemaphore() const override
		{
			return completedSemaphore;
		}
	};

	class VulkanFrameContext : public FrameContext
	{
	private:
		mem::sptr<VulkanDevice> _device;
		con::vector<DeviceQueueFamily> _queue_families;
		VkExtent2D _extent;
		VkSwapchainKHR _swapchain;
		con::vector<mem::sptr<VulkanFrame>> _frames;
		siz _acquire_frame_timeout;
		ui32 _acquired_frame_index;
		ui32 _prev_acquired_frame_index;

		static VkSwapchainCreateInfoKHR CreateVkInfo(mem::sptr<VulkanDevice> device, VkExtent2D extent)
		{
			const VkSurfaceFormatKHR surface_format = device->GetVkSurfaceFormat();
			mem::sptr<VulkanPresentTarget> target = device->GetPresentTarget();
			VkSwapchainCreateInfoKHR info{};
			info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			info.surface = *target;
			info.imageFormat = surface_format.format;
			info.imageColorSpace = surface_format.colorSpace;
			info.imageArrayLayers = 1;
			
			info.presentMode = device->GetVkPresentMode();
			
			info.clipped = VK_TRUE;
			info.imageExtent = extent;
			return info;
		}

		static VkSwapchainKHR CreateVkSwapchain(mem::sptr<VulkanDevice> device, const con::vector<DeviceQueueFamily>& queue_families, VkExtent2D extent)
		{
			const VulkanPhysicalDevice physical_device = device->GetLogicalDevice()->GetPhysicalDevice();
			mem::sptr<VulkanPresentTarget> target = device->GetPresentTarget();
			const VkSurfaceCapabilitiesKHR capabilities = physical_device.GetVkSurfaceCapabilities(target);
			
			con::vector<ui32> family_indices(queue_families.size());
			for (siz i = 0; i < family_indices.size(); i++)
				family_indices[i] = queue_families[i].index;

			VkSwapchainCreateInfoKHR info = CreateVkInfo(device, extent);
			info.imageSharingMode = family_indices.size() > 1 ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
			info.queueFamilyIndexCount = (ui32)family_indices.size();
			info.pQueueFamilyIndices = family_indices.empty() ? nullptr : family_indices.data();


			if ((capabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) != 0)
				info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			//TODO: else do we want an error here?

			if ((capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) != 0)
				info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			//TODO: else do we want an error here?

			info.preTransform = capabilities.currentTransform; // says that we don't want any local transform //TODO: investigate this. We may want to mess with this

			const ui32 min_image_count = capabilities.minImageCount;
			ui32 image_count = ::std::max(min_image_count + 1, (ui32)3); //TODO: should we pull the desired frame count (of 3) from target or something?
			const ui32 max_image_count = capabilities.maxImageCount != 0 ? capabilities.maxImageCount : image_count;
			info.minImageCount = ::std::min(image_count, max_image_count);

			VkSwapchainKHR swapchain = nullptr;
			if (vkCreateSwapchainKHR(*device->GetLogicalDevice(), &info, nullptr, &swapchain) != VK_SUCCESS)
				swapchain = nullptr;
			return swapchain;
		}

		static con::vector<VkImage> RetrieveVkImages(mem::sptr<VulkanDevice> device, VkSwapchainKHR swapchain)
		{
			con::vector<VkImage> images{};

			if (device && swapchain)
			{
				ui32 count;
				vkGetSwapchainImagesKHR(*device->GetLogicalDevice(), swapchain, &count, nullptr);
				images.resize(count);
				vkGetSwapchainImagesKHR(*device->GetLogicalDevice(), swapchain, &count, images.data());
			}

			return images;
		}

		static con::vector<mem::sptr<VulkanFrame>> CreateFrames(mem::sptr<VulkanDevice> device, VkSwapchainKHR swapchain, VkExtent2D extent)
		{
			const VulkanImageResourceUsage usage = VulkanImageResourceUsage::Color;
			const VulkanFormat format = device->GetVkSurfaceFormat().format;
			const con::vector<VkImage> vk_images = RetrieveVkImages(device, swapchain);

			con::vector<mem::sptr<VulkanImageResource>> images(vk_images.size());
			for (siz i = 0; i < images.size(); i++)
				images[i] = VulkanImageResource::Create(device, usage, vk_images[i], format, 1, 1, 0, extent.width, extent.height, 1,{});

			mem::sptr<srvc::Services> services = device->GetServices();
			con::vector<mem::sptr<VulkanFrame>> frames;
			for (siz i = 0; i < images.size(); i++)
			{
				frames.emplace_back(mem::create_sptr<VulkanFrame>(services->GetAllocator()));
				frames.back()->services = services;
				frames.back()->index = i;
				frames.back()->image = images[i];
				frames.back()->view = ImageResourceView::Create(device, images[i], usage);
			}

			return frames;
		}

		void DestroySwapchain()
		{
			if (_device && _swapchain)
			{
				vkDestroySwapchainKHR(*_device->GetLogicalDevice(), _swapchain, nullptr);
				_swapchain = nullptr;
			}
		}

		void RebuildFrames()
		{
			_frames = CreateFrames(_device, _swapchain, _extent);

			/*VkImageViewCreateInfo view_info = VulkanImageResourceView::CreateInfo();
			view_info.format = _device->GetVkSurfaceFormat().format;
			con::vector<VkImage> images = RetrieveVkImages(_device, _swapchain);

			while (images.size() < _frames.size())
				_frames.pop_back();

			for (siz i = 0; i < images.size(); i++)
			{
				view_info.image = images[i];
				mem::sptr<VulkanImage> image = mem::create_sptr<VulkanImage>(GetServices()->GetAllocator(), images[i], view_info.format);
				mem::sptr<VulkanImageView> view = mem::create_sptr<VulkanImageView>(GetServices()->GetAllocator(), device->GetLogicalDevice(), view_info);

				if (i < _frames.size())
				{
					_frames[i].image = image;
					_frames[i].imageView = view;
					_frames[i].prevFrameFence = nullptr;
				}
				else
				{
					_frames.emplace_back(i, device, image, view);
				}
			}*/
		}

	public:
		//TODO: CONSIDER PREVIOUS SWAPCHAIN WHEN RECREATING THE NEW ONE - LIKE WHEN RESIZING THE WINDOW, ETC

		VulkanFrameContext(mem::sptr<Device> device, const con::vector<DeviceQueueFamily>& queue_families) :
			_device(DetailObject::EnsureIsDetailType(device, DetailType::Vulkan)),
			_queue_families{ queue_families.begin(), queue_families.end() },
			_extent(_device->ChooseVkExtent2D()),
			_swapchain(CreateVkSwapchain(_device, _queue_families, _extent)),
			_frames(CreateFrames(_device, _swapchain, _extent)),
			_acquire_frame_timeout(SIZ_MAX),
			_acquired_frame_index(UI32_MAX),
			_prev_acquired_frame_index(UI32_MAX)
		{}

		virtual ~VulkanFrameContext()
		{
			mem::sptr<VulkanDevice> device = GetDevice();
			vkDeviceWaitIdle(*device->GetLogicalDevice());
			_frames.clear();
			DestroySwapchain();
		}

		operator VkSwapchainKHR() const
		{
			return _swapchain;
		}

		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual mem::sptr<srvc::Services> GetServices() const override
		{
			return _device->GetServices();
		}

		virtual mem::sptr<Device> GetDevice() const override
		{
			return _device;
		}

		virtual con::vector<DeviceQueueFamily> GetDeviceQueueFamilies() const override
		{
			return _queue_families;
		}

		virtual siz GetFrameWidth() const override
		{
			return _extent.width;
		}

		virtual siz GetFrameHeight() const override
		{
			return _extent.height;
		}

		virtual Format GetFrameFormat() const override
		{
			return VulkanFormat{ _device->GetVkSurfaceFormat().format };
		}

		virtual con::vector<mem::sptr<Frame>> GetFrames() const override
		{
			return { _frames.begin(), _frames.end() };
		}

		virtual void SetAcquireFrameTimeout(siz timeout) override
		{
			_acquire_frame_timeout = timeout;
		}

		virtual bl TryAcquireFrame() override
		{
			bl successful_enough = false;
			VkResult result = TryVkAcquireFrame();

			switch (result)
			{
			case VK_ERROR_OUT_OF_DATE_KHR:
			case VK_SUBOPTIMAL_KHR:
			case VK_SUCCESS:
			case VK_NOT_READY:
				successful_enough = true;
			}

			return successful_enough;
		}

		virtual mem::sptr<Frame> GetPrevAcquiredFrame() const override
		{
			return _prev_acquired_frame_index < _frames.size() ? _frames[_prev_acquired_frame_index] : nullptr;
		}

		virtual mem::sptr<Frame> GetAcquiredFrame() const override
		{
			return _acquired_frame_index < _frames.size() ? _frames[_acquired_frame_index] : nullptr;
		}

		virtual siz GetPrevAcquiredFrameIndex() const override
		{
			return _prev_acquired_frame_index;
		}

		virtual siz GetAcquiredFrameIndex() const override
		{
			return _acquired_frame_index;
		}







		VkExtent2D GetExtent() const
		{
			return _extent;
		}

		flt GetAspectRatio() const
		{
			return (flt)_extent.width / (flt)_extent.height;
		}






		//TODO: we need to revisit the whole rebuilding stuff
		void Rebuild() //TODO: should we support passing in the old swapchain for reference when creating the new swapchain?
		{
			DestroySwapchain();
			_extent = _device->ChooseVkExtent2D();
			_swapchain = CreateVkSwapchain(_device, _queue_families, _extent);
			RebuildFrames();
		}








		

		VkResult TryVkAcquireFrame()
		{
			mem::sptr<VulkanDevice> device = _device;
			mem::sptr<VulkanLogicalDevice> logical_device = device->GetLogicalDevice();

			/*
				TODO: I feel like we could have a ring buffer for these ready-semaphores/fences?
			*/

			//ui64 timeout = SIZ_MAX; //nanoseconds (use zero to conduction non-blocking behavior)
			// semaphore and fence will become signaled when acquired image is ready to be rendered to
			mem::sptr<VulkanSemaphore> ready_semaphore = mem::create_sptr<VulkanSemaphore>(GetServices()->GetAllocator(), logical_device);
			mem::sptr<VulkanFence> ready_fence = mem::create_sptr<VulkanFence>(GetServices()->GetAllocator(), logical_device);
			ui32 index = UI32_MAX; //acquired_image_index

			//vkAcquireNextImageKHR is a blocking call
			VkResult result = vkAcquireNextImageKHR(*logical_device, _swapchain, _acquire_frame_timeout, *ready_semaphore, *ready_fence, &index);

			switch (result)
			{
			//result VK_SUCCESS / VK_SUBOPTIMAL_KHR / VK_ERROR_OUT_OF_DATE_KHR indicate image acquired, and maybe a framebuffer resize/rebuild indication
			case VK_ERROR_OUT_OF_DATE_KHR:
			case VK_SUBOPTIMAL_KHR:
				//do we need to consider framebuffer resize/rebuild here? switch case may not be beneficial for this since important things are handled below
				//no break intentional

			case VK_SUCCESS:
				_prev_acquired_frame_index = _acquired_frame_index;
				_acquired_frame_index = index;
				_frames[_acquired_frame_index]->readySemaphore = ready_semaphore;
				_frames[_acquired_frame_index]->readyFence = ready_fence;
				_frames[_acquired_frame_index]->completedSemaphore = mem::create_sptr<VulkanSemaphore>(GetServices()->GetAllocator(), logical_device);
				break;
				
			//result: VK_NOT_READY indicates no image is ready yet, timeout probably tripped
			case VK_NOT_READY:
				break;

			default:
				NP_ENGINE_ASSERT(false, "other error went wrong");
				break;
			}

			return result;
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_FRAME_CONTEXT_HPP */