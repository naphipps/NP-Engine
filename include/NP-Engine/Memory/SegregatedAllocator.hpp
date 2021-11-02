//
//  SegregatedAllocator.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 3/1/21.
//

#ifndef NP_ENGINE_SEGEREGATED_ALLOCATOR_HPP
#define NP_ENGINE_SEGEREGATED_ALLOCATOR_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Insight/Insight.hpp"

#include "Allocator.hpp"

namespace np
{
    namespace memory
    {
        class SegregatedAllocator : public Allocator
        {
        private:
            Allocator* _primary;
            Allocator* _fallback;
            siz _size_threshold;
            
        public:
            
            /**
             constructor
             */
            SegregatedAllocator(Allocator* primary, Allocator* fallback, siz size_threshold):
            _primary(primary),
            _fallback(fallback),
            _size_threshold(size_threshold)
            {
                NP_ASSERT(_primary != nullptr, "we must get a valid primary allocator");
                NP_ASSERT(_fallback != nullptr, "we must get a valid fallback allocator");
                NP_ASSERT(_size_threshold > 0, "we must have a nonzero size threshold");
            }
            
            /**
             deconstructor
             */
            ~SegregatedAllocator() = default;
            
            /**
             gets the size threshold we use to determine allocation behavior
             */
            virtual siz GetSizeThreshold() const
            {
                return _size_threshold;
            }
            
            /**
             gets the primary allocator
             */
            virtual Allocator& GetPrimary()
            {
                return *_primary;
            }
            
            /**
             gets the primary allocator
             */
            virtual const Allocator& GetPrimary() const
            {
                return *_primary;
            }
            
            /**
             gets the fallback allocator
             */
            virtual Allocator& GetFallback()
            {
                return *_fallback;
            }
            
            /**
             gets the fallback allocator
             */
            virtual const Allocator& GetFallback() const
            {
                return *_fallback;
            }
            
            /**
             checks if we contain given block
             */
            virtual bl Contains(const Block& block) const override
            {
                return _primary->Contains(block) || _fallback->Contains(block);
            }
            
            /**
             checks if we contain given ptr
             */
            virtual bl Contains(const void* ptr) const override
            {
                return _primary->Contains(ptr) || _fallback->Contains(ptr);
            }
            
            /**
             allocates a Block of given size
             first checks if given is below or equal than our threshold then allocates
             if our primary allocator fails then we use our fallback
             if the given size exceeds our threshold then we use our fallback
             */
            virtual Block Allocate(siz size) override
            {
                Block block;
                if (size <= _size_threshold)
                {
                    block = _primary->Allocate(size);
                }
                if (!block.IsValid())
                {
                    block = _fallback->Allocate(size);
                }
                return block;
            }
            
            /**
             deallocates a given Block
             */
            virtual bl Deallocate(Block& block) override
            {
                return _primary->Contains(block) ? _primary->Contains(block) : _fallback->Deallocate(block);
            }
            
            /**
             deallocates a block given the ptr
             */
            virtual bl Deallocate(void* ptr) override
            {
                return _primary->Contains(ptr) ? _primary->Contains(ptr) : _fallback->Deallocate(ptr);
            }
        };
    }
}

#endif /* NP_ENGINE_SEGEREGATED_ALLOCATOR_HPP */
