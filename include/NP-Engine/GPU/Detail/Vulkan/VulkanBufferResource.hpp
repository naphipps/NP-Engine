//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/20/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_BUFFER_RESOURCE_HPP
#define NP_ENGINE_GPU_VULKAN_BUFFER_RESOURCE_HPP

#include <utility>

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanDevice.hpp"
#include "VulkanResource.hpp"

namespace np::gpu::__detail
{
	class VulkanBufferResourceUsage : public BufferResourceUsage
	{
	public:
		VulkanBufferResourceUsage(ui32 value): BufferResourceUsage(value) {}

		VkMemoryPropertyFlags GetVkMemoryPropertyFlags() const
		{
			VkMemoryPropertyFlags flags = 0;

			if (Contains(HostAccessible))
				flags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
			if (Contains(DeviceLocal))
				flags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			if (Contains(AutoClearCache))
				flags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

			return flags;
		}

		VkBufferUsageFlags GetVkBufferUsageFlags() const
		{
			const bl contains_read = Contains(Read);
			const bl contains_write = Contains(Write);
			VkBufferUsageFlags flags = 0;

			if (Contains(Transfer))
			{
				if (contains_read || !contains_write)
					flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
				if (contains_write || !contains_read)
					flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			}

			if (Contains(Uniform | Texel))
				flags |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
			if (Contains(Storage | Texel))
				flags |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
			if (Contains(Uniform))
				flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			if (Contains(Storage))
				flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
			if (Contains(Index))
				flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
			if (Contains(Vertex))
				flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			if (Contains(Indirect))
				flags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;

			return flags;
		}
	};

	class VulkanBufferResource : public BufferResource
	{
	private:
		mem::sptr<VulkanDevice> _device;
		siz _size;
		con::vector<DeviceQueueFamily> _queue_families;
		VkBuffer _buffer;
		mem::sptr<VulkanDeviceMemoryAllocation> _memory_allocation;

		static VkBufferCreateInfo CreateVkInfo(siz size, VulkanBufferResourceUsage usage)
		{
			VkBufferCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			info.size = size;
			info.usage = usage.GetVkBufferUsageFlags();
			return info;
		}

		static VkBuffer CreateVkBuffer(mem::sptr<VulkanDevice> device, VulkanBufferResourceUsage usage, siz size,
									   const con::vector<DeviceQueueFamily>& queue_families)
		{
			con::vector<ui32> family_indices(queue_families.size());
			for (siz i = 0; i < family_indices.size(); i++)
				family_indices[i] = queue_families[i].index;

			VkBufferCreateInfo info = CreateVkInfo(size, usage);
			info.sharingMode = family_indices.size() > 1 ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
			info.queueFamilyIndexCount = family_indices.size();
			info.pQueueFamilyIndices = family_indices.empty() ? nullptr : family_indices.data();

			mem::sptr<VulkanInstance> instance = device->GetDetailInstance();
			VkBuffer buffer = nullptr;
			VkResult result = vkCreateBuffer(*device->GetLogicalDevice(), &info, instance->GetVulkanAllocationCallbacks(), &buffer);
			return result == VK_SUCCESS ? buffer : nullptr;
		}

		static VkMemoryRequirements GetVkMemoryRequirements(mem::sptr<VulkanDevice> device, VkBuffer buffer)
		{
			VkMemoryRequirements requirements{};
			vkGetBufferMemoryRequirements(*device->GetLogicalDevice(), buffer, &requirements);
			requirements.alignment = mem::sanitize_alignment(requirements.alignment);
			requirements.size = mem::calc_aligned_size(requirements.size, requirements.alignment);
			return requirements;
		}

		void BindMemory()
		{
			VulkanResult result =  _memory_allocation->GetDeviceMemory()->Bind(_buffer, _memory_allocation->GetRegion().offset);
			//TODO: do we want to do anything with this result? at least assert success? do we need to succeed?
			NP_ENGINE_ASSERT(result.Contains(VulkanResult::Success), "Binding must succeed here.");
		}

	public:
		VulkanBufferResource(mem::sptr<Device> device, BufferResourceUsage usage, siz size,
							 const con::vector<DeviceQueueFamily>& queue_families):
			_device(device),
			_size(size),
			_queue_families{queue_families.begin(), queue_families.end()},
			_buffer(CreateVkBuffer(_device, VulkanBufferResourceUsage{usage}, _size, _queue_families)),
			_memory_allocation(_device->AllocateDeviceMemory(GetVkMemoryRequirements(_device, _buffer), VulkanBufferResourceUsage{ usage }.GetVkMemoryPropertyFlags()))
		{
			//TODO: check if _memory_allocation is valid?
			BindMemory();
		}

		~VulkanBufferResource()
		{
			if (_buffer)
			{
				mem::sptr<VulkanInstance> instance = _device->GetDetailInstance();
				vkDestroyBuffer(*_device->GetLogicalDevice(), _buffer, instance->GetVulkanAllocationCallbacks());
				_buffer = nullptr;
			}
		}

		operator VkBuffer() const
		{
			return _buffer;
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

		virtual siz GetSize() const override
		{
			return _size;
		}

		virtual con::vector<DeviceQueueFamily> GetDeviceQueueFamilies() const override
		{
			return _queue_families;
		}

		virtual bl Resize(siz size) override
		{
			return false;
		}

		virtual bl SetBytes(siz offset, const con::vector<ui8>& bytes) override
		{
			VulkanDeviceMemoryRegion region = _memory_allocation->GetRegion();
			region = { region.offset + offset, bytes.size() };
			return _memory_allocation->GetDeviceMemory()->SetBytes(region, bytes);
		}

		virtual con::vector<ui8> GetBytes(siz offset, siz size) override
		{
			VulkanDeviceMemoryRegion region = _memory_allocation->GetRegion();
			region = { region.offset + offset, size };
			return _memory_allocation->GetDeviceMemory()->GetBytes(region);
		}

		virtual bl ClearCacheForDevice(siz offset, siz size) override
		{
			VulkanDeviceMemoryRegion region = _memory_allocation->GetRegion();
			region = { region.offset + offset, size };
			return _memory_allocation->GetDeviceMemory()->ClearCacheForDevice(region);
		}

		virtual bl ClearCacheForHost(siz offset, siz size) override
		{
			VulkanDeviceMemoryRegion region = _memory_allocation->GetRegion();
			region = { region.offset + offset, size };
			return _memory_allocation->GetDeviceMemory()->ClearCacheForHost(region);
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_BUFFER_RESOURCE_HPP */