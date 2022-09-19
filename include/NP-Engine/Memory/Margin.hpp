//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/2/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MARGIN_HPP
#define NP_ENGINE_MARGIN_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::mem::__detail
{
	struct Margin
	{
		siz Size = 0;
		bl IsAllocated = false;
	};

	using MarginPtr = Margin*;

	const static siz MARGIN_SIZE = CalcAlignedSize(sizeof(Margin));
} // namespace np::mem::__detail

#endif /* NP_ENGINE_MARGIN_HPP */
