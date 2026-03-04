//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/2/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_COMMAND_BUFFER_POOL_HPP
#define NP_ENGINE_GPU_VULKAN_COMMAND_BUFFER_POOL_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanCommandBuffer.hpp"
#include "VulkanLogicalDevice.hpp"

namespace np::gpu::__detail
{
	class VulkanCommandBufferPoolUsage : public CommandBufferPoolUsage
	{
	public:
		VulkanCommandBufferPoolUsage(ui32 value) : CommandBufferPoolUsage(value) {}

		VkCommandPoolCreateFlags GetVkCommandPoolCreateFlags() const
		{
			VkCommandPoolCreateFlags flags = 0;

			if (Contains(Transient))
				flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
			if (Contains(Resettable))
				flags |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

			return flags;
		}
	};

	class VulkanCommandBufferPool : public CommandBufferPool
	{
	private:
		class VulkanCommandBufferDestroyer : public mem::smart_ptr_contiguous_destroyer<VulkanCommandBuffer>
		{
		private:
			using base = mem::smart_ptr_contiguous_destroyer<VulkanCommandBuffer>;
			mem::sptr<VulkanLogicalDevice> _device;
			VkCommandPool _pool;

		public:
			VulkanCommandBufferDestroyer(mem::sptr<VulkanLogicalDevice> device, VkCommandPool pool, mem::Allocator& allocator) :
				base(allocator),
				_device(device),
				_pool(pool)
			{}

			VulkanCommandBufferDestroyer(const VulkanCommandBufferDestroyer& other) :
				base(other._allocator),
				_device(other._device),
				_pool(other._pool)
			{}

			VulkanCommandBufferDestroyer(VulkanCommandBufferDestroyer&& other) noexcept :
				base(other._allocator),
				_device(::std::move(other._device)),
				_pool(::std::move(other._pool))
			{
				other._device.reset();
				other._pool = nullptr;
			}

			void destruct_object(VulkanCommandBuffer* ptr) override
			{
				const VkCommandBuffer command_buffer = *ptr;
				vkFreeCommandBuffers(*_device, _pool, 1, &command_buffer);
				base::destruct_object(ptr);
			}
		};

		mem::sptr<VulkanLogicalDevice> _device;
		VkCommandPool _pool;

		static VkCommandBufferAllocateInfo GetVkCommandBufferAllocateInfo(VkCommandPool pool, siz count)
		{
			VkCommandBufferAllocateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			info.commandPool = pool;
			info.commandBufferCount = count;
			return info;
		}

		static VkCommandBufferBeginInfo GetVkCommandBufferBeginInfo(VulkanCommandBufferUsage usage)
		{
			VkCommandBufferBeginInfo info{};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			info.flags = usage.GetVkCommandBufferUsageFlags();
			return info;
		}

		static VkCommandPoolCreateInfo CreateVkInfo(VulkanCommandBufferPoolUsage usage, DeviceQueueFamily family)
		{
			VkCommandPoolCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			info.queueFamilyIndex = family.index;
			info.flags = usage.GetVkCommandPoolCreateFlags();
			return info;
		}

		static VkCommandPool CreateVkCommandPool(mem::sptr<VulkanLogicalDevice> device, VulkanCommandBufferPoolUsage usage, DeviceQueueFamily family)
		{
			VkCommandPoolCreateInfo info = CreateVkInfo(usage, family);
			VkCommandPool pool = nullptr;
			VkResult result = vkCreateCommandPool(*device, &info, nullptr, &pool);
			return result == VK_SUCCESS ? pool : nullptr;
		}

	public:
		VulkanCommandBufferPool(mem::sptr<VulkanLogicalDevice> device, CommandBufferPoolUsage usage, DeviceQueueFamily family) :
			_device(device),
			_pool(CreateVkCommandPool(_device, VulkanCommandBufferPoolUsage{ usage }, family))
		{}

		~VulkanCommandBufferPool()
		{
			if (_pool)
			{
				vkDestroyCommandPool(*_device, _pool, nullptr);
				_pool = nullptr;
			}
		}

		operator VkCommandPool() const
		{
			return _pool;
		}

		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual mem::sptr<srvc::Services> GetServices() const override
		{
			return _device->GetServices();
		}

		virtual mem::sptr<CommandBuffer> CreateCommandBuffer() override
		{
			using destroyer_type = VulkanCommandBufferDestroyer;
			using resource_type = mem::smart_ptr_resource<VulkanCommandBuffer, destroyer_type>;
			using contiguous_block_type = mem::smart_ptr_contiguous_block<VulkanCommandBuffer, resource_type>;

			mem::Allocator& allocator = GetServices()->GetAllocator();
			resource_type* resource = nullptr;
			contiguous_block_type* contiguous_block = mem::Create<contiguous_block_type>(allocator);

			if (contiguous_block)
			{
				VkCommandBufferAllocateInfo info = GetVkCommandBufferAllocateInfo(_pool, 1);
				VkCommandBuffer command_buffer = nullptr;
				VkResult result = vkAllocateCommandBuffers(*_device, &info, &command_buffer);

				if (result == VK_SUCCESS)
				{
					VulkanCommandBuffer* object = mem::Construct<VulkanCommandBuffer>(contiguous_block->object_block, GetServices(), command_buffer);
					resource = mem::Construct<resource_type>(contiguous_block->resource_block, destroyer_type{ _device, info.commandPool, allocator }, object);
				}
			}
			return { resource };
		}

		virtual bl Begin(mem::sptr<CommandBuffer> command_buffer, CommandBufferUsage usage) override
		{
			bl begin = false;
			mem::sptr<VulkanCommandBuffer> vulkan_command_buffer = DetailObject::EnsureIsDetailType(command_buffer, DetailType::Vulkan);
			if (vulkan_command_buffer)
			{
				VkCommandBufferBeginInfo info = GetVkCommandBufferBeginInfo(VulkanCommandBufferUsage{ usage });
				VkResult result = vkBeginCommandBuffer(*vulkan_command_buffer, &info);
				begin = result == VK_SUCCESS;
			}

			return begin;
		}

		virtual bl End(mem::sptr<CommandBuffer> command_buffer, CommandBufferUsage usage) override
		{
			bl end = false;
			mem::sptr<VulkanCommandBuffer> vulkan_command_buffer = DetailObject::EnsureIsDetailType(command_buffer, DetailType::Vulkan);
			if (vulkan_command_buffer)
			{
				VkResult result = vkEndCommandBuffer(*vulkan_command_buffer);
				end = result == VK_SUCCESS;
			}
			return end;
		}

		virtual bl Reset(mem::sptr<CommandBuffer> command_buffer, CommandBufferUsage usage) override
		{
			bl reset = false;
			mem::sptr<VulkanCommandBuffer> vulkan_command_buffer = DetailObject::EnsureIsDetailType(command_buffer, DetailType::Vulkan);
			if (vulkan_command_buffer)
			{
				VkResult result = vkResetCommandBuffer(*vulkan_command_buffer, VulkanCommandBufferUsage{ usage }.GetVkCommandBufferResetFlags());
				reset = result == VK_SUCCESS;
			}
			return reset;
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_COMMAND_BUFFER_POOL_HPP */