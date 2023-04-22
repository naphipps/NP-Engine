//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/6/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_RENDER_SHADER_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_RENDER_SHADER_HPP

#include "NP-Engine/Services/Services.hpp"

#include "RenderDevice.hpp"
#include "Shader.hpp"
#include "GraphicsDetailType.hpp"

namespace np::gfx
{
	class RenderShader : public Shader
	{
	protected:

		mem::sptr<RenderDevice> _device;

		RenderShader(mem::sptr<RenderDevice> device, Shader::Properties& properties) : Shader(properties), _device(device) {}

	public:
		static mem::sptr<RenderShader> Create(mem::sptr<RenderDevice> device, Shader::Properties properties);

		virtual GraphicsDetailType GetDetailType() const
		{
			return _device->GetDetailType();
		}

		virtual mem::sptr<RenderDevice> GetRenderDevice() const
		{
			return _device;
		}

		virtual mem::sptr<srvc::Services> GetServices() const
		{
			return _device->GetServices();
		}
	};
}

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_RENDER_SHADER_HPP */