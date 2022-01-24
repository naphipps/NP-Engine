//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/12/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_BLOCK_HPP
#define NP_ENGINE_BLOCK_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::memory
{
	/*
		our Block design was inspired by Andrei Alexandrescu's cppcon 2017 talk "std::allocator is to allocation what
		std::vector is to vexation" <https://www.youtube.com/watch?v=LIb3L4vKZ7U>
	*/
	struct Block
	{
		void* ptr = nullptr;
		siz size = 0;

		void Invalidate()
		{
			ptr = nullptr;
			size = 0;
		}

		bl IsValid() const
		{
			return ptr != nullptr && size > 0;
		}

		void* Begin() const
		{
			return IsValid() ? ptr : nullptr;
		}

		void* End() const
		{
			return IsValid() ? static_cast<ui8*>(ptr) + size : nullptr;
		}

		bl Contains(const void* p) const
		{
			return IsValid() ? ptr <= p && p < End() : false;
		}

		bl Contains(const Block& block) const
		{
			return IsValid() && block.IsValid() ? Contains(block.Begin()) && Contains(static_cast<ui8*>(block.End()) - 1)
												: false;
		}

		void Zeroize()
		{
			for (siz i = 0; i < size; i++)
			{
				static_cast<ui8*>(ptr)[i] = 0;
			}
		}
	};
} // namespace np::memory

#endif /* NP_ENGINE_BLOCK_HPP */
