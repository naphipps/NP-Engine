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
	template <typename T>
	using ObjectPoolChunkType = smart_ptr_contiguous_block<T, smart_ptr_resource<T, smart_ptr_contiguous_destroyer<T>>>;

	template <typename T, typename A = PoolAllocator<ObjectPoolChunkType<T>>>
	class ObjectPool
	{
	public:
		using ChunkType = ObjectPoolChunkType<T>;

	private:
		using DefaultBase = PoolAllocator<ChunkType>;
		using DefaultLockingBase = LockingPoolAllocator<ChunkType>;

		NP_ENGINE_STATIC_ASSERT((::std::is_base_of_v<DefaultBase, A> || ::std::is_base_of_v<DefaultLockingBase, A>),
								"our given allocator must be our PoolAllocator or LockingPoolAllocator");

	protected:
		A _allocator;

	public:
		ObjectPool(Block block): _allocator(block) {}

		ObjectPool(siz object_count): _allocator(object_count * A::CHUNK_SIZE) {}

		siz GetObjectCount() const
		{
			return _allocator.GetChunkCount();
		}

		siz GetObjectSize() const
		{
			return sizeof(T);
		}

		siz GetChunkSize() const
		{
			return _allocator.GetChunkSize();
		}

		A& GetAllocator()
		{
			return _allocator;
		}

		const A& GetAllocator() const
		{
			return _allocator;
		}

		bl Contains(sptr<T> object)
		{
			return _allocator.Contains(AddressOf(*object));
		}

		template <typename... Args>
		sptr<T> CreateObject(Args&&... args)
		{
			return mem::create_sptr<T>(_allocator, ::std::forward<Args>(args)...);
		}
	};

	template <typename T>
	using LockingObjectPool = ObjectPool<T, LockingPoolAllocator<ObjectPoolChunkType<T>>>;
} // namespace np::mem

#endif /* NP_ENGINE_OBJECT_POOL_HPP */
