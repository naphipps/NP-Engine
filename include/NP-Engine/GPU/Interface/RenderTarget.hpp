//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/12/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_RENDER_TARGET_HPP
#define NP_ENGINE_GPU_INTERFACE_RENDER_TARGET_HPP

#include "NP-Engine/Window/Window.hpp"

#include "DetailType.hpp"
#include "DetailInstance.hpp"

// TODO: add ability to render to just half the window/surface, etc -- will be helpful adding DX12/etc support

namespace np::gpu
{
	class RenderTarget
	{
	protected:
		mem::sptr<DetailInstance> _instance;
		mem::sptr<win::Window> _window;

		RenderTarget(mem::sptr<DetailInstance> instance, mem::sptr<win::Window> window): _instance(instance), _window(window) {}

	public:
		static mem::sptr<RenderTarget> Create(mem::sptr<DetailInstance> instance, mem::sptr<win::Window> window);

		virtual ~RenderTarget() = default;

		virtual DetailType GetDetailType() const
		{
			return _instance->GetDetailType();
		}

		virtual mem::sptr<DetailInstance> GetInstance() const
		{
			return _instance;
		}

		virtual mem::sptr<srvc::Services> GetServices() const
		{
			return _instance->GetServices();
		}

		virtual mem::sptr<win::Window> GetWindow() const
		{
			return _window;
		}
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_RENDER_TARGET_HPP */