//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/12/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_RENDER_DEVICE_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_RENDER_DEVICE_HPP

#include "RenderTarget.hpp"
#include "Device.hpp"
#include "Model.hpp"

namespace np::gfx
{
	class RenderDevice : public Device
	{
	protected:
		mem::sptr<RenderTarget> _target;

		RenderDevice(mem::sptr<RenderTarget> target) : Device(), _target(target) {}

	public:
		static mem::sptr<RenderDevice> Create(mem::sptr<RenderTarget> target);

		virtual mem::sptr<DetailInstance> GetInstance() const override
		{
			return _target->GetInstance();
		}

		virtual GraphicsDetailType GetDetailType() const
		{
			return _target->GetDetailType();
		}

		virtual mem::sptr<RenderTarget> GetRenderTarget() const
		{
			return _target;
		}

		virtual mem::sptr<srvc::Services> GetServices() const
		{
			return _target->GetServices();
		}
	};
}

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_RENDER_DEVICE_HPP */