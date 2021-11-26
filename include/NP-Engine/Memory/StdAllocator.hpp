//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/15/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_STD_ALLOCATOR_HPP
#define NP_ENGINE_STD_ALLOCATOR_HPP

#include <memory>

#include "TraitAllocator.hpp"

namespace np::memory
{
	template <class T>
	class StdAllocator : public ::std::allocator<T>
	{
	protected:
		TraitAllocator _allocator;

	public:
		using value_type = typename ::std::allocator<T>::value_type;
		using pointer = typename ::std::allocator<T>::pointer;
		using const_pointer = typename ::std::allocator<T>::const_pointer;
		using reference = typename ::std::allocator<T>::reference;
		using const_reference = typename ::std::allocator<T>::const_reference;
		using size_type = typename ::std::allocator<T>::size_type;
		using difference_type = typename ::std::allocator<T>::difference_type;

		template <class U>
		struct rebind
		{
			typedef StdAllocator<U> other;
		};

		inline explicit StdAllocator()
		{
			// empty on purpose
		}

		inline StdAllocator(const StdAllocator& other)
		{
			// empty on purpose
		}

		template <typename U>
		inline explicit StdAllocator(const StdAllocator<U>& other)
		{
			// empty on purpose
		}

		inline pointer allocate(size_type size)
		{
			return (pointer)_allocator.Allocate(size * sizeof(T)).Begin();
		}

		inline void deallocate(pointer ptr, size_type size)
		{
			_allocator.Deallocate(ptr);
		}
	};
} // namespace np::memory

#endif /* NP_ENGINE_STD_ALLOCATOR_HPP */