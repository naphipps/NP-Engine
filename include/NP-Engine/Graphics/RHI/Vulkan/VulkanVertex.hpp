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

#include "../../RPI/RPI.hpp"

namespace np::graphics::rhi
{
	struct VulkanVertex : public Vertex
	{
		VulkanVertex(const Vertex& other)
		{
			Position = other.Position;
			Color = other.Color;
			TextureCoordinate = other.TextureCoordinate;
		}

		VulkanVertex& operator=(const Vertex& other)
		{
			Position = other.Position;
			Color = other.Color;
			TextureCoordinate = other.TextureCoordinate;
			return *this;
		}

		static container::vector<VkVertexInputBindingDescription> BindingDescriptions()
		{
			container::vector<VkVertexInputBindingDescription> descs{};

			VkVertexInputBindingDescription desc{};
			desc.binding = 0;
			desc.stride = sizeof(VulkanVertex);
			desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			descs.emplace_back(desc);

			return descs;
		}

		static container::array<VkVertexInputAttributeDescription, 3> AttributeDescriptions()
		{
			container::array<VkVertexInputAttributeDescription, 3> descs{};

			descs[0].binding = 0;
			descs[0].location = 0;
			descs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			descs[0].offset = offsetof(VulkanVertex, Position);

			descs[1].binding = 0;
			descs[1].location = 1;
			descs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			descs[1].offset = offsetof(VulkanVertex, Color);

			descs[2].binding = 0;
			descs[2].location = 2;
			descs[2].offset = offsetof(VulkanVertex, TextureCoordinate);
			descs[2].format = VK_FORMAT_R32G32_SFLOAT;

			return descs;
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_VERTEX_HPP */