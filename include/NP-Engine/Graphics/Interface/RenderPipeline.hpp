//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/12/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_RENDER_PIPELINE_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_RENDER_PIPELINE_HPP

#include "NP-Engine/Services/Services.hpp"

#include "Pipeline.hpp"
#include "Framebuffers.hpp"
#include "RenderShader.hpp"
#include "GraphicsDetailType.hpp"

namespace np::gfx
{
	class RenderPipeline : public Pipeline
	{
	public:

		struct Properties
		{
			mem::sptr<Framebuffers> framebuffers;
			mem::sptr<RenderShader> vertexShader;
			mem::sptr<RenderShader> fragmentShader;
			//TODO: I think we should be able to have more than one of each shader (maybe not the vertex shader...)
		};

	protected:

		Properties _properties;

		RenderPipeline(Properties& properties) : _properties(properties) {}

	public:
		static mem::sptr<RenderPipeline> Create(Properties properties);

		virtual GraphicsDetailType GetDetailType() const
		{
			return _properties.framebuffers->GetDetailType();
		}

		virtual Properties GetProperties() const
		{
			return _properties;
		}

		virtual mem::sptr<srvc::Services> GetServices() const
		{
			return _properties.framebuffers->GetServices();
		}
	};
}

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_RENDER_PIPELINE_HPP */