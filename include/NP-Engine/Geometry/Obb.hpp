//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/10/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_OBB_HPP
#define NP_ENGINE_OBB_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Math/Math.hpp"

#include "Aabb.hpp"

namespace np::geom
{
	template <siz DIMENSION_COUNT, typename T>
	struct Obb;

	template <typename T>
	struct Obb<2, T> : public Aabb<2, T>
	{
		using Point = typename Aabb<2, T>::Point;
		::glm::quat orientation{0.f, 0.f, 0.f, 1.f}; //default wxyz {0, 0, 0, 1}

		virtual Point ApplyOrientation(const Point& point) const
		{
			Point pt = point - center;
			::glm::vec4 t = { (flt)pt.x, (flt)pt.y, 0.f, 1.f };
			t = ::glm::toMat4(orientation) * t;
			pt = { (T)t.x, (T)t.y };
			return pt + center;
		}

		virtual bl Contains(const Point& point) const override
		{
			return Aabb<2, T>::Contains(ApplyOrientation(point));
		}

		virtual Aabb<2, T> GetAabb() const
		{
			Point a, b, c, d;
			a = b = c = d = halfLengths;

			b.x *= -1;
			c.x *= -1;
			c.y *= -1;
			d.y *= -1;

			a = ApplyOrientation(a + center);
			b = ApplyOrientation(b + center);
			c = ApplyOrientation(c + center);
			d = ApplyOrientation(d + center);

			Point min, max = {};
			min.x = ::std::min({ a.x, b.x, c.x, d.x });
			min.y = ::std::min({ a.y, b.y, c.y, d.y });
			max.x = ::std::max({ a.x, b.x, c.x, d.x });
			max.y = ::std::max({ a.y, b.y, c.y, d.y });

			Aabb<2, T> aabb{};
			aabb.center = center;
			aabb.halfLengths = max - min;
			return aabb;
		}
	};

	template <typename T>
	struct Obb<3, T> : public Aabb<3, T>
	{
		using Point = typename Aabb<3, T>::Point;
		::glm::quat orientation{0.f, 0.f, 0.f, 1.f}; //default wxyz {0, 0, 0, 1}

		virtual Point ApplyOrientation(const Point& point) const
		{
			Point pt = point - center;
			::glm::vec4 t = { (flt)pt.x, (flt)pt.y, (flt)pt.z, 1.f };
			t = ::glm::toMat4(orientation) * t;
			pt = { (T)t.x, (T)t.y, (T)t.z };
			return pt + center;
		}

		virtual bl Contains(const Point& point) const override
		{
			return Aabb<3, T>::Contains(ApplyOrientation(point));
		}

		virtual Aabb<3, T> GetAabb() const
		{
			Point a, b, c, d, e, f, g, h;
			a = b = c = d = e = f = g = h = halfLengths;
			
			b.x *= -1;
			c.x *= -1;
			c.y *= -1;
			d.y *= -1;

			e = a;
			f = b;
			g = c;
			h = d;

			e.z *= -1;
			f.z *= -1;
			g.z *= -1;
			h.z *= -1;

			a = ApplyOrientation(a + center);
			b = ApplyOrientation(b + center);
			c = ApplyOrientation(c + center);
			d = ApplyOrientation(d + center);
			e = ApplyOrientation(e + center);
			f = ApplyOrientation(f + center);
			g = ApplyOrientation(g + center);
			h = ApplyOrientation(h + center);

			Point min, max = {};
			min.x = ::std::min({ a.x, b.x, c.x, d.x, e.x, f.x, g.x, h.x });
			min.y = ::std::min({ a.y, b.y, c.y, d.y, e.y, f.y, g.y, h.y });
			min.z = ::std::min({ a.z, b.z, c.z, d.z, e.z, f.z, g.z, h.z });
			max.x = ::std::max({ a.x, b.x, c.x, d.x, e.x, f.x, g.x, h.x });
			max.y = ::std::max({ a.y, b.y, c.y, d.y, e.y, f.y, g.y, h.y });
			max.z = ::std::max({ a.z, b.z, c.z, d.z, e.z, f.z, g.z, h.z });

			Aabb<3, T> aabb{};
			aabb.center = center;
			aabb.halfLengths = max - min;
			return aabb;
		}
	};

	using Ui8Obb2D = Obb<2, ui8>;
	using Ui16Obb2D = Obb<2, ui16>;
	using Ui32Obb2D = Obb<2, ui32>;
	using Ui64Obb2D = Obb<2, ui64>;
	using I8Obb2D = Obb<2, i8>;
	using I16Obb2D = Obb<2, i16>;
	using I32Obb2D = Obb<2, i32>;
	using I64Obb2D = Obb<2, i64>;
	using SizObb2D = Obb<2, siz>;
	using FltObb2D = Obb<2, flt>;
	using DblObb2D = Obb<2, dbl>;
	using LdblObb2D = Obb<2, ldbl>;

	using Ui8Obb3D = Obb<3, ui8>;
	using Ui16Obb3D = Obb<3, ui16>;
	using Ui32Obb3D = Obb<3, ui32>;
	using Ui64Obb3D = Obb<3, ui64>;
	using I8Obb3D = Obb<3, i8>;
	using I16Obb3D = Obb<3, i16>;
	using I32Obb3D = Obb<3, i32>;
	using I64Obb3D = Obb<3, i64>;
	using SizObb3D = Obb<3, siz>;
	using FltObb3D = Obb<3, flt>;
	using DblObb3D = Obb<3, dbl>;
	using LdblObb3D = Obb<3, ldbl>;
} // namespace np::geom

#endif /* NP_ENGINE_OBB_HPP */