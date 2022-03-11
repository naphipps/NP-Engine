//
//  Point.hpp
//  Project Space
//
//  Created by Nathan Phipps on 12/27/19.
//  Copyright © 2019 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_POINT_HPP
#define NP_ENGINE_POINT_HPP

#include <type_traits>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Memory/Memory.hpp"

namespace np
{
	namespace math
	{
		template <typename T>
		struct Point // TODO: investigate if we should use glm::vec2 instead of this!! Yes. Yes we should.
		{
			NP_ENGINE_STATIC_ASSERT((::std::is_same_v<T, ui8> || ::std::is_same_v<T, ui16> || ::std::is_same_v<T, ui32> ||
									 ::std::is_same_v<T, ui64> || ::std::is_same_v<T, i8> || ::std::is_same_v<T, i16> ||
									 ::std::is_same_v<T, i32> || ::std::is_same_v<T, i64> || ::std::is_same_v<T, flt> ||
									 ::std::is_same_v<T, dbl>),
									"Point<T> requires T to be types listed in Point.hpp");

			T x = 0;
			T y = 0;

			inline bl operator==(const Point<T>& other) const
			{
				return x == other.x && y == other.y;
			}

			inline bl operator!=(const Point<T>& other) const
			{
				return !(*this == other);
			}

			inline bl operator<(const Point<T>& other) const
			{
				return x != other.x ? x < other.x : y < other.y;
			}

			inline bl operator>(const Point<T>& other) const
			{
				return x != other.x ? x > other.x : y > other.y;
			}

			inline bl operator<=(const Point<T>& other) const
			{
				return !(*this > other);
			}

			inline bl operator>=(const Point<T>& other) const
			{
				return !(*this < other);
			}

			template <typename Y>
			inline Point<T>& operator=(const Point<Y>& other)
			{
				x = (T)other.x;
				y = (T)other.y;
				return *this;
			}

			template <typename Y>
			inline Point<T>& operator=(Point<Y>&& other)
			{
				x = (T)other.x;
				y = (T)other.y;
				return *this;
			}

			template <typename Y>
			inline Point<T> operator+(const Point<Y>& other)
			{
				Point<T> p(*this);
				p.x += (T)other.x;
				p.y += (T)other.y;
				return p;
			}

			template <typename Y>
			inline Point<T> operator-(const Point<Y>& other)
			{
				Point<T> p(*this);
				p.x -= (T)other.x;
				p.y -= (T)other.y;
				return p;
			}

			template <typename Y>
			inline Point<T> operator*(const Point<Y>& other)
			{
				Point<T> p(*this);
				p.x *= (T)other.x;
				p.y *= (T)other.y;
				return p;
			}

			template <typename Y>
			inline Point<T> operator/(const Point<Y>& other)
			{
				Point<T> p(*this);
				p.x /= (T)other.x;
				p.y /= (T)other.y;
				return p;
			}

			template <typename Y>
			inline Point<T> operator+(const Y& t)
			{
				Point<T> p(*this);
				p.x += (T)t;
				p.y += (T)t;
				return p;
			}

			template <typename Y>
			inline Point<T> operator-(const Y& t)
			{
				Point<T> p(*this);
				p.x -= (T)t;
				p.y -= (T)t;
				return p;
			}

			template <typename Y>
			inline Point<T> operator*(const Y& t)
			{
				Point<T> p(*this);
				p.x *= (T)t;
				p.y *= (T)t;
				return p;
			}

			template <typename Y>
			inline Point<T> operator/(const Y& t)
			{
				Point<T> p(*this);
				p.x /= (T)t;
				p.y /= (T)t;
				return p;
			}

			template <typename Y>
			inline Point<T>& operator+=(const Point<Y>& other)
			{
				x += (T)other.x;
				y += (T)other.y;
				return *this;
			}

			template <typename Y>
			inline Point<T>& operator-=(const Point<Y>& other)
			{
				x -= (T)other.x;
				y -= (T)other.y;
				return *this;
			}

			template <typename Y>
			inline Point<T>& operator*=(const Point<Y>& other)
			{
				x *= (T)other.x;
				y *= (T)other.y;
				return *this;
			}

			template <typename Y>
			inline Point<T>& operator/=(const Point<Y>& other)
			{
				x /= (T)other.x;
				y /= (T)other.y;
				return *this;
			}

			template <typename Y>
			inline Point<T>& operator+=(const Y& t)
			{
				x += (T)t;
				y += (T)t;
				return *this;
			}

			template <typename Y>
			inline Point<T>& operator-=(const Y& t)
			{
				x -= (T)t;
				y -= (T)t;
				return *this;
			}

			template <typename Y>
			inline Point<T>& operator*=(const Y& t)
			{
				x *= (T)t;
				y *= (T)t;
				return *this;
			}

			template <typename Y>
			inline Point<T>& operator/=(const Y& t)
			{
				x /= (T)t;
				y /= (T)t;
				return *this;
			}
		};

		template <typename T>
		using PointQueue = container::queue<Point<T>>;
		template <typename T>
		using PointUset = container::uset<Point<T>>;
		template <typename T>
		using PointVector = container::vector<Point<T>>;
		template <typename T, siz SIZE>
		using PointArray = container::array<Point<T>, SIZE>;
		template <typename T>
		using PointVector2D = container::vector<PointVector<T>>;
		template <typename T>
		using PointVector3D = container::vector<PointVector2D<T>>;

		using ui8Point = Point<ui8>;
		using ui16Point = Point<ui16>;
		using ui32Point = Point<ui32>;
		using ui64Point = Point<ui64>;
		using i8Point = Point<i8>;
		using i16Point = Point<i16>;
		using i32Point = Point<i32>;
		using i64Point = Point<i64>;
		using fltPoint = Point<flt>;
		using dblPoint = Point<dbl>;

		using ui8PointQueue = PointQueue<ui8>;
		using ui16PointQueue = PointQueue<ui16>;
		using ui32PointQueue = PointQueue<ui32>;
		using ui64PointQueue = PointQueue<ui64>;
		using i8PointQueue = PointQueue<i8>;
		using i16PointQueue = PointQueue<i16>;
		using i32PointQueue = PointQueue<i32>;
		using i64PointQueue = PointQueue<i64>;
		using fltPointQueue = PointQueue<flt>;
		using dblPointQueue = PointQueue<dbl>;

		using ui8PointUset = PointUset<ui8>;
		using ui16PointUset = PointUset<ui16>;
		using ui32PointUset = PointUset<ui32>;
		// using ui64PointUset = PointUset<ui64>; //we do not have a good enough hash at this time
		using i8PointUset = PointUset<i8>;
		using i16PointUset = PointUset<i16>;
		using i32PointUset = PointUset<i32>;
		// using i64PointUset = PointUset<i64>; //we do not have a good enough hash at this time
		using fltPointUset = PointUset<flt>;
		// using dblPointUset = PointUset<dbl>; //we do not have a good enough hash at this time

		template <siz SIZE>
		using ui8PointArray = PointArray<ui8, SIZE>;
		template <siz SIZE>
		using ui16PointArray = PointArray<ui16, SIZE>;
		template <siz SIZE>
		using ui32PointArray = PointArray<ui32, SIZE>;
		template <siz SIZE>
		using ui64PointArray = PointArray<ui64, SIZE>;
		template <siz SIZE>
		using i8PointArray = PointArray<i8, SIZE>;
		template <siz SIZE>
		using i16PointArray = PointArray<i16, SIZE>;
		template <siz SIZE>
		using i32PointArray = PointArray<i32, SIZE>;
		template <siz SIZE>
		using i64PointArray = PointArray<i64, SIZE>;
		template <siz SIZE>
		using fltPointArray = PointArray<flt, SIZE>;
		template <siz SIZE>
		using dblPointArray = PointArray<dbl, SIZE>;

		using ui8PointVector = PointVector<ui8>;
		using ui16PointVector = PointVector<ui16>;
		using ui32PointVector = PointVector<ui32>;
		using ui64PointVector = PointVector<ui64>;
		using i8PointVector = PointVector<i8>;
		using i16PointVector = PointVector<i16>;
		using i32PointVector = PointVector<i32>;
		using i64PointVector = PointVector<i64>;
		using fltPointVector = PointVector<flt>;
		using dblPointVector = PointVector<dbl>;

		using ui8PointVector2D = PointVector2D<ui8>;
		using ui16PointVector2D = PointVector2D<ui16>;
		using ui32PointVector2D = PointVector2D<ui32>;
		using ui64PointVector2D = PointVector2D<ui64>;
		using i8PointVector2D = PointVector2D<i8>;
		using i16PointVector2D = PointVector2D<i16>;
		using i32PointVector2D = PointVector2D<i32>;
		using i64PointVector2D = PointVector2D<i64>;
		using fltPointVector2D = PointVector2D<flt>;
		using dblPointVector2D = PointVector2D<dbl>;

		using ui8PointVector3D = PointVector3D<ui8>;
		using ui16PointVector3D = PointVector3D<ui16>;
		using ui32PointVector3D = PointVector3D<ui32>;
		using ui64PointVector3D = PointVector3D<ui64>;
		using i8PointVector3D = PointVector3D<i8>;
		using i16PointVector3D = PointVector3D<i16>;
		using i32PointVector3D = PointVector3D<i32>;
		using i64PointVector3D = PointVector3D<i64>;
		using fltPointVector3D = PointVector3D<flt>;
		using dblPointVector3D = PointVector3D<dbl>;
	} // namespace math
} // namespace np

namespace std
{
	// these hashes allow for uset support

	template <>
	struct hash<::np::math::ui8Point>
	{
		ui64 operator()(const ::np::math::ui8Point& p) const noexcept
		{
			return ((ui64)p.x) << 32 | (ui64)p.y;
		}
	};

	template <>
	struct hash<::np::math::ui16Point>
	{
		ui64 operator()(const ::np::math::ui16Point& p) const noexcept
		{
			return ((ui64)p.x) << 32 | (ui64)p.y;
		}
	};

	template <>
	struct hash<::np::math::ui32Point>
	{
		ui64 operator()(const ::np::math::ui32Point& p) const noexcept
		{
			return ((ui64)p.x) << 32 | (ui64)p.y;
		}
	};

	template <>
	struct hash<::np::math::i8Point>
	{
		ui64 operator()(const ::np::math::i8Point& p) const noexcept
		{
			return ((ui64)p.x) << 32 | (ui64)p.y;
		}
	};

	template <>
	struct hash<::np::math::i16Point>
	{
		ui64 operator()(const ::np::math::i16Point& p) const noexcept
		{
			return ((ui64)p.x) << 32 | (ui64)p.y;
		}
	};

	template <>
	struct hash<::np::math::i32Point>
	{
		ui64 operator()(const ::np::math::i32Point& p) const noexcept
		{
			return ((ui64)p.x) << 32 | (ui64)p.y;
		}
	};

	template <>
	struct hash<::np::math::fltPoint>
	{
		ui64 operator()(const ::np::math::fltPoint& p) const noexcept
		{
			ui32 x, y;
			::np::memory::CopyBytes(&x, &p.x, sizeof(flt));
			::np::memory::CopyBytes(&y, &p.y, sizeof(flt));

			return ((ui64)x) << 32 | (ui64)y;
		}
	};
} // namespace std

#endif /* NP_ENGINE_POINT_HPP */
