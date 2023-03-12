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

//TODO: AccumulatingPool has not been tested

namespace np::mem
{
	template <typename T, typename A = PoolAllocator<T>>
	class AccumulatingPool
	{
	private:
		using PoolType = ObjectPool<T, A>;

		constexpr static siz POOL_TYPE_SIZE = CalcAlignedSize(sizeof(PoolType));

		CAllocator _c_allocator;
		::std::vector<PoolType*> _pools;

	public:
		using ObjectType = typename PoolType::ObjectType;
		using ObjectTypePtr = typename PoolType::ObjectTypePtr;

		siz ObjectSize() const
		{
			return sizeof(T);
		}

		siz ChunkSize() const
		{
			return A::CHUNK_SIZE;
		}

		bl Contains(ObjectTypePtr object) const
		{
			bl contains = false;
			for (siz i = 0; i < _pools.size() && !contains; i++)
				contains = _pools[i]->Contains(object);

			return contains;
		}

		template <typename... Args>
		ObjectTypePtr CreateObject(Args&&... args)
		{
			ObjectTypePtr object = nullptr;

			for (siz i = 0; i < _pools.size() && !object; i++) 
				object = _pools[i]->CreateObject(::std::forward<Args>(args)...);

			if (!object)
			{
				siz size = POOL_TYPE_SIZE + (A::CHUNK_SIZE * NP_ENGINE_ACCUMULATING_POOL_OBJECT_COUNT);
				Block block = _c_allocator.Allocate(size);
				Block pool_block{ block.ptr, POOL_TYPE_SIZE };
				Block pool_given_block{ pool_block.End(), block.size - pool_block.size };
				Construct<PoolType>(pool_block, pool_given_block);
				_pools.emplace_back((PoolType*)pool_block.ptr);

				object = _pools.back()->CreateObject(::std::forward<Args>(args)...);
			}

			return object;
		}

		bl DestroyObject(ObjectTypePtr object)
		{
			bl destroyed = false;
			for (siz i=0; i<_pools.size() && !destroyed; i++)
				if (_pools[i]->Contains(object))
					destroyed = _pools[i]->DestroyObject(object);

			return destroyed;
		}

		void Clear()
		{
			for (PoolType* pool : _pools)
				Destroy<PoolType>(_c_allocator, pool);

			_pools.clear();
		}
	};
}

#endif /* NP_ENGINE_ACCUMULATING_POOL_HPP */