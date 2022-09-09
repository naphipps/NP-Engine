//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/27/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_LINE_HPP
#define NP_ENGINE_LINE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/GlmInclude.hpp"

#include "Shape.hpp"

// TODO: do we add besier curves/lines here?? I think they go in their own file

namespace np::geom
{
	template <siz DIMENSION_COUNT, typename T>
	struct Line : public Shape<DIMENSION_COUNT, T>
	{
		using Point = typename Shape<DIMENSION_COUNT, T>::Point;

		Point Begin = {0};
		Point End = {0};
	};

	template <typename T>
	using Line2D = Line<2, T>;

	template <typename T>
	using Line3D = Line<3, T>;

	template <typename T>
	using Line4D = Line<4, T>;
} // namespace np::geom

#endif /* NP_ENGINE_LINE_HPP */