//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/7/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_RENDERER_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_RENDERER_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Window/Window.hpp"
#include "NP-Engine/Services/Services.hpp"

#include "GraphicsDetailType.hpp"
#include "Model.hpp"
#include "Frame.hpp"
#include "RenderableLight.hpp"
#include "RenderableObject.hpp"

namespace np::gfx
{
	class Renderer
	{
	protected:
		srvc::Services& _services;

		Renderer(srvc::Services& services): _services(services) {}

	public:
		static Renderer* Create(srvc::Services& services);

		virtual ~Renderer() {}

		virtual void RegisterGraphicsDetailType() const
		{
			__detail::RegisteredGraphicsDetailType.store(GetGraphicsDetailType(), mo_release);
		}

		virtual GraphicsDetailType GetGraphicsDetailType() const = 0;

		virtual str GetName() const = 0;

		virtual void AttachToWindow(win::Window& window) = 0;
		virtual void DetachFromWindow(uid::Uid windowId) = 0;

		virtual Frame& BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void DrawFrame() = 0;

		virtual bl IsAttachedToWindow(uid::Uid windowId) const = 0;
	};
} // namespace np::gfx

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_RENDERER_HPP */