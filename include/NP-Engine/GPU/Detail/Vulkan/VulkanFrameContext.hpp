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

#include "NP-Engine/GPU/Interface/FrameContext.hpp"

#include "VulkanDevice.hpp"
#include "VulkanFence.hpp"
#include "VulkanImageResource.hpp"
#include "VulkanImageResourceView.hpp"
#include "VulkanSemaphore.hpp"
#include "VulkanResult.hpp"

namespace np::gpu::__detail
{
	struct VulkanFrame : public Frame
	{
		mem::sptr<srvc::Services> services;
		siz index;
		mem::sptr<VulkanImageResource> image;
		mem::sptr<VulkanImageResourceView> view;

		VulkanFrame(mem::sptr<srvc::Services> services_, siz index_, mem::sptr<VulkanImageResource> image_, mem::sptr<VulkanImageResourceView> view_) :
			services(services_),
			index(index_),
			image(image_),
			view(view_)
		{}

		VulkanFrame() : VulkanFrame(nullptr, 0, nullptr, nullptr) {}

		virtual ~VulkanFrame() = default;

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

		static VkSwapchainCreateInfoKHR CreateVkInfo(mem::sptr<VulkanDevice> device, VkExtent2D extent, VkSwapchainKHR old_swapchain)
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
			info.oldSwapchain = old_swapchain;
			return info;
		}

		static VkExtent2D SanitizeVkExtent2D(mem::sptr<VulkanPresentTarget> target, const VkSurfaceCapabilitiesKHR& capabilities)
		{
			VkExtent2D extent{};

			if (capabilities.currentExtent.width != UI32_MAX)
			{
				extent = capabilities.currentExtent;
			}
			else
			{
				const VkExtent2D framebuffer_extent = target->GetFramebufferExtent();
				const VkExtent2D min_extent = capabilities.minImageExtent;
				const VkExtent2D max_extent = capabilities.maxImageExtent;

				extent = {::std::clamp(framebuffer_extent.width, min_extent.width, max_extent.width),
						  ::std::clamp(framebuffer_extent.height, min_extent.height, max_extent.height)};
			}

			return extent;
		}

		static VkSwapchainKHR CreateVkSwapchain(mem::sptr<VulkanDevice> device,
												const con::vector<DeviceQueueFamily>& queue_families, VkExtent2D& extent, VkSwapchainKHR old_swapchain)
		{
			const VulkanPhysicalDevice physical_device = device->GetLogicalDevice()->GetPhysicalDevice();
			mem::sptr<VulkanPresentTarget> target = device->GetPresentTarget();
			const VkSurfaceCapabilitiesKHR capabilities = physical_device.GetVkSurfaceCapabilities(target);
			//ensure we set extent between the latest GetVkSurfaceCapabilities call and vkCreateSwapchainKHR
			extent = SanitizeVkExtent2D(target, capabilities);

			con::vector<ui32> family_indices(queue_families.size());
			for (siz i = 0; i < family_indices.size(); i++)
				family_indices[i] = queue_families[i].index;

			VkSwapchainCreateInfoKHR info = CreateVkInfo(device, extent, old_swapchain);
			info.imageSharingMode = family_indices.size() > 1 ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
			info.queueFamilyIndexCount = (ui32)family_indices.size();
			info.pQueueFamilyIndices = family_indices.empty() ? nullptr : family_indices.data();

			if ((capabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) != 0)
				info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			//TODO: else do we want an error here?

			if ((capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) != 0)
				info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			//TODO: else do we want an error here?

			info.preTransform = capabilities.currentTransform; // says that we don't want any local transform //TODO:
															   // investigate this. We may want to mess with this

			const ui32 min_image_count = capabilities.minImageCount;
			ui32 image_count = ::std::max(
				min_image_count + 1, (ui32)3); //TODO: should we pull the desired frame count (of 3) from target or something?
			const ui32 max_image_count = capabilities.maxImageCount != 0 ? capabilities.maxImageCount : image_count;
			info.minImageCount = ::std::min(image_count, max_image_count);

			mem::sptr<VulkanInstance> instance = device->GetDetailInstance();
			VkSwapchainKHR swapchain = nullptr;
			VkResult result = vkCreateSwapchainKHR(*device->GetLogicalDevice(), &info, instance->GetVulkanAllocationCallbacks(), &swapchain);
			return result == VK_SUCCESS ? swapchain : nullptr;
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

		static con::vector<mem::sptr<VulkanImageResource>> ToImageResources(mem::sptr<VulkanDevice> device, const con::vector<VkImage>& vk_images, VkExtent2D extent)
		{
			const VulkanImageResourceUsage usage = VulkanImageResourceUsage::Color;
			const VulkanFormat format = device->GetVkSurfaceFormat().format;

			con::vector<mem::sptr<VulkanImageResource>> images(vk_images.size());
			for (siz i = 0; i < images.size(); i++)
				images[i] = VulkanImageResource::Create(device, usage, vk_images[i], format, 1, 1, 0, extent.width,
					extent.height, 1, {});

			return images;
		}

		static con::vector<mem::sptr<VulkanFrame>> CreateFrames(mem::sptr<VulkanDevice> device, VkSwapchainKHR swapchain,
																VkExtent2D extent)
		{
			const VulkanImageResourceUsage usage = VulkanImageResourceUsage::Color;
			const VulkanFormat format = device->GetVkSurfaceFormat().format;
			const con::vector<VkImage> vk_images = RetrieveVkImages(device, swapchain);
			con::vector<mem::sptr<VulkanImageResource>> images = ToImageResources(device, vk_images, extent);

			mem::sptr<srvc::Services> services = device->GetServices();
			con::vector<mem::sptr<VulkanFrame>> frames;
			for (siz i = 0; i < images.size(); i++)
				frames.emplace_back(mem::create_sptr<VulkanFrame>(services->GetAllocator(),
					services, i, images[i], ImageResourceView::Create(device, images[i], usage)));

			return frames;
		}

		void DestroySwapchain()
		{
			if (_device && _swapchain)
			{
				mem::sptr<VulkanInstance> instance = _device->GetDetailInstance();
				vkDestroySwapchainKHR(*_device->GetLogicalDevice(), _swapchain, instance->GetVulkanAllocationCallbacks());
				_swapchain = nullptr;
			}
		}

		void RebuildFrames()
		{
			const VulkanImageResourceUsage usage = VulkanImageResourceUsage::Color;
			const VulkanFormat format = _device->GetVkSurfaceFormat().format;
			const con::vector<VkImage> vk_images = RetrieveVkImages(_device, _swapchain);
			con::vector<mem::sptr<VulkanImageResource>> images = ToImageResources(_device, vk_images, _extent);

			mem::sptr<srvc::Services> services = _device->GetServices();

			while (images.size() < _frames.size())
				_frames.pop_back();

			for (siz i = 0; i < images.size(); i++)
			{
				if (i < _frames.size())
				{
					_frames[i]->image = images[i];
					_frames[i]->view = ImageResourceView::Create(_device, _frames[i]->image, usage);
				}
				else
				{
					_frames.emplace_back(mem::create_sptr<VulkanFrame>(services->GetAllocator(),
						services, i, images[i], ImageResourceView::Create(_device, images[i], usage)));
				}
			}
		}

	public:
		//TODO: CONSIDER PREVIOUS SWAPCHAIN WHEN RECREATING THE NEW ONE - LIKE WHEN RESIZING THE WINDOW, ETC

		VulkanFrameContext(mem::sptr<Device> device, const con::vector<DeviceQueueFamily>& queue_families):
			_device(DetailObject::EnsureIsDetailType(device, DetailType::Vulkan)),
			_queue_families{queue_families.begin(), queue_families.end()},
			_extent{},
			_swapchain(CreateVkSwapchain(_device, _queue_families, _extent, nullptr)),
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
			return VulkanFormat{_device->GetVkSurfaceFormat().format};
		}

		virtual con::vector<mem::sptr<Frame>> GetFrames() const override
		{
			return {_frames.begin(), _frames.end()};
		}

		/*
			nanoseconds (use zero to conduct non-blocking behavior)
		*/
		virtual void SetAcquireFrameTimeout(siz timeout) override
		{
			_acquire_frame_timeout = timeout;
		}

		/*
			semaphore and fence will become signaled when acquired image is ready to be rendered to
		*/
		virtual Result TryAcquireFrame(mem::sptr<Semaphore> semaphore_, mem::sptr<Fence> fence_) override
		{
			mem::sptr<VulkanLogicalDevice> logical_device = _device->GetLogicalDevice();
			mem::sptr<VulkanSemaphore> semaphore = DetailObject::EnsureIsDetailType(semaphore_, DetailType::Vulkan);
			mem::sptr<VulkanFence> fence = DetailObject::EnsureIsDetailType(fence_, DetailType::Vulkan);
			ui32 index = UI32_MAX;

			//vkAcquireNextImageKHR is a blocking call
			VulkanResult result = vkAcquireNextImageKHR(*logical_device, _swapchain, _acquire_frame_timeout,
				semaphore ? *semaphore : (VkSemaphore)nullptr, fence ? *fence : (VkFence)nullptr, &index);

			if (result.Contains(Result::Success))
			{
				_prev_acquired_frame_index = _acquired_frame_index;
				_acquired_frame_index = index;
			}

			return result;
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

		virtual void Rebuild() override
		{
			VkSwapchainKHR next_swapchain = CreateVkSwapchain(_device, _queue_families, _extent, _swapchain);
			DestroySwapchain();
			_swapchain = next_swapchain;
			RebuildFrames();
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_FRAME_CONTEXT_HPP */