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
		Point origin = {0, 0};
		Point boundary = {0, 0};

		bl Contains(const Point& point) const
		{
			return point.x >= origin.x && point.x < boundary.x - 1 && point.y >= origin.y && point.y < boundary.y - 1;
		}
	};
} // namespace np::gfxalg

#endif /* NP_ENGINE_SUBVIEW_HPP */