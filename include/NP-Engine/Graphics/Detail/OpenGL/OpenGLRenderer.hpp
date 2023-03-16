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

#include "NP-Engine/Graphics/Interface/Interface.hpp"

namespace np::gfx::__detail
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
		OpenGLRenderer(srvc::Services& services): Renderer(services) {}

		GraphicsDetailType GetGraphicsDetailType() const override
		{
			return GraphicsDetailType::OpenGL;
		}

		str GetName() const override
		{
			return "OpenGL";
		}

		void AttachToWindow(win::Window& window) override {}

		void DetachFromWindow(uid::Uid windowId) override {}

		Frame& BeginFrame() override
		{
			return _frame;
		}

		void EndFrame() override {}

		void DrawFrame() override {}

		virtual bl IsAttachedToWindow(uid::Uid windowId) const override
		{
			return false;
		}
	};
} // namespace np::gfx::__detail

#endif /* NP_ENGINE_GRAPHICS_OPENGL_RENDERER_HPP */