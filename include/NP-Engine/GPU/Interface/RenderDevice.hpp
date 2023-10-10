//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/12/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_RENDER_DEVICE_HPP
#define NP_ENGINE_GPU_INTERFACE_RENDER_DEVICE_HPP

#include "RenderTarget.hpp"
#include "Device.hpp"
#include "Model.hpp"

namespace np::gpu
{
	class RenderDevice : public Device
	{
	protected:
		mem::sptr<RenderTarget> _target;

		RenderDevice(mem::sptr<RenderTarget> target): Device(), _target(target) {}

	public:
		static mem::sptr<RenderDevice> Create(mem::sptr<RenderTarget> target);

		virtual ~RenderDevice() = default;

		virtual mem::sptr<DetailInstance> GetInstance() const override
		{
			return _target->GetInstance();
		}

		virtual DetailType GetDetailType() const
		{
			return _target->GetDetailType();
		}

		virtual mem::sptr<RenderTarget> GetRenderTarget() const
		{
			return _target;
		}

		virtual mem::sptr<srvc::Services> GetServices() const override
		{
			return _target->GetServices();
		}
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_RENDER_DEVICE_HPP */