//
//  AABB.hpp
//  Project Space
//
//  Created by Nathan Phipps on 1/12/21.
//  Copyright © 2021 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_AABB_HPP
#define NP_ENGINE_AABB_HPP

#include "Primitive/Primitive.hpp"

#include "Point.hpp"

namespace np
{
    namespace math
    {
        template <typename T>
        struct AABB
        {
            Point<T> LowerLeft;
            Point<T> UpperRight;
            
            bl Contains(const Point<T>& Point) const
            {
                bl contains = false;
                
                if (Point.x >= LowerLeft.x && Point.x <= UpperRight.x - 1 &&
                    Point.y >= LowerLeft.y && Point.y <= UpperRight.y - 1)
                {
                    contains = true;
                }
                
                return contains;
            }
        };
        
        using ui8AABB = AABB<ui8>;
        using ui16AABB = AABB<ui16>;
        using ui32AABB = AABB<ui32>;
        using ui64AABB = AABB<ui64>;
        using i8AABB = AABB<i8>;
        using i16AABB = AABB<i16>;
        using i32AABB = AABB<i32>;
        using i64AABB = AABB<i64>;
        using fltAABB = AABB<flt>;
        using dblAABB = AABB<dbl>;
    }
}

#endif /* NP_ENGINE_AABB_HPP */
