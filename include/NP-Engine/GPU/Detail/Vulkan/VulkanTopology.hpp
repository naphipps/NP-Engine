//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/11/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_TOPOLOGY_HPP
#define NP_ENGINE_GPU_VULKAN_TOPOLOGY_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Topology.hpp"

namespace np::gpu::__detail
{
	class VulkanPrimitiveTopology : public PrimitiveTopology
	{
	public:
		VulkanPrimitiveTopology(ui32 value): PrimitiveTopology(value) {}

		VkPrimitiveTopology GetVkPrimitiveTopology() const
		{
			VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;

			if (Contains(Point)) //TODO: check all Enum<T> checks where we use ContainsAll when we should use Contains -- make
								 //sure all checks are correct
				topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
			else if (Contains(Line | List | Adjacency))
				topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
			else if (Contains(Line | List))
				topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			else if (Contains(Line | Strip | Adjacency))
				topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
			else if (Contains(Line | Strip))
				topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
			else if (Contains(Triangle | List | Adjacency))
				topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
			else if (Contains(Triangle | List))
				topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			else if (Contains(Triangle | Strip | Adjacency))
				topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
			else if (Contains(Triangle | Strip))
				topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
			else if (Contains(Triangle | Fan))
				topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
			else if (Contains(Patch | List))
				topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;

			return topology;
		}
	};
} //namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_TOPOLOGY_HPP */