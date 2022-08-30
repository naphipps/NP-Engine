//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/27/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_SHAPE_HPP
#define NP_ENGINE_SHAPE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/GlmInclude.hpp"

namespace np::geom
{
	template <siz DIMENSION_COUNT, typename T>
	struct Shape
	{
		using Point = ::glm::vec<DIMENSION_COUNT, T>;
	};
}

#endif /* NP_ENGINE_SHAPE_HPP */