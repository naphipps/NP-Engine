//
//  BresenhamLine.hpp
//  Project Space
//
//  Created by Nathan Phipps on 4/4/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_BRESENHAM_LINE_HPP
#define NP_ENGINE_BRESENHAM_LINE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "Point.hpp"

namespace np
{
    namespace math
    {
        namespace _hidden
        {
            /**
             gets the series of points on low sloped line given begin point and given end point
             */
            static inline ui16PointVector GetLowSlopedLine(ui16Point begin, ui16Point end)
            {
                ui16PointVector points;
                
                i32 dx = end.x - begin.x;
                i32 dy = end.y - begin.y;
                i32 yi = 1;
                
                if (dy < 0)
                {
                    yi = -yi;
                    dy = -dy;
                }
                
                i32 D = 2 * dy - dx;
                ui16 y = begin.y;
                
                for (ui16 x = begin.x; x <= end.x; x++)
                {
                    points.push_back(ui16Point{.x = x, .y = y});
                    
                    if (D > 0)
                    {
                        y += yi;
                        D -= 2 * dx;
                    }
                    
                    D += 2 * dy;
                }
                
                //points.push_back(end);
                
                return points;
            }
            
            /**
             gets the high sloped line given the begin point and the end point
             */
            static inline ui16PointVector GetHighSlopedLine(ui16Point begin, ui16Point end)
            {
                ui16PointVector points;
                
                i32 dx = end.x - begin.x;
                i32 dy = end.y - begin.y;
                i32 xi = 1;
                
                if (dx < 0)
                {
                    xi = -xi;
                    dx = -dx;
                }
                
                i32 D = 2 * dx - dy;
                ui16 x = begin.x;
                
                for (ui16 y = begin.y; y <= end.y; y++)
                {
                    points.push_back(ui16Point{.x = x, .y = y});
                    
                    if (D > 0)
                    {
                        x += xi;
                        D -= 2 * dy;
                    }
                    
                    D += 2 * dx;
                }
                
                //points.push_back(end);
                
                return points;
            }
            
            /**
             gets the series of points on low sloped line given begin point and given end point
             inputs include the line array and line size ref for us to set as long as it stays under the given max line size
             */
            static inline bl GetLowSlopedLine(ui16Point line[], ui32& line_size, const ui32 line_max_size, ui16Point begin, ui16Point end)
            {
                ui32 i = 0;
                bl result = i < line_max_size;
                
                i32 dx = end.x - begin.x;
                i32 dy = end.y - begin.y;
                i32 yi = 1;
                
                if (dy < 0)
                {
                    yi = -yi;
                    dy = -dy;
                }
                
                i32 D = 2 * dy - dx;
                ui16 y = begin.y;
                
                for (ui16 x = begin.x; x <= end.x && result; x++)
                {
                    if (result)
                    {
                        line[i].x = x;
                        line[i].y = y;
                        i++;
                        result = i < line_max_size;
                    }
                    
                    if (D > 0)
                    {
                        y += yi;
                        D -= 2 * dx;
                    }
                    
                    D += 2 * dy;
                }
                
                //points.push_back(end);
                
                line_size = i;
                
                return result;
            }
            
            /**
             gets the high sloped line given the begin point and the end point
             inputs include the line array and line size ref for us to set as long as it stays under the given max line size
             */
            static inline bl GetHighSlopedLine(ui16Point line[], ui32& line_size, const ui32 line_max_size, ui16Point begin, ui16Point end)
            {
                ui32 i = 0;
                bl result = i < line_max_size;
                
                i32 dx = end.x - begin.x;
                i32 dy = end.y - begin.y;
                i32 xi = 1;
                
                if (dx < 0)
                {
                    xi = -xi;
                    dx = -dx;
                }
                
                i32 D = 2 * dx - dy;
                ui16 x = begin.x;
                
                for (ui16 y = begin.y; y <= end.y && result; y++)
                {
                    if (result)
                    {
                        line[i].x = x;
                        line[i].y = y;
                        i++;
                        result = i < line_max_size;
                    }
                    
                    if (D > 0)
                    {
                        x += xi;
                        D -= 2 * dy;
                    }
                    
                    D += 2 * dx;
                }
                
                //points.push_back(end);
                
                line_size = i;
                
                return result;
            }
            
            /**
             gets the series of points on low sloped line given begin point and given end point
             inputs include the line array and line size ref for us to set as long as it stays under the given max line size
             */
            template <siz ARRAY_SIZE>
            static inline bl GetLowSlopedLine(ui16PointArray<ARRAY_SIZE>& line, ui32& line_size, ui16Point begin, ui16Point end)
            {
                ui32 i = 0;
                bl result = i < line.size();
                
                i32 dx = end.x - begin.x;
                i32 dy = end.y - begin.y;
                i32 yi = 1;
                
                if (dy < 0)
                {
                    yi = -yi;
                    dy = -dy;
                }
                
                i32 D = 2 * dy - dx;
                ui16 y = begin.y;
                
                for (ui16 x = begin.x; x <= end.x && result; x++)
                {
                    if (result)
                    {
                        line[i].x = x;
                        line[i].y = y;
                        i++;
                        result = i < line.size();
                    }
                    
                    if (D > 0)
                    {
                        y += yi;
                        D -= 2 * dx;
                    }
                    
                    D += 2 * dy;
                }
                
    //            if (end != line[i] && begin != end && i != line.Size())
    //            {
    //                line[i] = end;
    //                i++;
    //            }
                
                line_size = i;
                
                return result;
            }
            
            /**
             gets the high sloped line given the begin point and the end point
             inputs include the line array and line size ref for us to set as long as it stays under the given max line size
             */
            template <siz ARRAY_SIZE>
            static inline bl GetHighSlopedLine(ui16PointArray<ARRAY_SIZE>& line, ui32& line_size, ui16Point begin, ui16Point end)
            {
                ui32 i = 0;
                bl result = i < line.size();
                
                i32 dx = end.x - begin.x;
                i32 dy = end.y - begin.y;
                i32 xi = 1;
                
                if (dx < 0)
                {
                    xi = -xi;
                    dx = -dx;
                }
                
                i32 D = 2 * dx - dy;
                ui16 x = begin.x;
                
                for (ui16 y = begin.y; y <= end.y && result; y++)
                {
                    if (result)
                    {
                        line[i].x = x;
                        line[i].y = y;
                        i++;
                        result = i < line.size();
                    }
                    
                    if (D > 0)
                    {
                        x += xi;
                        D -= 2 * dy;
                    }
                    
                    D += 2 * dx;
                }
                
    //            if (end != line[i] && begin != end && i != line.Size())
    //            {
    //                line[i] = end;
    //                i++;
    //            }
                
                line_size = i;
                
                return result;
            }
        }
        
        /**
         gets the series of points on line from given begin point to given end point
         */
        static inline ui16PointVector GetBresenhamLinePoints(ui16Point begin, ui16Point end)
        {
            ui16PointVector points;
            
            if (math::abs(end.y-begin.y) < math::abs(end.x-begin.x))
            {
                if (begin.x > end.x)
                {
                    points = _hidden::GetLowSlopedLine(end, begin);
                }
                else
                {
                    points = _hidden::GetLowSlopedLine(begin, end);
                }
            }
            else
            {
                if (begin.y > end.y)
                {
                    points = _hidden::GetHighSlopedLine(end, begin);
                }
                else
                {
                    points = _hidden::GetHighSlopedLine(begin, end);
                }
            }
            
            return points;
        }
        
        /**
         gets the series of points on line from given begin point to given end point
         inputs include the line array and line size ref for us to set as long as it stays under the given max line size
         */
        static inline bl GetBresenhamLinePoints(ui16Point line[], ui32& line_size, const ui32 line_max_size, ui16Point begin, ui16Point end)
        {
            bl result = true;
            
            if (math::abs(end.y-begin.y) < math::abs(end.x-begin.x))
            {
                if (begin.x > end.x)
                {
                    result = _hidden::GetLowSlopedLine(line, line_size, line_max_size, end, begin);
                }
                else
                {
                    result = _hidden::GetLowSlopedLine(line, line_size, line_max_size, begin, end);
                }
            }
            else
            {
                if (begin.y > end.y)
                {
                    result = _hidden::GetHighSlopedLine(line, line_size, line_max_size, end, begin);
                }
                else
                {
                    result = _hidden::GetHighSlopedLine(line, line_size, line_max_size, begin, end);
                }
            }
            
            return result;
        }
        
        /**
         gets the series of points on line from given begin point to given end point
         inputs include the line array and line size ref for us to set as long as it stays under the given max line size
         */
        template <siz ARRAY_SIZE>
        static inline bl GetBresenhamLinePoints(ui16PointArray<ARRAY_SIZE>& line, ui32& line_size, ui16Point begin, ui16Point end)
        {
            bl result = true;
            
            if (math::abs(end.y-begin.y) < math::abs(end.x-begin.x))
            {
                if (begin.x > end.x)
                {
                    result = _hidden::GetLowSlopedLine(line, line_size, end, begin);
                }
                else
                {
                    result = _hidden::GetLowSlopedLine(line, line_size, begin, end);
                }
            }
            else
            {
                if (begin.y > end.y)
                {
                    result = _hidden::GetHighSlopedLine(line, line_size, end, begin);
                }
                else
                {
                    result = _hidden::GetHighSlopedLine(line, line_size, begin, end);
                }
            }
            
            return result;
        }
    }
}

#endif /* NP_ENGINE_BRESENHAM_LINE_HPP */
