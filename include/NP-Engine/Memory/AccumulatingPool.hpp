//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/4/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MEM_ACCUMULATING_POOL_HPP
#define NP_ENGINE_MEM_ACCUMULATING_POOL_HPP

#include <vector>

#include "Allocator.hpp"
#include "ObjectPool.hpp"

namespace np::mem
{
	template <typename T, siz ALIGNMENT, typename ALLOCATOR_TYPE = pool_allocator<object_pool_chunk_type<T>, ALIGNMENT>>
	class accumulating_pool
	{
	protected:
		NP_ENGINE_STATIC_ASSERT((::std::is_base_of_v<pool_allocator_interface<object_pool_chunk_type<T>, ALIGNMENT>, ALLOCATOR_TYPE>),
			"our given allocator must inherit from pool_allocator_interface<object_pool_chunk_type<T>, ALIGNMENT>");

		using pool_type = object_pool<T, ALIGNMENT, ALLOCATOR_TYPE>;
		constexpr static siz POOL_ALIGNMENT = ALIGNMENT;
		constexpr static siz POOL_TYPE_SIZE = calc_aligned_size(sizeof(pool_type), POOL_ALIGNMENT);
		mutexed_wrapper<::std::vector<pool_type*>> _pools;

		static void add_pool(::std::vector<pool_type*>& pools)
		{
			c_allocator _c_allocator{};
			const siz object_count = BIT(pools.size() + 2); //start with 4, then times 2 from there
			const siz chunk_size = ALLOCATOR_TYPE::CHUNK_SIZE;
			const siz size = POOL_TYPE_SIZE + (chunk_size * object_count);
			const block contiguous_block = _c_allocator.allocate(size, POOL_ALIGNMENT);
			const block object_block{ contiguous_block.ptr, POOL_TYPE_SIZE};
			const block allocate_block{object_block.end(), contiguous_block.size - object_block.size};
			pools.emplace_back(mem::construct<pool_type>(object_block, allocate_block));
		}

	public:
		virtual siz get_object_size() const
		{
			return sizeof(T);
		}

		virtual siz get_chunk_size() const
		{
			return ALLOCATOR_TYPE::CHUNK_SIZE;
		}

		virtual siz get_chunk_count()
		{
			siz capacity = 0;
			auto pools = _pools.get_access();
			for (pool_type* pool : *pools)
				capacity += pool->get_chunk_count();
			return capacity;
		}

		virtual void reserve(siz count)
		{
			siz capacity = 0;
			auto pools = _pools.get_access();
			for (pool_type* pool : *pools)
				capacity += pool->get_chunk_count();

			while (count > capacity)
			{
				add_pool(*pools);
				capacity += pools->back()->get_chunk_count();
			}
		}

		virtual bl contains(sptr<T>& object)
		{
			bl contains = false;
			auto pools = _pools.get_access();
			for (siz i = 0; i < pools->size() && !contains; i++)
				contains = (*pools)[i]->contains(object);
			return contains;
		}

		template <typename... Args,
			::std::enable_if_t<is_parenthesis_constructible_v<T, Args...> || is_list_constructible_v<T, Args...>, bl> = true>
		sptr<T> create_object(Args&&... args)
		{
			sptr<T> object = nullptr;
			auto pools = _pools.get_access();
			for (auto it = pools->rbegin(); !object && it != pools->rend(); it++)
				object = (*it)->create_object(::std::forward<Args>(args)...);

			if (!object)
			{
				add_pool(*pools);
				object = pools->back()->create_object(::std::forward<Args>(args)...);
			}

			return object;
		}

		virtual void clear()
		{
			c_allocator _c_allocator{};
			auto pools = _pools.get_access();
			for (pool_type* pool : *pools)
				mem::destroy<pool_type>(_c_allocator, pool);
			pools->clear();
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_MEM_ACCUMULATING_POOL_HPP */