//
//  MidpointCircle.hpp
//  Project Space
//
//  Created by Nathan Phipps on 4/4/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_MIDPOINTCIRCLE_HPP
#define NP_ENGINE_MIDPOINTCIRCLE_HPP

#include "Container/Container.hpp"

#include "Point.hpp"
#include "PointMath.hpp"
#include "math_functions.hpp"
#include "trig_functions.hpp"

namespace np
{
    namespace math
    {
        /**
         gets the points about the origin representing a circle via the midpoint circle algorithm
         */
        static inline fltPointVector GetMidpointCirclePoints(const flt radius)
        {
            fltPointVector points;
            
            flt x = radius;
            flt y = 0;
            
            points.push_back(fltPoint{.x=x, .y=y});
            
            // When radius is zero only a single point
            if (radius > 0)
            {
                points.push_back(fltPoint{.x=-x, .y=y});
                points.push_back(fltPoint{.x=y, .y=x});
                points.push_back(fltPoint{.x=y, .y=-x});
            }
            
            // Initialising the value of P
            i32 P = 1 - radius;
            while (x > y)
            {
                y++;
                
                // Mid-point is inside or on the perimeter
                if (P <= 0)
                {
                    P = P + 2 * y + 1;
                }
                // Mid-point is outside the perimeter
                else
                {
                    x--;
                    P = P + 2 * y - 2 * x + 1;
                }
                
                // All the perimeter points have already been printed
                if (x < y)
                {
                    break;
                }
                
                // Printing the generated point and its reflection
                // in the other octants after translation
                points.push_back(fltPoint{.x=x, .y=y});
                points.push_back(fltPoint{.x=-x, .y=y});
                points.push_back(fltPoint{.x=x, .y=-y});
                points.push_back(fltPoint{.x=-x, .y=-y});
                
                // If the generated point is on the line x = y then
                // the perimeter points have already been printed
                if (x != y)
                {
                    points.push_back(fltPoint{.x=y, .y=x});
                    points.push_back(fltPoint{.x=-y, .y=x});
                    points.push_back(fltPoint{.x=y, .y=-x});
                    points.push_back(fltPoint{.x=-y, .y=-x});
                }
            }
            
            //sort CCW
            //sort(points.begin(), points.end(),
            //     [](const fltPoint & a, const fltPoint & b)
            //{
            //    return getAngle(a) < getAngle(b);
            //});
            
            ::std::sort(points.begin(), points.end(), ccwAngleComparer<flt>);
            
            return points;
        }
        
        /**
         gets the points about the origin representing a circle via the midpoint circle algorithm
         inputs include the line array and line size ref for us to set as long as it stays under the given max line size
         */
        static inline bl GetMidpointCirclePoints(const flt radius, ui16Point* points, ui32& points_size, const ui32 points_max_size, const ui16Point& offset)
        {
            ui32 i = 0;
            bl result = i < points_max_size;
            
            flt x = radius;
            flt y = 0;
            
            if (result)
            {
                points[i].x = x + offset.x;
                points[i].y = y + offset.y;
                i++;
            }
            
            // When radius is zero only a single point
            if (radius > 0)
            {
                if (result)
                {
                    points[i].x = -x + offset.x;
                    points[i].y = y + offset.y;
                    i++;
                    result = i < points_max_size;
                }
                
                if (result)
                {
                    points[i].x = y + offset.x;
                    points[i].y = x + offset.y;
                    i++;
                    result = i < points_max_size;
                }
                
                if (result)
                {
                    points[i].x = y + offset.x;
                    points[i].y = -x + offset.y;
                    i++;
                    result = i < points_max_size;
                }
            }
            
            // Initialising the value of P
            i32 P = 1 - radius;
            while (result)
            {
                if (!(x > y))
                {
                    break;
                }
                
                y++;
                
                // Mid-point is inside or on the perimeter
                if (P <= 0)
                {
                    P = P + 2 * y + 1;
                }
                // Mid-point is outside the perimeter
                else
                {
                    x--;
                    P = P + 2 * y - 2 * x + 1;
                }
                
                // All the perimeter points have already been printed
                if (x < y)
                {
                    break;
                }
                
                // Printing the generated point and its reflection
                // in the other octants after translation
                if (result)
                {
                    points[i].x = x + offset.x;
                    points[i].y = y + offset.y;
                    i++;
                    result = i < points_max_size;
                }
                
                if (result)
                {
                    points[i].x = -x + offset.x;
                    points[i].y = y + offset.y;
                    i++;
                    result = i < points_max_size;
                }
                
                if (result)
                {
                    points[i].x = x + offset.x;
                    points[i].y = -y + offset.y;
                    i++;
                    result = i < points_max_size;
                }
                
                if (result)
                {
                    points[i].x = -x + offset.x;
                    points[i].y = -y + offset.y;
                    i++;
                    result = i < points_max_size;
                }
                
                // If the generated point is on the line x = y then
                // the perimeter points have already been printed
                if (x != y)
                {
                    if (result)
                    {
                        points[i].x = y + offset.x;
                        points[i].y = x + offset.y;
                        i++;
                        result = i < points_max_size;
                    }
                    
                    if (result)
                    {
                        points[i].x = -y + offset.x;
                        points[i].y = x + offset.y;
                        i++;
                        result = i < points_max_size;
                    }
                    
                    if (result)
                    {
                        points[i].x = y + offset.x;
                        points[i].y = -x + offset.y;
                        i++;
                        result = i < points_max_size;
                    }
                    
                    if (result)
                    {
                        points[i].x = -y + offset.x;
                        points[i].y = -x + offset.y;
                        i++;
                        result = i < points_max_size;
                    }
                }
            }
            
            points_size = i;
            
            //sort CCW
            if (result)
            {
                ::std::sort(points, points + points_size, [&offset](const ui16Point & a, const ui16Point & b)
                {
                    return atan2(-((flt)a.y) + (flt)offset.y, -((flt)a.x) + (flt)offset.x) <
                    atan2(-((flt)b.y) + (flt)offset.y, -((flt)b.x) + (flt)offset.x);
                });
            }
            
            return result;
        }
        
        /**
         gets the points about the origin representing a circle via the midpoint circle algorithm
         inputs include the line array and line size ref for us to set as long as it stays under the given max line size
         */
        template <siz ARRAY_SIZE>
        static inline bl GetMidpointCirclePoints(const flt radius, ui16PointArray<ARRAY_SIZE>& points, ui32& points_size, const ui16Point& offset)
        {
            ui32 i = 0;
            bl result = i < points.size();
            
            flt x = radius;
            flt y = 0;
            
            if (result)
            {
                points[i].x = x + offset.x;
                points[i].y = y + offset.y;
                i++;
            }
            
            // When radius is zero only a single point
            if (radius > 0)
            {
                if (result)
                {
                    points[i].x = -x + offset.x;
                    points[i].y = y + offset.y;
                    i++;
                    result = i < points.size();
                }
                
                if (result)
                {
                    points[i].x = y + offset.x;
                    points[i].y = x + offset.y;
                    i++;
                    result = i < points.size();
                }
                
                if (result)
                {
                    points[i].x = y + offset.x;
                    points[i].y = -x + offset.y;
                    i++;
                    result = i < points.size();
                }
            }
            
            // Initialising the value of P
            i32 P = 1 - radius;
            while (result)
            {
                if (!(x > y))
                {
                    break;
                }
                
                y++;
                
                // Mid-point is inside or on the perimeter
                if (P <= 0)
                {
                    P = P + 2 * y + 1;
                }
                // Mid-point is outside the perimeter
                else
                {
                    x--;
                    P = P + 2 * y - 2 * x + 1;
                }
                
                // All the perimeter points have already been printed
                if (x < y)
                {
                    break;
                }
                
                // Printing the generated point and its reflection
                // in the other octants after translation
                if (result)
                {
                    points[i].x = x + offset.x;
                    points[i].y = y + offset.y;
                    i++;
                    result = i < points.size();
                }
                
                if (result)
                {
                    points[i].x = -x + offset.x;
                    points[i].y = y + offset.y;
                    i++;
                    result = i < points.size();
                }
                
                if (result)
                {
                    points[i].x = x + offset.x;
                    points[i].y = -y + offset.y;
                    i++;
                    result = i < points.size();
                }
                
                if (result)
                {
                    points[i].x = -x + offset.x;
                    points[i].y = -y + offset.y;
                    i++;
                    result = i < points.size();
                }
                
                // If the generated point is on the line x = y then
                // the perimeter points have already been printed
                if (x != y)
                {
                    if (result)
                    {
                        points[i].x = y + offset.x;
                        points[i].y = x + offset.y;
                        i++;
                        result = i < points.size();
                    }
                    
                    if (result)
                    {
                        points[i].x = -y + offset.x;
                        points[i].y = x + offset.y;
                        i++;
                        result = i < points.size();
                    }
                    
                    if (result)
                    {
                        points[i].x = y + offset.x;
                        points[i].y = -x + offset.y;
                        i++;
                        result = i < points.size();
                    }
                    
                    if (result)
                    {
                        points[i].x = -y + offset.x;
                        points[i].y = -x + offset.y;
                        i++;
                        result = i < points.size();
                    }
                }
            }
            
            points_size = i;
            
            //sort CCW
            if (result)
            {
                ::std::sort(points.begin(), points.begin() + points_size, [&offset](const ui16Point& a, const ui16Point& b)
                {
                    return atan2(-((flt)a.y) + (flt)offset.y, -((flt)a.x) + (flt)offset.x) <
                    atan2(-((flt)b.y) + (flt)offset.y, -((flt)b.x) + (flt)offset.x);
                });
            }
            
            return result;
        }
    }
}

#endif /* NP_ENGINE_MIDPOINTCIRCLE_HPP */
