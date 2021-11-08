//
//  ObjectPool.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 2/17/21.
//

#ifndef NP_ENGINE_OBJECT_POOL_HPP
#define NP_ENGINE_OBJECT_POOL_HPP

#include <type_traits>

#include "NP-Engine/Insight/Insight.hpp"

#include "PoolAllocator.hpp"
#include "LockingPoolAllocator.hpp"
namespace np
{
    namespace memory
    {
        template <typename T, typename A = PoolAllocator<T>>
        class ObjectPool
        {
        private:
            NP_STATIC_ASSERT((
                ::std::is_base_of_v<PoolAllocator<T>, A> ||
                ::std::is_base_of_v<LockingPoolAllocator<T>, A>),
                "our given allocator must be our PoolAllocator or LockingPoolAllocator");
            
        public:
            using ObjectType = T;
            using ObjectTypePtr = T*;
            
        protected:
            A _allocator;
            
        public:
            
            /**
             constructor
             */
            ObjectPool(Block block):
            _allocator(block)
            {}
            
            /**
             constructor
             */
            ObjectPool(siz object_count):
            _allocator(object_count * A::CHUNK_ALIGNED_SIZE)
            {}
            
            /**
             deconstructor
             */
            virtual ~ObjectPool() = default;
            
            /**
             gets the object count
             */
            siz ObjectCount() const
            {
                return _allocator.ChunkCount();
            }
            
            /**
             get the object size
             */
            siz ObjectSize() const
            {
                return sizeof(T);
            }
            
            /**
             gets the chunk size
             */
            siz ChunkSize() const
            {
                return _allocator.ChunkSize();
            }
            
            /**
             returns our allocator for delicate custom use
             */
            A& GetAllocator()
            {
                return _allocator;
            }
            
            /**
             indicates if we contain the given object's pointer
             */
            bl Contains(ObjectTypePtr object)
            {
                return _allocator.Contains(object);
            }
            
            /**
             allocate and create an object
             */
            template <typename ... Args>
            ObjectTypePtr CreateObject(Args&& ... args)
            {
                Block block = _allocator.Allocate(ObjectSize());
                Construct<T>(block, utility::Forward<Args>(args)...);
                return static_cast<ObjectTypePtr>(block.ptr);
            }
            
            /**
             destroys the given object that was created from this pool
             returns true if the object came from this pool and successfully destructed and deallocated
             */
            virtual bl DestroyObject(ObjectTypePtr object)
            {
                bl destroyed = false;
                if (_allocator.Contains(object) && Destruct<T>(object))
                {
                    destroyed = _allocator.Deallocate(object);
                }
                return destroyed;
            }
            
            /**
             clears the pool - reseting it back to it was on construction
             */
            virtual void Clear()
            {
                _allocator.Zeroize();
            }
        };
        
        template <typename T>
        using LockingObjectPool = ObjectPool<T, LockingPoolAllocator<T>>;
    }
}

#endif /* NP_ENGINE_OBJECT_POOL_HPP */
