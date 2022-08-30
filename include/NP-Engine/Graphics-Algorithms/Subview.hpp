//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/27/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_SUBVIEW_HPP
#define NP_ENGINE_SUBVIEW_HPP

#include "NP-Engine/Graphics/Graphics.hpp"

namespace np::gfxalg
{
	struct Subview
	{
		using Point = gfx::Image::Point;
		Point Origin = {0, 0};
		Point Boundary = {0, 0};

		bl Contains(const Point& point) const
		{
			return point.x >= Origin.x && point.x <= Boundary.x - 1 && point.y >= Origin.y && point.y <= Boundary.y - 1;
		}
	};
} // namespace np::gfxalg

#endif /* NP_ENGINE_SUBVIEW_HPP */