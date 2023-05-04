//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/8/23
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/GPU/Interface/RenderShader.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
#include "NP-Engine/GPU/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/GPU/Detail/Vulkan/VulkanRenderShader.hpp"

namespace np::gpu
{
	mem::sptr<RenderShader> RenderShader::Create(mem::sptr<RenderDevice> device, Shader::Properties properties)
	{
		mem::sptr<RenderShader> shader = nullptr;

		switch (device->GetDetailType())
		{
		case DetailType::Vulkan:
			shader = mem::create_sptr<__detail::VulkanRenderShader>(device->GetServices()->GetAllocator(), device, properties);
			break;

		default:
			break;
		}

		return shader;
	}
}