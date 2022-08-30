//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/4/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MIDPOINTCIRCLE_HPP
#define NP_ENGINE_MIDPOINTCIRCLE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Math/Math.hpp"

namespace np::alg
{
	/*
		gets the points about the origin representing a circle via the midpoint circle algorithm
	*/
	template <typename T>
	static inline con::vector<::glm::vec<2, T>> GetMidpointCirclePoints(const dbl radius)
	{
		con::vector<::glm::vec<2, T>> points;

		dbl x = radius;
		dbl y = 0;

		points.emplace_back(x, y);

		// When radius is zero only a single point
		if (radius > 0)
		{
			points.emplace_back(-x, y);
			points.emplace_back(y, x);
			points.emplace_back(y, -x);
		}

		// Initialising the value of P
		i64 P = 1 - radius;
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
				break;

			// Printing the generated point and its reflection
			// in the other octants after translation
			points.emplace_back(x, y);
			points.emplace_back(-x, y);
			points.emplace_back(x, -y);
			points.emplace_back(-x, -y);

			// If the generated point is on the line x = y then
			// the perimeter points have already been printed
			if (x != y)
			{
				points.emplace_back(y, x);
				points.emplace_back(-y, x);
				points.emplace_back(y, -x);
				points.emplace_back(-y, -x);
			}
		}

		::std::sort(points.begin(), points.end(), mat::AngleComparerCCW<T>);

		return points;
	}

	/*
		gets the points about the origin representing a circle via the midpoint circle algorithm
		inputs include the line array and line size ref for us to set as long as it stays under the given max line size
	*/
	template <typename T>
	static inline bl GetMidpointCirclePoints(const dbl radius, ::glm::vec<2, T> points[], siz& points_size,
											 const siz points_max_size, const ::glm::vec<2, T>& offset)
	{
		siz i = 0;
		bl result = i <= points_max_size;

		dbl x = radius;
		dbl y = 0;

		if (result)
		{
			points[i].x = x + offset.x;
			points[i].y = y + offset.y;
			result = ++i <= points_max_size;
		}

		// When radius is zero only a single point
		if (radius > 0)
		{
			if (result)
			{
				points[i].x = -x + offset.x;
				points[i].y = y + offset.y;
				result = ++i <= points_max_size;
			}

			if (result)
			{
				points[i].x = y + offset.x;
				points[i].y = x + offset.y;
				result = ++i <= points_max_size;
			}

			if (result)
			{
				points[i].x = y + offset.x;
				points[i].y = -x + offset.y;
				result = ++i <= points_max_size;
			}
		}

		// Initialising the value of P
		i64 P = 1 - radius;
		while (result)
		{
			if (!(x > y))
				break;

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
				break;

			// Printing the generated point and its reflection
			// in the other octants after translation
			if (result)
			{
				points[i].x = x + offset.x;
				points[i].y = y + offset.y;
				result = ++i <= points_max_size;
			}

			if (result)
			{
				points[i].x = -x + offset.x;
				points[i].y = y + offset.y;
				result = ++i <= points_max_size;
			}

			if (result)
			{
				points[i].x = x + offset.x;
				points[i].y = -y + offset.y;
				result = ++i <= points_max_size;
			}

			if (result)
			{
				points[i].x = -x + offset.x;
				points[i].y = -y + offset.y;
				result = ++i <= points_max_size;
			}

			// If the generated point is on the line x = y then
			// the perimeter points have already been printed
			if (x != y)
			{
				if (result)
				{
					points[i].x = y + offset.x;
					points[i].y = x + offset.y;
					result = ++i <= points_max_size;
				}

				if (result)
				{
					points[i].x = -y + offset.x;
					points[i].y = x + offset.y;
					result = ++i <= points_max_size;
				}

				if (result)
				{
					points[i].x = y + offset.x;
					points[i].y = -x + offset.y;
					result = ++i <= points_max_size;
				}

				if (result)
				{
					points[i].x = -y + offset.x;
					points[i].y = -x + offset.y;
					result = ++i <= points_max_size;
				}
			}
		}

		points_size = i;

		if (result)
		{
			::std::sort(points, points + points_size,
						[&offset](const ::glm::vec<2, T>& a, const ::glm::vec<2, T>& b)
						{
							::glm::vec<2, T> ao{offset.x - a.x, offset.y - a.y};
							::glm::vec<2, T> bo{offset.x - b.x, offset.y - b.y};

							return mat::AngleComparerCCW<T>(ao, bo);
						});
		}

		return result;
	}

	template <siz ARRAY_SIZE, typename T>
	static inline bl GetMidpointCirclePoints(const dbl radius, con::array<::glm::vec<2, T>, ARRAY_SIZE>& points,
											 siz& points_size, const ::glm::vec<2, T>& offset)
	{
		return GetMidpointCirclePoints<T>(radius, points.data(), points_size, points.size(), offset);
	}
} // namespace np::alg

#endif /* NP_ENGINE_MIDPOINTCIRCLE_HPP */
