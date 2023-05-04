//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/27/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_SUBVIEW_HPP
#define NP_ENGINE_SUBVIEW_HPP

#include "NP-Engine/GPU/GPU.hpp"

namespace np::gpualg
{
	struct Subview
	{
		using Point = gpu::Image::Point;
		Point origin = {0, 0};
		Point boundary = {0, 0};

		bl Contains(const Point& point) const
		{
			return point.x >= origin.x && point.x < boundary.x - 1 && point.y >= origin.y && point.y < boundary.y - 1;
		}
	};
} // namespace np::gpualg

#endif /* NP_ENGINE_SUBVIEW_HPP */