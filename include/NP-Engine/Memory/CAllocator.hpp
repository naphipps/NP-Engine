//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/12/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MEM_C_ALLOCATOR_HPP
#define NP_ENGINE_MEM_C_ALLOCATOR_HPP

#include <cstdlib>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "Allocator.hpp"
#include "Block.hpp"

namespace np::mem
{
	class c_allocator : public allocator
	{
	public:
		
		virtual ~c_allocator() = default;

		/*
			this value is meaningless from c_allocator
		*/
		virtual bl contains(const block& b) override
		{
			return true;
		}

		/*
			this value is meaningless from c_allocator
		*/
		virtual bl contains(const void* ptr) override
		{
			return true;
		}

		virtual block allocate(siz size, siz alignment) override
		{
			NP_ENGINE_ASSERT(size > 0, "given size must be greater than 0");

			alignment = sanitize_alignment(alignment);
			size = calc_aligned_size(size, alignment);
			void* ptr = nullptr;

#if NP_ENGINE_PLATFORM_IS_WINDOWS
			ptr = ::_aligned_malloc(size, alignment);
#else
			ptr = ::std::aligned_alloc(alignment, size);
#endif

			return ptr ? block{ptr, size} : block{};
		}

		virtual block reallocate(block& b_, siz size, siz alignment) override
		{
			block b = reallocate(b_.ptr, size, alignment);
			b_.invalidate();
			return b;
		}

		virtual block reallocate(void* ptr_, siz size, siz alignment) override
		{
			block b{};
			if (ptr_)
			{
				alignment = sanitize_alignment(alignment);
				size = calc_aligned_size(size, alignment);

				void* ptr = nullptr;

#if NP_ENGINE_PLATFORM_IS_WINDOWS
				ptr = ::_aligned_realloc(ptr_, size, alignment);
#else
				ptr = allocate(size, alignment).ptr;
				copy_bytes(ptr, ptr_, size);
#endif
				if (ptr)
					b = {ptr, size};
			}
			else
			{
				b = allocate(size, alignment);
			}

			return b;
		}

		virtual bl deallocate(block& b) override
		{
			bl deallocated = deallocate(b.ptr);
			b.invalidate();
			return deallocated;
		}

		virtual bl deallocate(void* ptr) override
		{
#if NP_ENGINE_PLATFORM_IS_WINDOWS
			::_aligned_free(ptr);
#else
			::std::free(ptr);
#endif
			return true;
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_MEM_C_ALLOCATOR_HPP */
