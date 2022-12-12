//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/15/22
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Graphics/Interface/RenderableModel.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/Graphics/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/Graphics/Detail/Vulkan/VulkanGraphics.hpp"

namespace np::gfx
{
	RenderableModel* RenderableModel::Create(srvc::Services& services, Model& model)
	{
		RenderableModel* renderable_model = nullptr;

		switch (__detail::RegisteredGraphicsDetailType.load(mo_acquire))
		{
		case GraphicsDetailType::Vulkan:
			renderable_model = mem::Create<__detail::VulkanRenderableModel>(services.GetAllocator(), services, model);
			break;
		default:
			break;
		}

		return renderable_model;
	}
} // namespace np::gfx