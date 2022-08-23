//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/4/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_BRESENHAM_LINE_HPP
#define NP_ENGINE_BRESENHAM_LINE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Math/Math.hpp"

namespace np::alg
{
	namespace __detail
	{
		/*
			gets the series of points on low sloped line given begin point and given end point
		*/
		template <typename T, ::glm::qualifier Q = ::glm::qualifier::defaultp>
		static inline con::vector<::glm::vec<2, T, Q>> GetLowSlopedLine(const ::glm::vec<2, T, Q>& begin,
																		const ::glm::vec<2, T, Q>& end)
		{
			con::vector<::glm::vec<2, T, Q>> points;

			T dx = end.x - begin.x;
			T dy = end.y - begin.y;
			T yi = 1;

			if (dy < 0)
			{
				yi = -yi;
				dy = -dy;
			}

			T D = 2 * dy - dx;
			T y = begin.y;

			for (T x = begin.x; x <= end.x; x++)
			{
				points.emplace_back(x, y);

				if (D > 0)
				{
					y += yi;
					D -= 2 * dx;
				}

				D += 2 * dy;
			}

			return points;
		}

		/*
			gets the high sloped line given the begin point and the end point
		*/
		template <typename T, ::glm::qualifier Q = ::glm::qualifier::defaultp>
		static inline con::vector<::glm::vec<2, T, Q>> GetHighSlopedLine(const ::glm::vec<2, T, Q>& begin,
																		 const ::glm::vec<2, T, Q>& end)
		{
			con::vector<::glm::vec<2, T, Q>> points;

			T dx = end.x - begin.x;
			T dy = end.y - begin.y;
			T xi = 1;

			if (dx < 0)
			{
				xi = -xi;
				dx = -dx;
			}

			T D = 2 * dx - dy;
			T x = begin.x;

			for (T y = begin.y; y <= end.y; y++)
			{
				points.emplace_back(x, y);

				if (D > 0)
				{
					x += xi;
					D -= 2 * dy;
				}

				D += 2 * dx;
			}

			return points;
		}

		/*
			gets the series of points on low sloped line given begin point and given end point
			inputs include the line array and line size ref for us to set as long as it stays under the given max line size
		*/
		template <typename T, ::glm::qualifier Q = ::glm::qualifier::defaultp>
		static inline bl GetLowSlopedLine(::glm::vec<2, T, Q> line[], siz& line_size, const siz line_max_size,
										  const ::glm::vec<2, T, Q>& begin, const ::glm::vec<2, T, Q>& end)
		{
			siz i = 0;
			bl result = i < line_max_size;

			T dx = end.x - begin.x;
			T dy = end.y - begin.y;
			T yi = 1;

			if (dy < 0)
			{
				yi = -yi;
				dy = -dy;
			}

			T D = 2 * dy - dx;
			T y = begin.y;

			for (T x = begin.x; x <= end.x && result; x++)
			{
				if (result)
				{
					line[i].x = x;
					line[i].y = y;
					result = ++i <= line_max_size;
				}

				if (D > 0)
				{
					y += yi;
					D -= 2 * dx;
				}

				D += 2 * dy;
			}

			line_size = i;

			return result;
		}

		/*
			gets the high sloped line given the begin point and the end point
			inputs include the line array and line size ref for us to set as long as it stays under the given max line size
		*/
		template <typename T, ::glm::qualifier Q = ::glm::qualifier::defaultp>
		static inline bl GetHighSlopedLine(::glm::vec<2, T, Q> line[], siz& line_size, const siz line_max_size,
										   const ::glm::vec<2, T, Q>& begin, const ::glm::vec<2, T, Q>& end)
		{
			siz i = 0;
			bl result = i < line_max_size;

			T dx = end.x - begin.x;
			T dy = end.y - begin.y;
			T xi = 1;

			if (dx < 0)
			{
				xi = -xi;
				dx = -dx;
			}

			T D = 2 * dx - dy;
			T x = begin.x;

			for (T y = begin.y; y <= end.y && result; y++)
			{
				if (result)
				{
					line[i].x = x;
					line[i].y = y;
					result = ++i <= line_max_size;
				}

				if (D > 0)
				{
					x += xi;
					D -= 2 * dy;
				}

				D += 2 * dx;
			}

			line_size = i;

			return result;
		}

		template <siz ARRAY_SIZE, typename T, ::glm::qualifier Q = ::glm::qualifier::defaultp>
		static inline bl GetLowSlopedLine(con::array<::glm::vec<2, T, Q>, ARRAY_SIZE>& line, siz& line_size,
										  const ::glm::vec<2, T, Q>& begin, const ::glm::vec<2, T, Q>& end)
		{
			return GetLowSlopedLine(line.data(), line_size, line.size(), begin, end);
		}

		template <siz ARRAY_SIZE, typename T, ::glm::qualifier Q = ::glm::qualifier::defaultp>
		static inline bl GetHighSlopedLine(con::array<::glm::vec<2, T, Q>, ARRAY_SIZE>& line, siz& line_size,
										   const ::glm::vec<2, T, Q>& begin, const ::glm::vec<2, T, Q>& end)
		{
			return GetHighSlopedLine(line.data(), line_size, line.size(), begin, end);
		}
	} // namespace __detail

	/*
		gets the series of points on line from given begin point to given end point
	*/
	template <typename T, ::glm::qualifier Q = ::glm::qualifier::defaultp>
	static inline con::vector<::glm::vec<2, T, Q>> GetBresenhamLinePoints(const ::glm::vec<2, T, Q>& begin,
																		  const ::glm::vec<2, T, Q>& end)
	{
		con::vector<::glm::vec<2, T, Q>> points;

		if (::std::abs(end.y - begin.y) < ::std::abs(end.x - begin.x))
		{
			if (begin.x > end.x)
				points = __detail::GetLowSlopedLine(end, begin);
			else
				points = __detail::GetLowSlopedLine(begin, end);
		}
		else
		{
			if (begin.y > end.y)
				points = __detail::GetHighSlopedLine(end, begin);
			else
				points = __detail::GetHighSlopedLine(begin, end);
		}

		return points;
	}

	/*
		gets the series of points on line from given begin point to given end point
		inputs include the line array and line size ref for us to set as long as it stays under the given max line size
	*/
	template <typename T, ::glm::qualifier Q = ::glm::qualifier::defaultp>
	static inline bl GetBresenhamLinePoints(::glm::vec<2, T, Q> line[], siz& line_size, const siz line_max_size,
											const ::glm::vec<2, T, Q>& begin, const ::glm::vec<2, T, Q>& end)
	{
		bl result = true;

		if (::std::abs(end.y - begin.y) < ::std::abs(end.x - begin.x))
		{
			if (begin.x > end.x)
				result = __detail::GetLowSlopedLine(line, line_size, line_max_size, end, begin);
			else
				result = __detail::GetLowSlopedLine(line, line_size, line_max_size, begin, end);
		}
		else
		{
			if (begin.y > end.y)
				result = __detail::GetHighSlopedLine(line, line_size, line_max_size, end, begin);
			else
				result = __detail::GetHighSlopedLine(line, line_size, line_max_size, begin, end);
		}

		return result;
	}

	template <siz ARRAY_SIZE, typename T, ::glm::qualifier Q = ::glm::qualifier::defaultp>
	static inline bl GetBresenhamLinePoints(con::array<::glm::vec<2, T, Q>, ARRAY_SIZE>& line, siz& line_size,
											const ::glm::vec<2, T, Q>& begin, const ::glm::vec<2, T, Q>& end)
	{
		return GetBresenhamLinePoints<T, Q>(line.data(), line_size, line.size(), begin, end);
	}
} // namespace np::alg

#endif /* NP_ENGINE_BRESENHAM_LINE_HPP */
