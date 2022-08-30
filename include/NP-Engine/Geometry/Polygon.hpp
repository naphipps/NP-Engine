//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/27/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_POLYGON_HPP
#define NP_ENGINE_POLYGON_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "NP-Engine/Vendor/GlmInclude.hpp"

#include "Shape.hpp"

namespace np::geom
{
	template <siz DIMENSION_COUNT, typename T>
	struct Polygon : public Shape<DIMENSION_COUNT, T>
	{
		using Point = typename Shape<DIMENSION_COUNT, T>::Point;

		con::vector<Point> Points;
	};

	template <typename T>
	using Polygon2D = Polygon<2, T>;

	template <typename T>
	using Polygon3D = Polygon<3, T>;

	template <typename T>
	using Polygon4D = Polygon<4, T>;
} // namespace np::geom

#endif /* NP_ENGINE_POLYGON_HPP */