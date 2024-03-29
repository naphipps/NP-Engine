//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/18/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_VERTEX_HPP
#define NP_ENGINE_VULKAN_VERTEX_HPP

#include <cstddef>

#include "NP-Engine/Container/Container.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "../../Interface/Interface.hpp"

namespace np::gpu::__detail
{
	struct VulkanVertex : public Vertex
	{
		VulkanVertex(const Vertex& other)
		{
			position = other.position;
			color = other.color;
			uv = other.uv;
		}

		VulkanVertex& operator=(const Vertex& other)
		{
			position = other.position;
			color = other.color;
			uv = other.uv;
			return *this;
		}

		static con::vector<VkVertexInputBindingDescription> BindingDescriptions()
		{
			con::vector<VkVertexInputBindingDescription> descs{};

			VkVertexInputBindingDescription desc{};
			desc.binding = 0;
			desc.stride = sizeof(VulkanVertex);
			desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			descs.emplace_back(desc);

			return descs;
		}

		static con::array<VkVertexInputAttributeDescription, 3> AttributeDescriptions()
		{
			con::array<VkVertexInputAttributeDescription, 3> descs{};

			descs[0].binding = 0;
			descs[0].location = 0;
			descs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			descs[0].offset = offsetof(VulkanVertex, position);

			descs[1].binding = 0;
			descs[1].location = 1;
			descs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			descs[1].offset = offsetof(VulkanVertex, color);

			descs[2].binding = 0;
			descs[2].location = 2;
			descs[2].offset = offsetof(VulkanVertex, uv);
			descs[2].format = VK_FORMAT_R32G32_SFLOAT;

			return descs;
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_VULKAN_VERTEX_HPP */