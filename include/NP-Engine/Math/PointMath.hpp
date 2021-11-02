//
//  PointMath.h
//  Project Space
//
//  Created by Nathan Phipps on 1/24/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_POINT_MATH_HPP
#define NP_ENGINE_POINT_MATH_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "Point.hpp"
#include "AABB.hpp"
#include "math_functions.hpp"
#include "trig_functions.hpp"
#include "ease_functions.hpp"

namespace np
{
    namespace math
    {
        /**
         lerps between point a and b
         */
        template <typename T>
        static inline fltPoint lerp(const Point<T>& a, const Point<T>& b, const flt t)
        {
            return fltPoint
            {
                .x = lerp((flt)a.x, (flt)b.x, t),
                .y = lerp((flt)a.y, (flt)b.y, t)
            };
        }
        
        /**
         gets the midpoint between points a and b
         */
        template <typename T>
        static inline fltPoint midpoint(const Point<T>& a, const Point<T>& b)
        {
            return fltPoint
            {
                .x = ((flt)a.x + (flt)b.x) / 2.0f,
                .y = ((flt)a.y + (flt)b.y) / 2.0f
            };
        }
                
        /**
         rounds the point's valus to the nearest 32th
         */
        static inline void roundTo32nd(fltPoint& point) //TODO: make template but require flt and dbl
        {
            point.x = roundTo32nd(point.x);
            point.y = roundTo32nd(point.y);
        }
        
        /**
         rounds the point's values to the nearest16th
         */
        static inline void roundTo16th(fltPoint& point)
        {
            point.x = roundTo16th(point.x);
            point.y = roundTo16th(point.y);
        }
        
        /**
         gets the distance squared value from point a to b
         */
        static inline dbl distanceSquared(const fltPoint& a, const fltPoint& b)
        {
            return distanceSquared(a.x, a.y, b.x, b.y);
        }
        
        /**
         gets the distance squared value from point a to b
         */
        static inline dbl distanceSquared(const dblPoint& a, const dblPoint& b)
        {
            return distanceSquared(a.x, a.y, b.x, b.y);
        }
        
        /**
         gets the distance squared value from point a to b
         */
        template <typename T>
        static inline ui64 distanceSquared(const Point<T>& a, const Point<T>& b)
        {
            return distanceSquared(a.x, a.y, b.x, b.y);
        }
        
        /**
         gets the atan2 of the point
         */
        template <typename T>
        static inline flt getAngle(const Point<T>& point)
        {
            return math::atan2(-((flt)point.y), -((flt)point.x));
        }
        
        /**
         gets the angle given the points abc
         */
        template <typename T>
        static inline flt getAngleB(const Point<T>& a_pt, const Point<T>& b_pt, const Point<T>& c_pt)
        {
            flt a_sq = distanceSquared(b_pt, c_pt);
            flt b_sq = distanceSquared(a_pt, c_pt);
            flt c_sq = distanceSquared(b_pt, a_pt);
            flt a = math::sqrt(a_sq);
            flt c = math::sqrt(c_sq);
            flt B = math::acos((a_sq - b_sq + c_sq) / (2.f * a * c));
            return B;
        }
        
        /**
         compares the angles of points a and b in a ccw fashion
         */
        template <typename T>
        static inline bl ccwAngleComparer(const Point<T>& a, const Point<T>& b)
        {
            return getAngle(a) < getAngle(b);
        }
        
        /**
         gets the center points of the given aabb
         */
        template <typename T>
        static inline Point<T> centerPointOfAabb(const AABB<T>& aabb)
        {
            Point<T> point = (aabb.UpperRight - aabb.LowerLeft) / 2.f;
            point += aabb.LowerLeft;
            return point;
        }
        
        /**
         inputs points a, b, and c of a triangle
         outputs the area squared of given triangle
         
         if return > 0 then CCW
         else if return < 0 then CW
         else if return == 0 then colinear
         */
        static inline dbl getOrientation(const fltPoint& a, const fltPoint& b, const fltPoint& c)
        {
            /*
             
             |1  1  1 |
             |ax bx cx|
             |ay by cy|
             
             result = (1/2)((bx*cy - by*cx) - (ax*cy - ay*cx) + (ax*by - ay*bx))
             //but we don't need to divide by 2, just get relationship to 0
             
             if result > 0 then CCW
             else if result < 0 then CW
             else if result == 0 then colinear
             
             */
            
            dbl ax = a.x;
            dbl ay = a.y;
            dbl bx = b.x;
            dbl by = b.y;
            dbl cx = c.x;
            dbl cy = c.y;
            
            dbl result = (bx * cy) * 0.5 - (by * cx) * 0.5 - (ax * cy) * 0.5 +
            (ay * cx) * 0.5 + (ax * by) * 0.5 - (ay * bx) * 0.5;
            
            //if we need output 1, 0, -1 then use below
            //result = result > 0 ? 1 : (result < 0 ? -1 : 0);
            
            return result;
        }
        
        /**
         inputs points a, b, and c of a triangle
         outputs the area squared of given triangle
         
         if return > 0 then CCW
         else if return < 0 then CW
         else if return == 0 then colinear
         */
        static inline dbl getOrientation(const dblPoint& a, const dblPoint& b, const dblPoint& c)
        {
            /*
             
             |1  1  1 |
             |ax bx cx|
             |ay by cy|
             
             result = (1/2)((bx*cy - by*cx) - (ax*cy - ay*cx) + (ax*by - ay*bx))
             //but we don't need to divide by 2, just get relationship to 0
             
             if result > 0 then CCW
             else if result < 0 then CW
             else if result == 0 then colinear
             
             */
            
            dbl ax = a.x;
            dbl ay = a.y;
            dbl bx = b.x;
            dbl by = b.y;
            dbl cx = c.x;
            dbl cy = c.y;
            
            dbl result = (bx * cy) * 0.5 - (by * cx) * 0.5 - (ax * cy) * 0.5 +
            (ay * cx) * 0.5 + (ax * by) * 0.5 - (ay * bx) * 0.5;
            
            //if we need output 1, 0, -1 then use below
            //result = result > 0 ? 1 : (result < 0 ? -1 : 0);
            
            return result;
        }
        
        /**
         inputs points a, b, and c of a triangle
         outputs the area squared of given triangle
         
         if return > 0 then CCW
         else if return < 0 then CW
         else if return == 0 then colinear
         */
        template <typename T>
        static inline i64 getOrientation(const Point<T>& a, const Point<T>& b, const Point<T>& c)
        {
            /*
             
             |1  1  1 |
             |ax bx cx|
             |ay by cy|
             
             result = (1/2)((bx*cy - by*cx) - (ax*cy - ay*cx) + (ax*by - ay*bx))
             //but we don't need to divide by 2, just get relationship to 0
             
             if result > 0 then CCW
             else if result < 0 then CW
             else if result == 0 then colinear
             
             */
            
            i64 ax = a.x;
            i64 ay = a.y;
            i64 bx = b.x;
            i64 by = b.y;
            i64 cx = c.x;
            i64 cy = c.y;
            
            i64 result = bx * cy - by * cx - ax * cy + ay * cx + ax * by - ay * bx;
            
            //if we need output 1, 0, -1 then use below
            //result = result > 0 ? 1 : (result < 0 ? -1 : 0);
            
            return result;
        }
        
        /**
         gets the t value from given point, from a to b
         */
        template <typename T>
        static inline flt getT(const Point<T>& a, const Point<T>& b, const Point<T>& point)
        {
            T x_diff = b.x > a.x ? b.x - a.x : a.x - b.x;
            T y_diff = b.y > a.y ? b.y - a.y : a.y - b.y;
            
            flt t;
            
            //we'll use the longest length (x or y direction) so our t value is accurate
            if (x_diff > y_diff)
            {
                //get t from x values
                t = ((flt)point.x - (flt)a.x) / ((flt)b.x - (flt)a.x);
            }
            else
            {
                //get t from y values
                t = ((flt)point.y - (flt)a.y) / ((flt)b.y - (flt)a.y);
            }
            
            return t;
        }
    }
}

#endif /* NP_ENGINE_POINT_MATH_HPP */
