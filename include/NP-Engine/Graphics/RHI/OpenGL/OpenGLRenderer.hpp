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
#include "NP-Engine/Services/Services.hpp"

#include "NP-Engine/Vendor/EnttInclude.hpp"

#include "NP-Engine/Graphics/RPI/RPI.hpp"

namespace np::graphics::rhi
{
	namespace __detail
	{
		struct OpenGLFrame : public Frame
		{
			virtual bl IsValid() const override
			{
				return false;
			}

			virtual void Invalidate() override {}
		};
	} // namespace __detail

	class OpenGLRenderer : public Renderer
	{
	private:
		__detail::OpenGLFrame _frame;

	public:
		OpenGLRenderer(services::Services& services): Renderer(services) {}

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

		Frame& BeginFrame() override
		{
			return _frame;
		}

		void EndFrame() override {}

		void DrawFrame() override {}

		void AdjustForWindowResize(window::Window& window) override {}

		virtual bl IsAttachedToWindow(window::Window& window) const override
		{
			return false;
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_GRAPHICS_OPENGL_RENDERER_HPP */