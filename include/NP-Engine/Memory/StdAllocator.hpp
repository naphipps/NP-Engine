//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/15/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MEM_STD_ALLOCATOR_HPP
#define NP_ENGINE_MEM_STD_ALLOCATOR_HPP

#include <memory>

#include "TraitAllocator.hpp"

namespace np::mem
{
	template <class T>
	class std_allocator : public ::std::allocator<T>
	{
	protected:
		using base = ::std::allocator<T>;

		trait_allocator _allocator;

	public:
		using value_type = typename base::value_type;
		using size_type = typename base::size_type;
		using difference_type = typename base::difference_type;

		template <class U>
		struct rebind
		{
			using other = std_allocator<U>;
		};

		inline explicit std_allocator()
		{
			// empty on purpose
		}

		inline std_allocator(const std_allocator& other)
		{
			// empty on purpose
		}

		template <typename U>
		inline explicit std_allocator(const std_allocator<U>& other)
		{
			// empty on purpose
		}

		virtual ~std_allocator() = default;

		inline value_type* allocate(size_type size)
		{
			return static_cast<value_type*>(_allocator.allocate(size * sizeof(value_type), mem::DEFAULT_ALIGNMENT).ptr);
		}

		inline void deallocate(value_type* ptr, size_type size)
		{
			_allocator.deallocate(ptr);
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_MEM_STD_ALLOCATOR_HPP */