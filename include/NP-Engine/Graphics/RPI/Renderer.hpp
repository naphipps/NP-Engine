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

#include "RhiType.hpp"

namespace np::graphics
{
	class Renderer
	{
	public:
		static Renderer* Create(memory::Allocator& allocator);

		virtual ~Renderer() {}

		virtual void RegisterRhiType() const
		{
			__detail::RegisteredRhiType = GetRhiType();
		}

		virtual RhiType GetRhiType() const = 0;

		virtual str GetName() const = 0;

		virtual void AttachToWindow(window::Window& window) = 0;
		virtual void DetachFromWindow(window::Window& window) = 0;
		virtual void Draw(time::DurationMilliseconds time_delta) = 0;
		virtual void AdjustForWindowResize(window::Window& window) = 0;
	};
} // namespace np::graphics

#endif /* NP_ENGINE_RPI_RENDERER_HPP */