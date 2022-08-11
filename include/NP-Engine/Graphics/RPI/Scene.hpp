//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RPI_SCENE_HPP
#define NP_ENGINE_RPI_SCENE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/ECS/ECS.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/Platform/Platform.hpp"

#include "NP-Engine/Vendor/EnttInclude.hpp"

#include "RhiType.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"

namespace np::graphics
{
	class Scene
	{
	protected:
		::entt::registry& _ecs_registry;
		Renderer& _renderer;
		memory::Delegate _on_draw_delegate;

	public:
		static Scene* Create(memory::Allocator& allocator, ::entt::registry& ecs_registry, Renderer& renderer);

		Scene(::entt::registry& ecs_registry, Renderer& renderer): _ecs_registry(ecs_registry), _renderer(renderer) {}

		virtual ~Scene() {}

		Renderer& GetRenderer()
		{
			return _renderer;
		}

		const Renderer& GetRenderer() const
		{
			return _renderer;
		}

		memory::Delegate& GetOnDrawDelegate()
		{
			return _on_draw_delegate;
		}

		const memory::Delegate& GetOnDrawDelegate() const
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
		virtual void AdjustForWindowResize(window::Window& window) = 0;
	};
} // namespace np::graphics

#endif /* NP_ENGINE_RPI_SCENE_HPP */