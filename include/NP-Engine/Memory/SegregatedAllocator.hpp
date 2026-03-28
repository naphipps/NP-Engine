//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/1/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MEM_SEGEREGATED_ALLOCATOR_HPP
#define NP_ENGINE_MEM_SEGEREGATED_ALLOCATOR_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "Allocator.hpp"
#include "MemoryFunctions.hpp"

namespace np::mem
{
	class segregated_allocator : public allocator
	{
	private:
		allocator* _primary;
		allocator* _fallback;
		siz _size_threshold;

	public:
		segregated_allocator(allocator& primary, allocator& fallback, siz size_threshold):
			_primary(mem::address_of(primary)),
			_fallback(mem::address_of(fallback)),
			_size_threshold(size_threshold)
		{
			NP_ENGINE_ASSERT(_size_threshold > 0, "we must have a nonzero size threshold");
		}

		virtual siz get_size_threshold() const
		{
			return _size_threshold;
		}

		virtual allocator& get_primary()
		{
			return *_primary;
		}

		virtual const allocator& get_primary() const
		{
			return *_primary;
		}

		virtual allocator& get_fallback()
		{
			return *_fallback;
		}

		virtual const allocator& get_fallback() const
		{
			return *_fallback;
		}

		virtual bl contains(const block& b) override
		{
			return _primary->contains(b) || _fallback->contains(b);
		}

		virtual bl contains(const void* ptr) override
		{
			return _primary->contains(ptr) || _fallback->contains(ptr);
		}

		virtual block allocate(siz size, siz alignment) override
		{
			block b = calc_aligned_size(size, alignment) <= _size_threshold ? _primary->allocate(size, alignment) : block{};
			return b.is_valid() ? b : _fallback->allocate(size, alignment);
		}

		virtual block reallocate(block& b_, siz size, siz alignment) override
		{
			block b = allocate(size, alignment);
			if (contains(b_))
			{
				const siz byte_count = b.size < b_.size ? b.size : b_.size;
				copy_bytes(b.begin(), b_.begin(), byte_count);
				deallocate(b_);
				b_.invalidate();
			}
			return b;
		}

		/*
			this value is meaningless from segregated_allocator
		*/
		virtual block reallocate(void* ptr, siz size, siz alignment) override
		{
			return {};
		}

		virtual bl deallocate(block& b) override
		{
			return _primary->contains(b) ? _primary->contains(b) : _fallback->deallocate(b);
		}

		virtual bl deallocate(void* ptr) override
		{
			return _primary->contains(ptr) ? _primary->contains(ptr) : _fallback->deallocate(ptr);
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_MEM_SEGEREGATED_ALLOCATOR_HPP */
