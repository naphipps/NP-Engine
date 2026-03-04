//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 6/18/25
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_BUFFER_RESOURCE_VIEW_HPP
#define NP_ENGINE_GPU_VULKAN_BUFFER_RESOURCE_VIEW_HPP

#include <utility>

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanLogicalDevice.hpp"

namespace np::gpu::__detail
{
	class VulkanBufferResourceView // : public BufferResourceView
	{
	private:
		mem::sptr<VulkanLogicalDevice> _device;
		VkBufferView _buffer_view;

		static VkBufferView CreateBufferView(mem::sptr<VulkanLogicalDevice> device, VkBufferViewCreateInfo info)
		{
			VkBufferView view = nullptr;
			if (vkCreateBufferView(*device, &info, nullptr, &view) != VK_SUCCESS)
				view = nullptr;

			return view;
		}

		void Dispose()
		{
			if (_device && _buffer_view)
			{
				vkDestroyBufferView(*_device, _buffer_view, nullptr);
				_buffer_view = nullptr;
			}

			_device.reset();
		}

	public:
		static VkBufferViewCreateInfo CreateInfo()
		{
			VkBufferViewCreateInfo info{}; //TODO: ??? what should the default values be here??
			info.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
			info.format = VK_FORMAT_R8G8B8A8_SRGB;
			return info;
		}

		VulkanBufferResourceView(mem::sptr<VulkanLogicalDevice> device, const VkBufferViewCreateInfo& info) :
			_device(device),
			_buffer_view(CreateBufferView(_device, info))
		{}

		VulkanBufferResourceView(VulkanBufferResourceView&& other) noexcept :
			_device(::std::move(other._device)),
			_buffer_view(::std::move(other._buffer_view))
		{
			other._device.reset();
			other._buffer_view = nullptr;
		}

		~VulkanBufferResourceView()
		{
			Dispose();
		}

		VulkanBufferResourceView& operator=(VulkanBufferResourceView&& other) noexcept
		{
			Dispose();
			_device = ::std::move(other._device);
			_buffer_view = ::std::move(other._buffer_view);
			other._device.reset();
			other._buffer_view = nullptr;
			return *this;
		}

		operator VkBufferView() const
		{
			return _buffer_view;
		}

		mem::sptr<VulkanLogicalDevice> GetLogicalDevice() const
		{
			return _device;
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_BUFFER_RESOURCE_VIEW_HPP */