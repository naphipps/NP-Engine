//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/7/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_OPENGL_RENDERER_HPP
#define NP_ENGINE_GRAPHICS_OPENGL_RENDERER_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Window/Window.hpp"
#include "NP-Engine/Time/Time.hpp"

#include "NP-Engine/Vendor/EnttInclude.hpp"

#include "NP-Engine/Graphics/RPI/RPI.hpp"

// TODO: add summary comments

namespace np::graphics::rhi
{
	class OpenGLRenderer : public Renderer
	{
	public:
		OpenGLRenderer(::entt::registry& ecs_registry): Renderer(ecs_registry) {}

		RhiType GetRhiType() const override
		{
			return RhiType::OpenGL;
		}

		str GetName() const override
		{
			return "OpenGL";
		}

		void AttachToWindow(window::Window& window) override {}

		void DetachFromWindow(window::Window& window) override {}

		void Draw() override {}

		void AdjustForWindowResize(window::Window& window) override {}

		virtual bl IsAttachedToWindow(window::Window& window) const override
		{
			return false;
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_GRAPHICS_OPENGL_RENDERER_HPP */