//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/12/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_RENDER_PASS_HPP
#define NP_ENGINE_GPU_INTERFACE_RENDER_PASS_HPP

#include "NP-Engine/Memory/Memory.hpp"

#include "DetailType.hpp"
#include "Pass.hpp"
#include "RenderContext.hpp"

namespace np::gpu
{
	class RenderPass : public Pass
	{
	protected:

		mem::sptr<RenderContext> _context;

		RenderPass(mem::sptr<RenderContext> context) : Pass(), _context(context) {}

	public:
		static mem::sptr<RenderPass> Create(mem::sptr<RenderContext> context);

		virtual DetailType GetDetailType() const
		{
			return _context->GetDetailType();
		}

		virtual mem::sptr<RenderContext> GetRenderContext() const
		{
			return _context;
		}

		virtual mem::sptr<srvc::Services> GetServices() const
		{
			return _context->GetServices();
		}
	};
}

#endif /* NP_ENGINE_GPU_INTERFACE_RENDER_PASS_HPP */