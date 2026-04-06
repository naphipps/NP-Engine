//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 1/27/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_ACCESS_HPP
#define NP_ENGINE_GPU_VULKAN_ACCESS_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Access.hpp"

namespace np::gpu::__detail
{
	class VulkanAccess : public Access
	{
	public:
		VulkanAccess(ui32 value): Access(value) {}

		VkAccessFlags GetVkAccessFlags() const
		{
			const bl contains_read = Contains(Read);
			const bl contains_write = Contains(Write);
			VkAccessFlags access = VK_ACCESS_NONE;

			if (Contains(Indirect))
				access |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
			if (Contains(Index))
				access |= VK_ACCESS_INDEX_READ_BIT;
			if (Contains(Vertex))
				access |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
			if (Contains(Uniform))
				access |= VK_ACCESS_UNIFORM_READ_BIT;
			if (Contains(Input))
				access |= VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;

			if (Contains(Shader))
			{
				if (contains_read || !contains_write)
					access |= VK_ACCESS_SHADER_READ_BIT;
				if (contains_write || !contains_read)
					access |= VK_ACCESS_SHADER_WRITE_BIT;
			}

			if (Contains(Image))
			{
				if (contains_read || !contains_write)
					access |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
				if (contains_write || !contains_read)
					access |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			}

			if (ContainsAny(Depth | Stencil))
			{
				if (contains_read || !contains_write)
					access |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
				if (contains_write || !contains_read)
					access |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			}

			if (Contains(Transfer))
			{
				if (contains_read || !contains_write)
					access |= VK_ACCESS_TRANSFER_READ_BIT;
				if (contains_write || !contains_read)
					access |= VK_ACCESS_TRANSFER_WRITE_BIT;
			}

			if (Contains(Host))
			{
				if (contains_read || !contains_write)
					access |= VK_ACCESS_HOST_READ_BIT;
				if (contains_write || !contains_read)
					access |= VK_ACCESS_HOST_WRITE_BIT;
			}

			if (ContainsAll(Read))
				access |= VK_ACCESS_MEMORY_READ_BIT;

			if (ContainsAll(Write))
				access |= VK_ACCESS_MEMORY_WRITE_BIT;

			return access;
		}
	};
} //namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_ACCESS_HPP */