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
		siz Value = 0;

		bl IsAllocated() const
		{
			return Value & 1;
		}

		siz GetSize() const
		{
			return (Value >> 1) << 1;
		}

		void SetSize(siz size)
		{
			Value = IsAllocated() ? size & 1 : size;
		}

		void SetAllocated()
		{
			Value |= 1;
		}

		void SetDeallocated()
		{
			Value = GetSize();
		}
	};

	using MarginPtr = Margin*;

	const static siz MARGIN_ALIGNED_SIZE = CalcAlignedSize(sizeof(Margin));
} // namespace np::mem::__detail

#endif /* NP_ENGINE_MARGIN_HPP */
