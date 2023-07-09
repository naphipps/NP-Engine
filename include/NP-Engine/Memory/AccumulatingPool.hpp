//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/4/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_ACCUMULATING_POOL_HPP
#define NP_ENGINE_ACCUMULATING_POOL_HPP

#include <vector>

#include "Allocator.hpp"
#include "ObjectPool.hpp"

namespace np::mem
{
	template <typename T, typename A = PoolAllocator<typename ObjectPool<T>::ChunkType>>
	class AccumulatingPool
	{
	private:
		using PoolType = ObjectPool<T, A>;
		constexpr static siz POOL_TYPE_SIZE = CalcAlignedSize(sizeof(PoolType));
		mutexed_wrapper<::std::vector<PoolType*>> _pools;

		static void AddPool(::std::vector<PoolType*>& pools)
		{
			CAllocator _c_allocator;
			const siz object_count = pools.empty() ? 4 : pools.back()->GetObjectCount() * 2;
			const siz chunk_size = A::CHUNK_SIZE;
			const siz size = POOL_TYPE_SIZE + (chunk_size * object_count);
			const Block block = _c_allocator.Allocate(size);
			const Block object_block{block.ptr, POOL_TYPE_SIZE};
			const Block allocate_block{object_block.End(), block.size - object_block.size};
			pools.emplace_back(mem::Construct<PoolType>(object_block, allocate_block));
		}

	public:
		siz GetObjectSize() const
		{
			return sizeof(T);
		}

		siz GetChunkSize() const
		{
			return A::CHUNK_SIZE;
		}

		siz GetObjectCount()
		{
			siz capacity = 0;
			auto pools = _pools.get_access();
			for (PoolType* pool : *pools)
				capacity += pool->GetObjectCount();
			return capacity;
		}

		void Reserve(siz count)
		{
			siz capacity = 0;
			auto pools = _pools.get_access();
			for (PoolType* pool : *pools)
				capacity += pool->GetObjectCount();

			while (count > capacity)
			{
				AddPool(*pools);
				capacity += pools->back()->GetObjectCount();
			}
		}

		bl Contains(sptr<T>& object)
		{
			bl contains = false;
			auto pools = _pools.get_access();
			for (siz i = 0; i < pools->size() && !contains; i++)
				contains = (*pools)[i]->Contains(object);
			return contains;
		}

		template <typename... Args>
		sptr<T> CreateObject(Args&&... args)
		{
			sptr<T> object = nullptr;
			auto pools = _pools.get_access();
			for (auto it = pools->rbegin(); !object && it != pools->rend(); it++)
				object = (*it)->CreateObject(::std::forward<Args>(args)...);

			if (!object)
			{
				AddPool(*pools);
				object = pools->back()->CreateObject(::std::forward<Args>(args)...);
			}

			return object;
		}

		void Clear()
		{
			CAllocator _c_allocator;
			auto pools = _pools.get_access();
			for (PoolType* pool : *pools)
				mem::Destroy<PoolType>(_c_allocator, pool);
			pools->clear();
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_ACCUMULATING_POOL_HPP */