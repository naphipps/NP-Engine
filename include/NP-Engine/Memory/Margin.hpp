//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/2/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MARGIN_HPP
#define NP_ENGINE_MARGIN_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "Allocator.hpp"

namespace np::mem::__detail
{
	class Margin
	{
	private:
		NP_ENGINE_STATIC_ASSERT(ALIGNMENT % 2 == 0, "This implementation requires an even ALIGNMENT because we use that last bit.");

		siz _size_and_is_allocated = 0;

	public:
		siz GetSize() const
		{
			return _size_and_is_allocated & ~ALIGNMENT_MINUS_ONE;
		}

		bl IsAllocated() const
		{
			return (_size_and_is_allocated & ALIGNMENT_MINUS_ONE) == 1;
		}

		void SetSize(siz size)
		{
			_size_and_is_allocated = (size & ~ALIGNMENT_MINUS_ONE) | (_size_and_is_allocated & ALIGNMENT_MINUS_ONE);
		}

		void SetIsAllocated(bl is_allocated)
		{
			_size_and_is_allocated = GetSize() | ((is_allocated ? 1 : 0) & ALIGNMENT_MINUS_ONE);
		}
	};

	const static siz MARGIN_SIZE = CalcAlignedSize(sizeof(Margin));
} // namespace np::mem::__detail

#endif /* NP_ENGINE_MARGIN_HPP */
