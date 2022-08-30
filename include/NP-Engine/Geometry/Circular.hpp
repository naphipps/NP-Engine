//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/27/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_CIRCLE_HPP
#define NP_ENGINE_CIRCLE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/GlmInclude.hpp"

#include "Shape.hpp"

namespace np::geom
{
	template <siz DIMENSION_COUNT, typename T>
	struct Circular : public Shape<DIMENSION_COUNT, T>
	{
		using Point = Shape<DIMENSION_COUNT, T>::Point;

		Point Center;
		dbl Radius;
	};

	template <typename T>
	using Circle = Circular<2, T>;

	template <typename T>
	using Sphere = Circular<3, T>;

	template <typename T>
	using HyperSphere = Circular<4, T>;
}

#endif /* NP_ENGINE_CIRCLE_HPP */