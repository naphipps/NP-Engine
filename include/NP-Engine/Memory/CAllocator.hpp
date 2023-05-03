//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/12/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_DEFAULT_ALLOCATOR_HPP
#define NP_ENGINE_DEFAULT_ALLOCATOR_HPP

#include <cstdlib> //aligned_alloc and free

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "Allocator.hpp"
#include "Block.hpp"

namespace np::mem
{
	class CAllocator : public Allocator
	{
	public:
		virtual bl Contains(const Block& block) override
		{
			return true;
		}

		virtual bl Contains(const void* ptr) override
		{
			return true;
		}

		virtual Block Allocate(siz size) override
		{
			NP_ENGINE_ASSERT(size > 0, "given size must be greater than 0");

			size = CalcAlignedSize(size);
			Block block;
			void* ptr = nullptr;

#if defined(__clang__) || defined(_MSVC_LANG)
			ptr = ::std::malloc(size);
#else
			ptr = ::aligned_alloc(ALIGNMENT, size);
#endif

			if (ptr)
				block = {ptr, size};
			return block;
		}

		virtual Block Reallocate(Block& old_block, siz new_size) override
		{
			Block new_block = Reallocate(old_block.ptr, new_size);
			old_block.Invalidate();
			return new_block;
		}

		virtual Block Reallocate(void* old_ptr, siz new_size) override
		{
			Block new_block;
			if (old_ptr)
			{
				new_size = CalcAlignedSize(new_size);
				void* ptr = ::std::realloc(old_ptr, new_size);

				if (ptr)
					new_block = {ptr, new_size};
			}
			else
			{
				new_block = Allocate(new_size);
			}

			return new_block;
		}

		virtual bl Deallocate(Block& block) override
		{
			CAllocator::Deallocate(block.ptr);
			block.Invalidate();
			return true;
		}

		virtual bl Deallocate(void* ptr) override
		{
			::std::free(ptr);
			return true;
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_DEFAULT_ALLOCATOR_HPP */
