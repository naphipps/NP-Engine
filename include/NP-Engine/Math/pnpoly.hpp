//
//  pnpoly.hpp
//  Project Space
//
//  Created by Nathan Phipps on 5/15/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_PNPOLY_HPP
#define NP_ENGINE_PNPOLY_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "Point.hpp"

namespace np
{
    namespace math
    {
        //TODO: handle precalc_pnpoly from <http://alienryderflex.com/polygon/>
        namespace precalc_pnpoly
        {
            
            //  Globals which should be set before calling these functions:
            //
            //  i32    polyCorners  =  how many corners the polygon has (no repeats)
            //  flt  polyX[]      =  horizontal coordinates of corners
            //  flt  polyY[]      =  vertical coordinates of corners
            //  flt  x, y         =  point to be tested
            //
            //  The following global arrays should be allocated before calling these functions:
            //
            //  flt  constant[] = storage for precalculated constants (same size as polyX)
            //  flt  multiple[] = storage for precalculated multipliers (same size as polyX)
            //
            //  (Globals are used in this example for purposes of speed.  Change as
            //  desired.)
            //
            //  USAGE:
            //  Call precalc_values() to initialize the constant[] and multiple[] arrays,
            //  then call pointInPolygon(x, y) to determine if the point is in the polygon.
            //
            //  The function will return YES if the point x,y is inside the polygon, or
            //  NO if it is not.  If the point is exactly on the edge of the polygon,
            //  then the function may return YES or NO.
            //
            //  Note that division by zero is avoided because the division is protected
            //  by the "if" clause which surrounds it.
            
            /*
            void precalc_values()
            {
                i32 i, j=polyCorners-1 ;
                
                for(i=0; i<polyCorners; i++)
                {
                    if(polyY[j]==polyY[i])
                    {
                        constant[i]=polyX[i];
                        multiple[i]=0;
                    }
                    else
                    {
                        constant[i]=polyX[i]-(polyY[i]*polyX[j])/(polyY[j]-polyY[i])+(polyY[i]*polyX[i])/(polyY[j]-polyY[i]);
                        multiple[i]=(polyX[j]-polyX[i])/(polyY[j]-polyY[i]);
                        
                    }
                    j=i;
                    
                }
                
            }
            
            bl pointInPolygon_precal()
            {
                i32   i, j=polyCorners-1 ;
                bl  oddNodes=NO      ;
                
                for (i=0; i<polyCorners; i++)
                {
                    if ((polyY[i]< y && polyY[j]>=y ||
                         polyY[j]< y && polyY[i]>=y))
                    {
                        oddNodes^=(y*multiple[i]+constant[i]<x);
                        
                    }
                    j=i;
                }
                
                return oddNodes;
            }
            
            //smart optimization provided by Evgueni Tcherniaev
            bl pointInPolygon_precal_optimized()
            {
                
                bl oddNodes=NO, current=polY[polyCorners-1]>y, previous;
                for (i32 i=0; i<polyCorners; i++)
                {
                    previous=current; current=polyY[i]>y;
                    if (current!=previous)
                    {
                        oddNodes^=y*multiple[i]+constant[i]<x;
                    }
                    
                }
                return oddNodes;
                
            }
            //*/
             
        }
        
        /**
         //  Randolph Franklin pnpoly
         //  Globals which should be set before calling this function:
         //
         //  i32    polyCorners  =  how many corners the polygon has (no repeats)
         //  flt  polyX[]      =  horizontal coordinates of corners
         //  flt  polyY[]      =  vertical coordinates of corners
         //  flt  x, y         =  point to be tested
         //
         //  (Globals are used in this example for purposes of speed.  Change as
         //  desired.)
         //
         //  The function will return YES if the point x,y is inside the polygon, or
         //  NO if it is not.  If the point is exactly on the edge of the polygon,
         //  then the function may return YES or NO.
         //
         //  Note that division by zero is avoided because the division is protected
         //  by the "if" clause which surrounds it.
         */
        static bl pnpoly(const math::ui16Point poly[], const ui32 poly_size, const math::ui16Point& point)
        {
            flt x = (flt)point.x;
            flt y = (flt)point.y;
            
            i32 i = 0;
            i32 j = poly_size - 1; //handle wrap around
            bl oddNodes = false;
            
            for (i=0; i < poly_size; i++)
            {
                if ((poly[i].y < y && poly[j].y >= y) ||
                    (poly[j].y < y && poly[i].y >= y))
                {
                    if ((flt)poly[i].x + (flt)(y - (flt)poly[i].y) /
                        (flt)((flt)poly[j].y - (flt)poly[i].y) *
                        (flt)((flt)poly[j].x - (flt)poly[i].x) < x)
                    {
                        oddNodes = !oddNodes;
                    }
                }
                
                j=i;
            }
            
            return oddNodes;
        }
        
        /**
         same pnpoly as above, but we support primitive array here
         */
        template <siz ARRAY_SIZE>
        static bl pnpoly(const ui16PointArray<ARRAY_SIZE>& poly, const ui32 poly_size, const math::ui16Point& point)
        {
            //TODO: could we just call the other pnpoly with poly.data()??
            //return pnpoly(poly.data(), poly_size, point); //?
            
            flt x = (flt)point.x;
            flt y = (flt)point.y;
            
            i32 i = 0;
            i32 j = poly_size - 1; //handle wrap around
            bl oddNodes = false;
            
            for (i=0; i < poly_size; i++)
            {
                if ((poly[i].y < y && poly[j].y >= y) ||
                    (poly[j].y < y && poly[i].y >= y))
                {
                    if ((flt)poly[i].x + (flt)(y - (flt)poly[i].y) /
                        (flt)((flt)poly[j].y - (flt)poly[i].y) *
                        (flt)((flt)poly[j].x - (flt)poly[i].x) < x)
                    {
                        oddNodes = !oddNodes;
                    }
                }
                
                j=i;
            }
            
            return oddNodes;
        }
    }
}

#endif /* NP_ENGINE_PNPOLY_HPP */
