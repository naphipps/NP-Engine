//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 1/7/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_FRAMEBUFFERS_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_FRAMEBUFFERS_HPP

#include "NP-Engine/Memory/Memory.hpp"

#include "GraphicsDetailType.hpp"
#include "RenderPass.hpp"

namespace np::gfx
{
	class Framebuffers
	{
	protected:

		mem::sptr<RenderPass> _render_pass;

		Framebuffers(mem::sptr<RenderPass> render_pass): _render_pass(render_pass) {}

	public:
		static mem::sptr<Framebuffers> Create(mem::sptr<RenderPass> render_pass);

		virtual GraphicsDetailType GetDetailType() const
		{
			return _render_pass->GetDetailType();
		}

		virtual mem::sptr<RenderPass> GetRenderPass() const
		{
			return _render_pass;
		}

		virtual mem::sptr<srvc::Services> GetServices() const
		{
			return _render_pass->GetServices();
		}
	};
}

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_FRAMEBUFFERS_HPP */