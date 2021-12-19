//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/18/21
//
//##===----------------------------------------------------------------------===##//

#include <cstddef>

#include "NP-Engine/Container/Container.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "../../RPI/Vertex.hpp"

namespace np::graphics::rhi
{
	struct VulkanVertex : public Vertex
	{
		static container::vector<VkVertexInputBindingDescription> BindingDescriptions() 
		{
			container::vector<VkVertexInputBindingDescription> descs{};

			VkVertexInputBindingDescription desc{};
			desc.binding = 0;
			desc.stride = sizeof(Vertex);
			desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			descs.emplace_back(desc);

			return descs;
		}

		static container::array<VkVertexInputAttributeDescription, 2> AttributeDescriptions()
		{
			container::array<VkVertexInputAttributeDescription, 2> descs{};

			descs[0].binding = 0;
			descs[0].location = 0;
			descs[0].format = VK_FORMAT_R32G32_SFLOAT;
			descs[0].offset = offsetof(Vertex, Position);

			descs[1].binding = 0;
			descs[1].location = 1;
			descs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			descs[1].offset = offsetof(Vertex, Color);

			return descs;
		}
	};
}