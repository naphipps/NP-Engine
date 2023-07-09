//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/2/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_COMMAND_POOL_HPP
#define NP_ENGINE_VULKAN_COMMAND_POOL_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanCommandBuffer.hpp"
#include "VulkanLogicalDevice.hpp"

namespace np::gpu::__detail
{
	class VulkanCommandPool
	{
	private:
		mem::sptr<VulkanLogicalDevice> _device;
		VkCommandPool _command_pool;

		static VkCommandPool CreateCommandPool(mem::sptr<VulkanLogicalDevice> device, VkCommandPoolCreateInfo& info)
		{
			VkCommandPool command_pool = nullptr;
			if (vkCreateCommandPool(*device, &info, nullptr, &command_pool) != VK_SUCCESS)
				command_pool = nullptr;

			return command_pool;
		}

	public:
		static VkCommandPoolCreateInfo CreateInfo()
		{
			VkCommandPoolCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			return info;
		}

		VulkanCommandPool(mem::sptr<VulkanLogicalDevice> device, VkCommandPoolCreateInfo command_pool_create_info):
			_device(device),
			_command_pool(CreateCommandPool(_device, command_pool_create_info))
		{}

		VulkanCommandPool(VulkanCommandPool&& other) noexcept:
			_device(::std::move(other._device)),
			_command_pool(::std::move(other._command_pool))
		{
			other._device = nullptr;
			other._command_pool = nullptr;
		}

		~VulkanCommandPool() // TODO: all these need virtual
		{
			Dispose();
		}

		void Dispose()
		{
			if (_command_pool)
			{
				vkDestroyCommandPool(*_device, _command_pool, nullptr);
				_command_pool = nullptr;
			}
		}

		operator VkCommandPool() const
		{
			return _command_pool;
		}

		mem::sptr<VulkanLogicalDevice> GetLogicalDevice() const
		{
			return _device;
		}

		mem::sptr<srvc::Services> GetServices() const
		{
			return _device->GetServices();
		}

		VkCommandBufferAllocateInfo CreateCommandBufferAllocateInfo() const
		{
			VkCommandBufferAllocateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			info.commandPool = _command_pool;
			info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			return info;
		}

		con::vector<mem::sptr<VulkanCommandBuffer>> AllocateCommandBuffers(siz count)
		{
			VkCommandBufferAllocateInfo command_buffer_allocate_info = CreateCommandBufferAllocateInfo();
			command_buffer_allocate_info.commandBufferCount = count;
			return AllocateCommandBuffers(command_buffer_allocate_info);
		}

		con::vector<mem::sptr<VulkanCommandBuffer>> AllocateCommandBuffers(
			const VkCommandBufferAllocateInfo& command_buffer_allocate_info)
		{
			con::vector<VkCommandBuffer> buffers(command_buffer_allocate_info.commandBufferCount);
			if (vkAllocateCommandBuffers(*_device, &command_buffer_allocate_info, buffers.data()) != VK_SUCCESS)
				buffers.clear();

			con::vector<mem::sptr<VulkanCommandBuffer>> vulkan_command_buffers(buffers.size());
			for (siz i = 0; i < buffers.size(); i++)
				vulkan_command_buffers[i] = mem::create_sptr<VulkanCommandBuffer>(GetServices()->GetAllocator(), buffers[i]);

			return vulkan_command_buffers;
		}

		void FreeCommandBuffers(const con::vector<mem::sptr<VulkanCommandBuffer>>& command_buffers)
		{
			con::vector<VkCommandBuffer> buffers(command_buffers.size());
			for (siz i = 0; i < buffers.size(); i++)
				buffers[i] = *command_buffers[i];

			vkFreeCommandBuffers(*_device, _command_pool, (ui32)buffers.size(), buffers.data());
		}

		static void BeginCommandBuffers(con::vector<mem::sptr<VulkanCommandBuffer>>& command_buffers,
										VkCommandBufferBeginInfo& begin_info)
		{
			for (mem::sptr<VulkanCommandBuffer>& command_buffer : command_buffers)
				command_buffer->Begin(begin_info);
		}

		static void BeginCommandBuffers(con::vector<mem::sptr<VulkanCommandBuffer>>& command_buffers,
										con::vector<VkCommandBufferBeginInfo>& begin_infos)
		{
			NP_ENGINE_ASSERT(command_buffers.size() == begin_infos.size(), "command_buffers size must equal begin_infos size");
			for (siz i = 0; i < command_buffers.size(); i++)
				command_buffers[i]->Begin(begin_infos[i]);
		}

		static void EndCommandBuffers(con::vector<mem::sptr<VulkanCommandBuffer>>& command_buffers)
		{
			for (mem::sptr<VulkanCommandBuffer>& command_buffer : command_buffers)
				command_buffer->End();
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_VULKAN_COMMAND_POOL_HPP */