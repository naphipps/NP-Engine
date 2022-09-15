//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/15/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MOUSE_POSITION_HPP
#define NP_ENGINE_MOUSE_POSITION_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/GlmInclude.hpp"

namespace np::nput
{
	class MousePosition
	{
	private:
		bl _is_over_surface = true;
		::glm::vec2 _position{0};

	public:

		bl IsOverSurface() const
		{
			return _is_over_surface;
		}

		::glm::vec2 GetPosition() const
		{
			return IsOverSurface() ? _position : ::glm::vec2{};
		}

		void SetIsOverSurface(bl is_over_screen)
		{
			_is_over_surface = is_over_screen;
		}

		void SetPosition(const ::glm::vec2& position)
		{
			_position = position;
		}
	};
}

#endif /* NP_ENGINE_MOUSE_POSITION_HPP */