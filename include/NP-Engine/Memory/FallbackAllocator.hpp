//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MEM_FALLBACK_ALLOCATOR_HHPP
#define NP_ENGINE_MEM_FALLBACK_ALLOCATOR_HHPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "Allocator.hpp"
#include "MemoryFunctions.hpp"

namespace np::mem
{
	class fallback_allocator : public allocator
	{
	protected:
		allocator* _primary;
		allocator* _fallback;

	public:
		fallback_allocator(allocator& primary, allocator& fallback):
			_primary(address_of(primary)),
			_fallback(address_of(fallback))
		{}

		virtual ~fallback_allocator() = default;

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
			block b = _primary->allocate(size, alignment);
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
			this value is meaningless from fallback_allocator
		*/
		virtual block reallocate(void* ptr, siz size, siz alignment) override
		{
			return {};
		}

		virtual bl deallocate(block& b) override
		{
			return _primary->contains(b) ? _primary->deallocate(b) : _fallback->deallocate(b);
		}

		virtual bl deallocate(void* ptr) override
		{
			return _primary->contains(ptr) ? _primary->deallocate(ptr) : _fallback->deallocate(ptr);
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_MEM_FALLBACK_ALLOCATOR_HHPP */
