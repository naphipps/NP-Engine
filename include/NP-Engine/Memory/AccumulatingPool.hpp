//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/4/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_ACCUMULATING_POOL_HPP
#define NP_ENGINE_ACCUMULATING_POOL_HPP

#ifndef NP_ENGINE_ACCUMULATING_POOL_OBJECT_COUNT
	#define NP_ENGINE_ACCUMULATING_POOL_OBJECT_COUNT 100
#endif

#include <vector>

#include "Allocator.hpp"
#include "ObjectPool.hpp"

// TODO: AccumulatingPool has not been tested
// TODO: add AccumulatingPool.Reserve(count) to reserve "count" objects in advance

namespace np::mem
{
	template <typename T, typename A = PoolAllocator<typename ObjectPool<T>::ChunkType>>
	class AccumulatingPool
	{
	private:
		using PoolType = ObjectPool<T, A>;

		constexpr static siz POOL_TYPE_SIZE = CalcAlignedSize(sizeof(PoolType));

		CAllocator _c_allocator;
		::std::vector<PoolType*> _pools;

	public:
		siz ObjectSize() const
		{
			return sizeof(T);
		}

		siz ChunkSize() const
		{
			return A::CHUNK_SIZE;
		}

		bl Contains(sptr<T>& object) const
		{
			bl contains = false;
			for (siz i = 0; i < _pools.size() && !contains; i++)
				contains = _pools[i]->Contains(object);

			return contains;
		}

		template <typename... Args>
		sptr<T> CreateObject(Args&&... args)
		{
			sptr<T> object = nullptr;

			for (siz i = 0; i < _pools.size() && !object; i++)
				object = _pools[i]->CreateObject(::std::forward<Args>(args)...);

			if (!object)
			{
				//TODO: can we simplify a pool's construction?? I don't think so
				//TODO: put this "AddPool" behavior in a method
				siz size = POOL_TYPE_SIZE + (A::CHUNK_SIZE * NP_ENGINE_ACCUMULATING_POOL_OBJECT_COUNT);
				//TODO: size should be POOL_TYPE_SIZE + <twice the size of the last pool OR CHUNK_SIZE * POOL_OBJECT_COUNT>
				Block block = _c_allocator.Allocate(size);
				Block pool_block{block.ptr, POOL_TYPE_SIZE};
				Block pool_given_block{pool_block.End(), block.size - pool_block.size};
				_pools.emplace_back(mem::Construct<PoolType>(pool_block, pool_given_block));

				object = _pools.back()->CreateObject(::std::forward<Args>(args)...);
			}

			return object;
		}

		void Clear()
		{
			for (PoolType* pool : _pools)
				mem::Destroy<PoolType>(_c_allocator, pool);

			_pools.clear();
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_ACCUMULATING_POOL_HPP */