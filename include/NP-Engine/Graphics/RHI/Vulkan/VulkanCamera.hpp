//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/18/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_CAMERA_HPP
#define NP_ENGINE_VULKAN_CAMERA_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/Graphics/RPI/RPI.hpp"

#include "VulkanCommands.hpp"

namespace np::graphics::rhi
{
	class VulkanCamera : public Camera
	{
	private:
		VulkanCommandPushConstants* _push_constants = nullptr;

	public:
		VulkanCamera& operator=(const Camera& camera)
		{
			View = camera.View;
			Projection = camera.Projection;
			return *this;
		}
		/*
		* //TODO: clean this up
		void RenderToFrame(Frame& frame, Pipeline& pipeline)
		{
			VulkanFrame& vulkan_frame = (VulkanFrame&)frame;
			VulkanPipeline& vulkan_pipeline = (VulkanPipeline&)pipeline;

			if (_push_constants)
				memory::Destroy<VulkanCommandPushConstants>(memory::DefaultTraitAllocator, _push_constants);

			_push_constants = memory::Create<VulkanCommandPushConstants>(memory::DefaultTraitAllocator,
				vulkan_pipeline.GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(VulkanCamera), this);

			vulkan_frame.StageCommand(*_push_constants);
		}
		*/
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_CAMERA_HPP */