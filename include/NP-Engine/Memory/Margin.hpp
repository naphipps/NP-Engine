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

		//TODO: go back to a POD class with setters and getters
		//TODO: go back to a compressed size and allocation
	};

	using MarginPtr = Margin*;

	const static siz MARGIN_SIZE = CalcAlignedSize(sizeof(Margin));
} // namespace np::mem::__detail

#endif /* NP_ENGINE_MARGIN_HPP */
