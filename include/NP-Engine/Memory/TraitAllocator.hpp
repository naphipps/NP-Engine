//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 7/20/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MEM_TRAIT_ALLOCATOR_HPP
#define NP_ENGINE_MEM_TRAIT_ALLOCATOR_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "Allocator.hpp"
#include "CAllocator.hpp"
#include "MemoryFunctions.hpp"

namespace np::mem
{
	class trait_allocator : public allocator
	{
	private:
		static c_allocator _default_allocator;
		static atm<allocator*> _registered_allocator;

		static void ensure_registration()
		{
			allocator* expected = nullptr;
			_registered_allocator.compare_exchange_strong(expected, address_of(_default_allocator), mo_release, mo_relaxed);
			NP_ENGINE_ASSERT(expected, "trait_allocator's registration is nullptr when it should not be");
		}

	public:
		virtual ~trait_allocator() = default;

		virtual bl contains(const block& b) override
		{
			return contains(b.ptr);
		}

		virtual bl contains(const void* ptr) override
		{
			ensure_registration();
			return _registered_allocator.load(mo_acquire)->contains(ptr);
		}

		virtual block allocate(siz size, siz alignment) override
		{
			ensure_registration();
			return _registered_allocator.load(mo_acquire)->allocate(size, alignment);
		}

		virtual block reallocate(block& b, siz size, siz alignment) override
		{
			ensure_registration();
			return _registered_allocator.load(mo_acquire)->reallocate(b, size, alignment);
		}

		virtual block reallocate(void* ptr, siz size, siz alignment) override
		{
			ensure_registration();
			return _registered_allocator.load(mo_acquire)->reallocate(ptr, size, alignment);
		}

		virtual bl deallocate(block& b) override
		{
			return deallocate(b.ptr);
		}

		virtual bl deallocate(void* ptr) override
		{
			ensure_registration();
			return _registered_allocator.load(mo_acquire)->deallocate(ptr);
		}

		static inline void* realloc(void* ptr, siz size, siz alignment)
		{
			ensure_registration();
			return _registered_allocator.load(mo_acquire)->reallocate(ptr, size, alignment).ptr;
		}

		static inline void* malloc(siz size, siz alignment)
		{
			ensure_registration();
			return _registered_allocator.load(mo_acquire)->allocate(size, alignment).ptr;
		}

		static inline void free(void* ptr)
		{
			ensure_registration();
			_registered_allocator.load(mo_acquire)->deallocate(ptr);
		}

		static inline void register_allocator(allocator& a)
		{
			_registered_allocator.store(address_of(a), mo_release);
		}

		static inline void reset_registration()
		{
			register_allocator(_default_allocator);
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_MEM_TRAIT_ALLOCATOR_HPP */
