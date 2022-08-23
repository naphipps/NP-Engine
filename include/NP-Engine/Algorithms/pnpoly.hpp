//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/15/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_PNPOLY_HPP
#define NP_ENGINE_PNPOLY_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Math/Math.hpp"

namespace np::alg
{
	// TODO: handle precalc_pnpoly from <http://alienryderflex.com/polygon/>
	namespace precalc_pnpoly // TODO: refactor to __detail
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

	} // namespace precalc_pnpoly

	/*
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
	template <typename T, ::glm::qualifier Q = ::glm::qualifier::defaultp>
	static bl pnpoly(const ::glm::vec<2, T, Q> poly[], const siz poly_size, const ::glm::vec<2, T, Q>& point)
	{
		dbl x = (dbl)point.x;
		dbl y = (dbl)point.y;

		siz i = 0;
		siz j = poly_size - 1; // handle wrap around
		bl oddNodes = false;

		for (i = 0; i < poly_size; i++)
		{
			if ((poly[i].y < y && poly[j].y >= y) || (poly[j].y < y && poly[i].y >= y))
			{
				if ((dbl)poly[i].x +
						(dbl)(y - (dbl)poly[i].y) / (dbl)((dbl)poly[j].y - (dbl)poly[i].y) *
							(dbl)((dbl)poly[j].x - (dbl)poly[i].x) <
					x)
				{
					oddNodes = !oddNodes;
				}
			}

			j = i;
		}

		return oddNodes;
	}

	template <siz POLY_SIZE, typename T, ::glm::qualifier Q = ::glm::qualifier::defaultp>
	static bl pnpoly(const con::array<::glm::vec<2, T, Q>, POLY_SIZE>& poly, const ::glm::vec<2, T, Q>& point)
	{
		return pnpoly(poly.data(), poly.size(), point);
	}
} // namespace np::alg

#endif /* NP_ENGINE_PNPOLY_HPP */
