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
#include "SmartPtr.hpp"

namespace np::mem
{
	template <typename T, typename A = PoolAllocator<T>>
	class ObjectPool
	{
	private:
		NP_ENGINE_STATIC_ASSERT((::std::is_base_of_v<PoolAllocator<T>, A> || ::std::is_base_of_v<LockingPoolAllocator<T>, A>),
								"our given allocator must be our PoolAllocator or LockingPoolAllocator");

	protected:
		A _allocator;

	public:
		ObjectPool(Block block): _allocator(block) {}

		ObjectPool(siz object_count): _allocator(object_count * A::CHUNK_SIZE) {}

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

		const A& GetAllocator() const
		{
			return _allocator;
		}

		bl Contains(sptr<T> object) const
		{
			return _allocator.Contains(object.get());
		}

		template <typename... Args>
		sptr<T> CreateObject(Args&&... args)
		{
			return CreateSptr<T>(_allocator, ::std::forward<Args>(args)...);
		}
	};

	template <typename T>
	using LockingObjectPool = ObjectPool<T, LockingPoolAllocator<T>>;
} // namespace np::mem

#endif /* NP_ENGINE_OBJECT_POOL_HPP */
