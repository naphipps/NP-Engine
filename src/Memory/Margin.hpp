//
//  Margin.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 3/2/21.
//

#ifndef NP_ENGINE_MARGIN_HPP
#define NP_ENGINE_MARGIN_HPP

#include "../Primitive/Primitive.hpp"

namespace np
{
    namespace memory
    {
        namespace hidden
        {
            /**
             represents the margins we use as our header and footers for allocations
             */
            struct Margin
            {
                siz Value = 0;
                
                /**
                 checks if this margin is allocated or not
                 */
                bl IsAllocated() const
                {
                    return Value & 1;
                }
                
                /**
                 gets the size of the allocation
                 */
                siz GetSize() const
                {
                    return (Value >> 1) << 1;
                }
                
                /**
                 sets the size of the margin and maintains the allocation bit
                 */
                void SetSize(siz size)
                {
                    Value = IsAllocated() ? size & 1 : size;
                }
                
                /**
                 sets the bit indicating this margin as allocated
                 */
                void SetAllocated()
                {
                    Value |= 1;
                }
                
                /**
                 clears the bit indicating this margin as allocated
                 */
                void SetDeallocated()
                {
                    Value = GetSize();
                }
            };
            
            using MarginPtr = Margin*;
            
            const static siz MARGIN_ALIGNED_SIZE = CalcAlignedSize(sizeof(Margin));
        }
    }
}

#endif /* NP_ENGINE_MARGIN_HPP */
