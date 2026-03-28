//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/2/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MEM_MARGIN_HPP
#define NP_ENGINE_MEM_MARGIN_HPP

#ifndef NP_ENGINE_MEM_ATTEMPT_MARGIN_EXTRACTION_COUNT
	#define NP_ENGINE_MEM_ATTEMPT_MARGIN_EXTRACTION_COUNT 5 //this is essentially treated as a maximum multiple of DEFAULT_ALIGNMENT
#endif

#include "NP-Engine/Primitive/Primitive.hpp"

#include "Alignment.hpp"

namespace np::mem::__detail
{
	class margin : public enm_siz
	{
	public:
		constexpr static siz allocated = BIT(0);
		
		margin(siz size, bl is_allocated) : enm_siz(zero)
		{
			set_size(size);
			set_is_allocated(is_allocated);
		}

		siz get_size() const
		{
			return _value & ~allocated;
		}

		/*
			this will truncate bit 0 if we are not allocated
		*/
		void set_size(siz size)
		{
			_value = is_allocated() ? size | allocated : size & ~allocated;
		}

		bl is_allocated() const
		{
			return contains(allocated);
		}

		void set_is_allocated(bl is_allocated)
		{
			_value = is_allocated ? _value | allocated : _value & ~allocated;
		}
	};

	NP_ENGINE_STATIC_ASSERT(sizeof(margin) == DEFAULT_ALIGNMENT, "size of margin must equal alignment -- we take advantage of this");
} // namespace np::mem::__detail

#endif /* NP_ENGINE_MEM_MARGIN_HPP */
