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
#include "VulkanDeviceMemory.hpp"
#include "VulkanResource.hpp"

namespace np::gpu::__detail
{
	class VulkanBufferResourceUsage : public BufferResourceUsage
	{
	public:
		VulkanBufferResourceUsage(ui32 value): BufferResourceUsage(value) {}

		VkMemoryPropertyFlags GetVkMemoryPropertyFlags() const
		{
			return Contains(HostAccessible) ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
											: VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
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
		VkMemoryPropertyFlags _memory_property_flags;
		mem::sptr<VulkanDeviceMemory> _memory; //TODO: not a good idea to allocate this per buffer, use a
											   //device-memory-allocator (guide pg85 mentions this)

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
			info.sharingMode = family_indices.empty() ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
			info.queueFamilyIndexCount = family_indices.size();
			info.pQueueFamilyIndices = family_indices.empty() ? nullptr : family_indices.data();

			VkBuffer buffer = nullptr;
			VkResult result = vkCreateBuffer(*device->GetLogicalDevice(), &info, nullptr, &buffer);
			return result == VK_SUCCESS ? buffer : nullptr;
		}

		static VkMemoryRequirements GetVkMemoryRequirements(mem::sptr<VulkanDevice> device, VkBuffer buffer)
		{
			VkMemoryRequirements requirements{};
			vkGetBufferMemoryRequirements(*device->GetLogicalDevice(), buffer, &requirements);
			return requirements;
		}

		void BindMemory()
		{
			vkBindBufferMemory(*_device->GetLogicalDevice(), _buffer, *_memory, 0);
		}

	public:
		VulkanBufferResource(mem::sptr<Device> device, BufferResourceUsage usage, siz size,
							 const con::vector<DeviceQueueFamily>& queue_families):
			_device(device),
			_size(size),
			_queue_families{queue_families.begin(), queue_families.end()},
			_buffer(CreateVkBuffer(_device, _size, usage, _queue_families)),
			_memory_property_flags(VulkanBufferResourceUsage{usage}.GetVkMemoryPropertyFlags()),
			_memory(_device->CreateDeviceMemory(GetVkMemoryRequirements(_device, _buffer), _memory_property_flags))
		{
			BindMemory();
		}

		~VulkanBufferResource()
		{
			_memory.reset();

			if (_buffer)
			{
				vkDestroyBuffer(*_device->GetLogicalDevice(), _buffer, nullptr);
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

		virtual bl Assign(siz offset,
						  const con::vector<ui8>& bytes) override //TODO: support offset and size? This area just feels a little
																  //off -- should this belong in the VulkanBufferView??
		{
			bl assigned = false;
			if (offset + bytes.size() < _size)
			{
				_memory->Remap(offset, bytes.size(), _memory_property_flags);
				BindMemory(); //TODO: is this neccessary?
				_memory->AssignData(bytes);
				assigned = true;
			}
			return assigned;
		}

		/*const mem::sptr<VulkanDeviceMemory> GetDeviceMemory() const
		{
			return _memory;
		}*/
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_BUFFER_RESOURCE_HPP */