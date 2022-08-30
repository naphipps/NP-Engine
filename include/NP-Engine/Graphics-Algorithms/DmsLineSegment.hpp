//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/17/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_DMS_LINE_SEGMENT_HPP
#define NP_ENGINE_DMS_LINE_SEGMENT_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Math/Math.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/FileSystem/FileSystem.hpp"

#include "NP-Engine/Math/Math.hpp"

#include "ImageSubview.hpp"

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
		DmsLineSegment(const DmsPoint& a, const DmsPoint& b): _a(a), _b(b), _midpoint(mat::Midpoint(_a, _b))
		{}

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
} // namespace np

namespace std
{
	template <>
	struct hash<::np::gfxalg::DmsLineSegment>
	{
		siz operator()(const ::np::gfxalg::DmsLineSegment& line) const noexcept
		{
			union
			{
				siz h;

				struct
				{
					flt x;
					flt y;
				} p;
			};

			p.x = line.Midpoint().x;
			p.y = line.Midpoint().y;
			return h;
		}
	};
}

#endif /* NP_ENGINE_DMS_LINE_SEGMENT_HPP */
