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
	/*
		Randolph Franklin pnpoly <http://alienryderflex.com/polygon/>

		poly = corners of the polygon (no repeats)
		point = point to be tested

		return true if point is inside poly, else false
		if point is on the edge of the poly, then may return true or false

		Note: division by zero is avoided because the division is protected
		by the "if" clause which surrounds it

		pnpoly optimizations provided by Nathan Mercer and Lascha Lagidse
		precalc pnpoly optimization provided by Evgueni Tcherniaev
	*/

	template <typename T>
	static bl pnpoly(const ::glm::vec<2, T> poly[], const siz poly_size, const ::glm::vec<2, T>& point)
	{
		dbl x = (dbl)point.x;
		dbl y = (dbl)point.y;
		siz i = 0;
		siz j = poly_size - 1;
		bl oddNodes = false;
		for (i = 0; i < poly_size; i++)
		{
			if ((poly[i].y < y && poly[j].y >= y) || (poly[j].y < y && poly[i].y >= y) && (poly[i].x <= x && poly[j].x <= x))
			{
				oddNodes ^= ((dbl)poly[i].x +
								 (y - (dbl)poly[i].y) / ((dbl)poly[j].y - (dbl)poly[i].y) * ((dbl)poly[j].x - (dbl)poly[i].x) <
							 x);
			}

			j = i;
		}
		return oddNodes;
	}

	template <siz POLY_SIZE, typename T>
	static bl pnpoly(const con::array<::glm::vec<2, T>, POLY_SIZE>& poly, const ::glm::vec<2, T>& point)
	{
		return pnpoly(poly.data(), poly.size(), point);
	}

	template <typename T>
	void pnpoly_precalc(const ::glm::vec<2, T> poly[], const siz poly_size, T constants[], T multiples[])
	{
		siz i;
		siz j = poly_size - 1;
		for (i = 0; i < poly_size; i++)
		{
			if (poly[j].y == poly[i].y)
			{
				constants[i] = poly[i].x;
				multiples[i] = 0;
			}
			else
			{
				constants[i] = poly[i].x - (poly[i].y * poly[j].x) / (poly[j].y - poly[i].y) +
					(poly[i].y * poly[i].x) / (poly[j].y - poly[i].y);
				multiples[i] = (poly[j].x - poly[i].x) / (poly[j].y - poly[i].y);
			}
			j = i;
		}
	}

	template <siz POLY_SIZE, typename T>
	void pnpoly_precalc(const con::array<::glm::vec<2, T>, POLY_SIZE>& poly, T constants[], T multiples[])
	{
		pnpoly_precalc<T>(poly.data(), poly.size(), constants, multiples);
	}

	template <typename T>
	bl pnpoly(const ::glm::vec<2, T> poly[], const siz poly_size, const T constants[], const T multiples[],
			  const ::glm::vec<2, T>& point)
	{
		bl oddNodes = false;
		bl current = poly[poly_size - 1].y > point.y;
		bl previous;
		for (siz i = 0; i < poly_size; i++)
		{
			previous = current;
			current = poly[i].y > point.y;
			if (current != previous)
				oddNodes ^= point.y * multiples[i] + constants[i] < point.x;
		}
		return oddNodes;
	}

	template <siz POLY_SIZE, typename T>
	bl pnpoly(const con::array<::glm::vec<2, T>, POLY_SIZE>& poly, const T constants[], const T multiples[],
			  const ::glm::vec<2, T>& point)
	{
		return pnpoly<T>(poly.data(), poly.size(), constants, multiples, point);
	}

	template <siz POLY_SIZE, typename T>
	void pnpoly_precalc(const con::array<::glm::vec<2, T>, POLY_SIZE>& poly, con::array<T, POLY_SIZE>& constants,
						con::array<T, POLY_SIZE>& multiples)
	{
		pnpoly_precalc<T>(poly.data(), poly.size(), constants.data(), multiples.data());
	}

	template <siz POLY_SIZE, typename T>
	bl pnpoly(const con::array<::glm::vec<2, T>, POLY_SIZE>& poly, const con::array<T, POLY_SIZE>& constants,
			  const con::array<T, POLY_SIZE>& multiples, const ::glm::vec<2, T>& point)
	{
		return pnpoly<T>(poly.data(), poly.size(), constants.data(), multiples.data(), point);
	}

	template <typename T>
	void pnpoly_precalc(const ::glm::vec<2, T> poly[], const siz poly_size, ::glm::vec<2, T> precalc[])
	{
		siz i;
		siz j = poly_size - 1;
		for (i = 0; i < poly_size; i++)
		{
			if (poly[j].y == poly[i].y)
			{
				precalc[i].x = poly[i].x;
				precalc[i].y = 0;
			}
			else
			{
				precalc[i].x = poly[i].x - (poly[i].y * poly[j].x) / (poly[j].y - poly[i].y) +
					(poly[i].y * poly[i].x) / (poly[j].y - poly[i].y);
				precalc[i].y = (poly[j].x - poly[i].x) / (poly[j].y - poly[i].y);
			}
			j = i;
		}
	}

	template <siz POLY_SIZE, typename T>
	void pnpoly_precalc(const con::array<::glm::vec<2, T>, POLY_SIZE>& poly, con::array<::glm::vec<2, T>, POLY_SIZE>& precalc)
	{
		pnpoly_precalc<T>(poly.data(), poly.size(), precalc.data());
	}

	template <typename T>
	bl pnpoly(const ::glm::vec<2, T> poly[], const siz poly_size, const ::glm::vec<2, T> precalc[],
			  const ::glm::vec<2, T>& point)
	{
		bl oddNodes = false;
		bl current = poly[poly_size - 1].y > point.y;
		bl previous;
		for (siz i = 0; i < poly_size; i++)
		{
			previous = current;
			current = poly[i].y > point.y;
			if (current != previous)
				oddNodes ^= point.y * precalc[i].y + precalc[i].x < point.x;
		}
		return oddNodes;
	}

	template <siz POLY_SIZE, typename T>
	bl pnpoly(const con::array<::glm::vec<2, T>, POLY_SIZE>& poly, const con::array<::glm::vec<2, T>, POLY_SIZE>& precalc,
			  const ::glm::vec<2, T>& point)
	{
		return pnpoly<T>(poly.data(), poly.size(), precalc.data(), point);
	}
} // namespace np::alg

#endif /* NP_ENGINE_PNPOLY_HPP */
