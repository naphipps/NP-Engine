//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_SCENE_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_SCENE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/ECS/ECS.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Services/Services.hpp"

#include "GraphicsDetailType.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"

namespace np::gfx
{
	class Scene
	{
	protected:
		srvc::Services& _services;
		Renderer& _renderer;
		mem::Delegate _on_draw_delegate;

	public:
		static Scene* Create(srvc::Services& services, Renderer& renderer);

		Scene(srvc::Services& services, Renderer& renderer): _services(services), _renderer(renderer) {}

		virtual ~Scene() {}

		Renderer& GetRenderer()
		{
			return _renderer;
		}

		const Renderer& GetRenderer() const
		{
			return _renderer;
		}

		mem::Delegate& GetOnDrawDelegate()
		{
			return _on_draw_delegate;
		}

		const mem::Delegate& GetOnDrawDelegate() const
		{
			return _on_draw_delegate;
		}

		// TODO: flesh out the following:
		virtual void Add(RenderableObject& renderable_object) {}
		virtual void Add(RenderableLightObject& renderable_light_object) {}
		virtual void Remove(RenderableObject& renderable_object) {}
		virtual void Remove(RenderableLightObject& renderable_light_object) {}

		virtual void Render() = 0;
		virtual void Prepare() = 0;
		virtual void Dispose() = 0;
		virtual void SetCamera(Camera& camera) = 0;
		virtual void AdjustForWindowResize(win::Window& window) = 0;
	};
} // namespace np::gfx

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_SCENE_HPP */