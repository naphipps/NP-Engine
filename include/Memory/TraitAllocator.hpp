//
//  TraitAllocator.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 7/20/21.
//

#ifndef NP_ENGINE_TRAIT_ALLOCATOR_HPP
#define NP_ENGINE_TRAIT_ALLOCATOR_HPP

#include "Primitive/Primitive.hpp"

//TODO: finish this

namespace np
{
    namespace memory
    {
        class TraitAllocator; //TODO: inherit anything?
        
        namespace _hidden //TODO: are we doing this or __detail? Or something different?
        {
            TraitAllocator* _trait_allocator = nullptr;
        }
        
        void* TraitAllocate(siz size) //TODO: we could make this a static for TraitAllocator? Maybe? I don't think that's needed
        {
            NP_ASSERT(_hidden::_trait_allocator != nullptr, "Registered TraitAllocator is nullptr");
            return _trait_allocator.Allocate(size).Begin();
        }
        
        void TraitDeallocate(void* ptr)
        {
            NP_ASSERT(_hidden::_trait_allocator != nullptr, "Registered TraitAllocator is nullptr");
            _trait_allocator.Deallocate(ptr);
        }
        
        class TraitAllocator
        {
        private:
            
        public:
            
            void register_this()
            {
                _hidden::_trait_allocator = this;
            }
            
            //TODO: in the actual implementation a malloc/free functions in a traits struct, we could right then and there set them
            
        };
        
        static TraitAllocator DefaultTraitAllocator;
        
    }
}

#endif /* NP_ENGINE_TRAIT_ALLOCATOR_HPP */
