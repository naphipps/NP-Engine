//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/GPU/Interface/Shader.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/GPU/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/GPU/Detail/Vulkan/VulkanShader.hpp"

namespace np::gpu
{
	mem::sptr<Shader> Shader::Create(mem::sptr<Device> device, Stage stage, str filename, str entrypoint)
	{
		mem::sptr<Shader> shader = nullptr;

		switch (device->GetDetailType())
		{
		case DetailType::Vulkan:
			shader = mem::create_sptr<__detail::VulkanShader>(device->GetServices()->GetAllocator(), device, stage, filename, entrypoint);
			break;

		default:
			break;
		}

		return shader;
	}
} // namespace np::gpu