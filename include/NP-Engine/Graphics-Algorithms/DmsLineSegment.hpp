//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/17/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_DMS_LINE_SEGMENT_HPP
#define NP_ENGINE_DMS_LINE_SEGMENT_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Math/Math.hpp"

namespace np::gfxalg
{
	class DmsLineSegment
	{
	public:
		using DmsPoint = ::glm::dvec2;

	private:
		DmsPoint _a;
		DmsPoint _b;
		DmsPoint _midpoint;

	public:
		DmsLineSegment(const DmsPoint& a, const DmsPoint& b): _a(a), _b(b), _midpoint(mat::Midpoint(_a, _b)) {}

		const DmsPoint& Begin() const
		{
			return _a;
		}

		const DmsPoint& End() const
		{
			return _b;
		}

		const DmsPoint& Midpoint() const
		{
			return _midpoint;
		}

		bl operator==(const DmsLineSegment& other) const
		{
			return Begin() == other.Begin() && End() == other.End() && Midpoint() == other.Midpoint();
		}
	};
} // namespace np::gfxalg

namespace std
{
	template <>
	struct hash<::np::gfxalg::DmsLineSegment>
	{
		::np::siz operator()(const ::np::gfxalg::DmsLineSegment& line) const noexcept
		{
			union {
				::np::siz h;

				struct
				{
					::np::flt x;
					::np::flt y;
				} p;
			};

			p = { (::np::flt)line.Midpoint().x, (::np::flt)line.Midpoint().y };
			//TODO: I'd rather combine the hashes from line.Begin() and line.End()
			return h;
		}
	};
} // namespace std

#endif /* NP_ENGINE_DMS_LINE_SEGMENT_HPP */
