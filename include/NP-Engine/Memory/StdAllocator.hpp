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

		using value_type = ::std::allocator<T>::value_type;
		using pointer = ::std::allocator<T>::pointer;
		using const_pointer = ::std::allocator<T>::const_pointer;
		using reference = ::std::allocator<T>::reference;
		using const_reference = ::std::allocator<T>::const_reference;
		using size_type = ::std::allocator<T>::size_type;
		using difference_type = ::std::allocator<T>::difference_type;

		template< class U > 
		struct rebind
		{
			typedef StdAllocator<U> other;
		};
		
		inline explicit StdAllocator()
		{
			//empty on purpose
		}

		inline StdAllocator(const StdAllocator& other)
		{
			//empty on purpose
		}
		
		template<typename U>
		inline explicit StdAllocator(const StdAllocator<U>& other)
		{
			//empty on purpose
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
}

#endif /* NP_ENGINE_STD_ALLOCATOR_HPP */