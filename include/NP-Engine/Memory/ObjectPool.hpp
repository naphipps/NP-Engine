//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/17/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_OBJECT_POOL_HPP
#define NP_ENGINE_OBJECT_POOL_HPP

#include <type_traits>
#include <utility>

#include "NP-Engine/Foundation/Foundation.hpp"

#include "PoolAllocator.hpp"
#include "LockingPoolAllocator.hpp"

namespace np::mem
{
	template <typename T, typename A = PoolAllocator<T>>
	class ObjectPool
	{
	private:
		NP_ENGINE_STATIC_ASSERT((::std::is_base_of_v<PoolAllocator<T>, A> || ::std::is_base_of_v<LockingPoolAllocator<T>, A>),
								"our given allocator must be our PoolAllocator or LockingPoolAllocator");

	public:
		using ObjectType = T;
		using ObjectTypePtr = T*;

	protected:
		A _allocator;

	public:
		ObjectPool(Block block): _allocator(block) {}

		ObjectPool(siz object_count): _allocator(object_count * A::CHUNK_ALIGNED_SIZE) {}

		siz ObjectCount() const
		{
			return _allocator.ChunkCount();
		}

		siz ObjectSize() const
		{
			return sizeof(T);
		}

		siz ChunkSize() const
		{
			return _allocator.ChunkSize();
		}

		A& GetAllocator()
		{
			return _allocator;
		}

		bl Contains(ObjectTypePtr object)
		{
			return _allocator.Contains(object);
		}

		template <typename... Args>
		ObjectTypePtr CreateObject(Args&&... args)
		{
			Block block = _allocator.Allocate(ObjectSize());
			Construct<T>(block, ::std::forward<Args>(args)...);
			return static_cast<ObjectTypePtr>(block.ptr);
		}

		virtual bl DestroyObject(ObjectTypePtr object)
		{
			bl destroyed = false;
			if (_allocator.Contains(object) && Destruct<T>(object))
			{
				destroyed = _allocator.Deallocate(object);
			}
			return destroyed;
		}

		virtual void Clear()
		{
			_allocator.Zeroize();
		}
	};

	template <typename T>
	using LockingObjectPool = ObjectPool<T, LockingPoolAllocator<T>>;
} // namespace np::mem

#endif /* NP_ENGINE_OBJECT_POOL_HPP */
