//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/12/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_RENDER_CONTEXT_HPP
#define NP_ENGINE_GPU_INTERFACE_RENDER_CONTEXT_HPP

#include "RenderDevice.hpp"
#include "Context.hpp"

namespace np::gpu
{
	class RenderContext : public Context
	{
	protected:
		mem::sptr<RenderDevice> _device;

		RenderContext(mem::sptr<RenderDevice> device): Context(), _device(device) {}

	public:
		static mem::sptr<RenderContext> Create(mem::sptr<RenderDevice> device);

		virtual ~RenderContext() = default;

		virtual DetailType GetDetailType() const override
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

		/*

		virtual mem::sptr<CommandStaging> ProduceCommandStaging()
		{
#error //TODO: implement
		}

		virtual void ConsumeCommandStaging(CommandStaging& staging)
		{
#error //TODO: implement
		}

		virtual void Begin(CommandStaging& staging)
		{
#error //TODO: implement
		}

		virtual void End(CommandStaging& staging)
		{
#error //TODO: implement
		}

		//*/
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_RENDER_CONTEXT_HPP */