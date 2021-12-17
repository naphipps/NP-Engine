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

#include "../../RPI/Renderer.hpp"
#include "../../RPI/RhiType.hpp"

// TODO: add summary comments

namespace np::graphics::rhi
{
	class OpenGLRenderer : public Renderer
	{
	public:
		OpenGLRenderer() {}

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

		void Draw(time::DurationMilliseconds time_delta) override {}

		void AdjustForWindowResize(window::Window& window) override {}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_GRAPHICS_OPENGL_RENDERER_HPP */