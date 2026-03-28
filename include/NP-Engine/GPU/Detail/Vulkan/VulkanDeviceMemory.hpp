//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/26/25
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_DEVICE_MEMORY_HPP
#define NP_ENGINE_GPU_VULKAN_DEVICE_MEMORY_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanLogicalDevice.hpp"

namespace np::gpu::__detail
{
	class VulkanDeviceMemory
	{
	private:
		mem::sptr<VulkanLogicalDevice> _device;
		VkDeviceMemory _memory;
		void* _mapping;

		static VkMemoryAllocateInfo CreateVkMemoryAllocateInfo(mem::sptr<VulkanLogicalDevice> device,
															   VkMemoryRequirements requirements, VkMemoryPropertyFlags flags)
		{
			::std::optional<ui32> memory_type_index =
				device->GetPhysicalDevice().GetMemoryTypeIndex(requirements.memoryTypeBits, flags);

			VkMemoryAllocateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			info.allocationSize = requirements.size;
			info.memoryTypeIndex = memory_type_index.has_value()
				? memory_type_index.value()
				: UI32_MAX; //TODO: what do I do if we don't have a value here? pg83 of guide does UI32_MAX
			return info;
		}

		static VkDeviceMemory CreateVkDeviceMemory(mem::sptr<VulkanLogicalDevice> device, VkMemoryRequirements requirements,
												   VkMemoryPropertyFlags flags)
		{
			VkMemoryAllocateInfo info = CreateVkMemoryAllocateInfo(device, requirements, flags);
			VkDeviceMemory memory = nullptr;
			VkResult result = vkAllocateMemory(*device, &info, nullptr, &memory);
			return result == VK_SUCCESS ? memory : nullptr;
		}

	public:
		VulkanDeviceMemory(mem::sptr<VulkanLogicalDevice> device, VkMemoryRequirements requirements,
						   VkMemoryPropertyFlags flags):
			_device(device),
			_memory(CreateVkDeviceMemory(device, requirements, flags)),
			_mapping(nullptr)
		{}

		~VulkanDeviceMemory()
		{
			if (_memory)
			{
				Unmap();
				vkFreeMemory(*_device, _memory, nullptr);
				_memory = nullptr;
			}
		}

		operator VkDeviceMemory() const
		{
			return _memory;
		}

		mem::sptr<VulkanLogicalDevice> GetDevice() const
		{
			return _device;
		}

		siz GetCommitment() const
		{
			VkDeviceSize commitment;
			vkGetDeviceMemoryCommitment(*_device, _memory, &commitment);
			return commitment;
		}

		//TODO: I don't know if we want to expose mapping?

		bl IsMapped() const
		{
			return _mapping;
		}

		void Map(siz offset, siz size, VkMemoryMapFlags flags)
		{
			if (!IsMapped())
			{
				VkResult result = vkMapMemory(*_device, _memory, offset, size, flags, &_mapping);
				if (result != VK_SUCCESS)
					_mapping = nullptr;
			}
		}

		void Unmap()
		{
			if (IsMapped())
			{
				vkUnmapMemory(*_device, _memory);
				_mapping = nullptr;
			}
		}

		void Remap(siz offset, siz size, VkMemoryMapFlags flags)
		{
			Unmap();
			Map(offset, size, flags);
		}

		void AssignData(const con::vector<ui8>& bytes)
		{
			if (IsMapped())
				mem::copy_bytes(_mapping, bytes);
		}

		bl ClearCacheForDevice(siz offset, siz size) const //TODO: support offset and size?
		{
			VkMappedMemoryRange range{};
			range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			range.memory = _memory;
			range.offset = offset;
			range.size = size; //TODO: consider GetSize()?
			VkResult result = vkFlushMappedMemoryRanges(*_device, 1, &range);
			return result == VK_SUCCESS;
		}

		bl ClearCacheForHost(siz offset, siz size) const
		{
			VkMappedMemoryRange range{};
			range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			range.memory = _memory;
			range.offset = offset;
			range.size = size; //TODO: consider GetSize()?
			VkResult result = vkInvalidateMappedMemoryRanges(*_device, 1, &range);
			return result == VK_SUCCESS;
		}
	};
} //namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_DEVICE_MEMORY_HPP */