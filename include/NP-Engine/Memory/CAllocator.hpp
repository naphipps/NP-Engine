//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/12/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_DEFAULT_ALLOCATOR_HPP
#define NP_ENGINE_DEFAULT_ALLOCATOR_HPP

#include <cstdlib> //aligned_alloc and free

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Insight/Insight.hpp"

#include "Allocator.hpp"
#include "Block.hpp"

namespace np::memory
{
	class CAllocator : public Allocator
	{
	public:
		virtual bl Contains(const Block& block) const override
		{
			return true;
		}

		virtual bl Contains(const void* ptr) const override
		{
			return true;
		}

		virtual Block Allocate(siz size) override
		{
			NP_ASSERT(size > 0, "given size must be greater than 0");

			size = CalcAlignedSize(size);
			Block block;
			void* ptr = nullptr;

#if defined(__clang__) || defined(_MSVC_LANG)
			ptr = ::std::malloc(size);
#else
			ptr = ::aligned_alloc(ALIGNMENT, size);
#endif

			if (ptr != nullptr)
			{
				block.ptr = ptr;
				block.size = size;
			}
			return block;
		}

		virtual bl Deallocate(Block& block) override
		{
			CAllocator::Deallocate(block.ptr);
			block.Invalidate();
			return true;
		}

		virtual bl Deallocate(void* ptr) override
		{
			::free(ptr);
			return true;
		}
	};

	extern CAllocator DefaultAllocator;
} // namespace np::memory

#endif /* NP_ENGINE_DEFAULT_ALLOCATOR_HPP */
