//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/7/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RPI_RENDERER_HPP
#define NP_ENGINE_RPI_RENDERER_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Window/Window.hpp"

#include "NP-Engine/Vendor/EnttInclude.hpp"

#include "RhiType.hpp"
#include "Model.hpp"
#include "Frame.hpp"
#include "RenderableLight.hpp"
#include "RenderableObject.hpp"

namespace np::graphics
{
	class Renderer
	{
	protected:
		::entt::registry& _ecs_registry;

		Renderer(::entt::registry& ecs_registry): _ecs_registry(ecs_registry) {}

		virtual void Destruct() {}

	public:
		static Renderer* Create(memory::Allocator& allocator, ::entt::registry& ecs_registry);

		virtual ~Renderer()
		{
			Destruct();
		}

		virtual void RegisterRhiType() const
		{
			__detail::RegisteredRhiType.store(GetRhiType(), mo_release);
		}

		virtual RhiType GetRhiType() const = 0;

		virtual str GetName() const = 0;

		virtual void AttachToWindow(window::Window& window) = 0;
		virtual void DetachFromWindow(window::Window& window) = 0;

		virtual Frame& BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void DrawFrame() = 0;

		virtual void AdjustForWindowResize(window::Window& window) = 0;
		virtual bl IsAttachedToWindow(window::Window& window) const = 0;
	};
} // namespace np::graphics

#endif /* NP_ENGINE_RPI_RENDERER_HPP */