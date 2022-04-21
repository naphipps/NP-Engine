//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/15/22
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Graphics/RPI/RenderableModel.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/Graphics/RHI/OpenGL/OpenGLRenderer.hpp" //TODO: create OpenGLGraphics.hpp
#endif

#include "NP-Engine/Graphics/RHI/Vulkan/VulkanGraphics.hpp"

namespace np::graphics
{
	RenderableModel* RenderableModel::Create(memory::Allocator& allocator, Model& model)
	{
		RenderableModel* renderable_model = nullptr;

		switch (__detail::RegisteredRhiType.load(mo_acquire))
		{
		case RhiType::Vulkan:
			renderable_model = memory::Create<rhi::VulkanRenderableModel>(allocator, model);
			break;
		default:
			break;
		}

		return renderable_model;
	}
} // namespace np::graphics