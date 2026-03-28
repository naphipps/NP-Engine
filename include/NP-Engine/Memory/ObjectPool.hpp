//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/17/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MEM_OBJECT_POOL_HPP
#define NP_ENGINE_MEM_OBJECT_POOL_HPP

#include <type_traits>
#include <utility>

#include "NP-Engine/Foundation/Foundation.hpp"

#include "PoolAllocator.hpp"
#include "SmartPtr.hpp"

namespace np::mem
{
	template <typename T>
	using object_pool_chunk_type = smart_ptr_contiguous_block<T, smart_ptr_resource<T, smart_ptr_contiguous_destroyer<T>>>;

	template <typename T, siz ALIGNMENT, typename POOL_ALLOCATOR_TYPE = pool_allocator<object_pool_chunk_type<T>, ALIGNMENT>>
	class object_pool
	{
	public:
		using chunk_type = object_pool_chunk_type<T>;

	protected:
		NP_ENGINE_STATIC_ASSERT((::std::is_base_of_v<pool_allocator_interface<object_pool_chunk_type<T>, ALIGNMENT>, POOL_ALLOCATOR_TYPE>),
								"our given allocator must inherit from pool_allocator_interface<object_pool_chunk_type<T>, ALIGNMENT>");

		POOL_ALLOCATOR_TYPE _allocator;

	public:
		object_pool(block b): _allocator(b) {}

		object_pool(siz object_count, siz alignment): _allocator(object_count * POOL_ALLOCATOR_TYPE::CHUNK_SIZE, alignment) {}

		virtual ~object_pool() = default;

		virtual siz get_chunk_count() const
		{
			return _allocator.get_chunk_count();
		}

		virtual siz get_object_size() const
		{
			return sizeof(T);
		}

		virtual siz get_chunk_size() const
		{
			return _allocator.get_chunk_size();
		}

		virtual POOL_ALLOCATOR_TYPE& get_allocator()
		{
			return _allocator;
		}

		virtual const POOL_ALLOCATOR_TYPE& get_allocator() const
		{
			return _allocator;
		}

		virtual bl contains(sptr<T> object)
		{
			return _allocator.contains(address_of(*object));
		}

		template <typename... Args,
			::std::enable_if_t<is_parenthesis_constructible_v<T, Args...> || is_list_constructible_v<T, Args...>, bl> = true>
		sptr<T> create_object(Args&&... args)
		{
			return mem::create_sptr<T>(_allocator, ::std::forward<Args>(args)...);
		}
	};

	template <typename T, siz ALIGNMENT>
	using locking_object_pool = object_pool<T, ALIGNMENT, locking_pool_allocator<object_pool_chunk_type<T>, ALIGNMENT>>;
} // namespace np::mem

#endif /* NP_ENGINE_MEM_OBJECT_POOL_HPP */
