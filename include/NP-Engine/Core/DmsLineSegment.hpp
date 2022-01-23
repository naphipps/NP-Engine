//
//  DmsLineSegment.hpp
//  Project Space
//
//  Created by Nathan Phipps on 5/17/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_DMS_LINE_SEGMENT_HPP
#define NP_ENGINE_DMS_LINE_SEGMENT_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Math/Math.hpp"
#include "NP-Engine/Serialization/Serialization.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Filesystem/Filesystem.hpp"

namespace np
{
	namespace core
	{
		/**
		 DmsLineSegment represents the line segments that are produced within DmsImage
		 this contains a begining and ending point, an associated point, and midpoint for the line segment
		 */
		class DmsLineSegment : public serialization::Serializable
		{
		private:
			math::fltPoint _begin;
			math::fltPoint _end;
			math::ui16Point _associated;
			math::fltPoint _midpoint;

		public:
			/**
			 constructor
			 */
			DmsLineSegment(math::fltPoint begin, math::fltPoint end, math::ui16Point associated):
				_begin(end < begin ? end : begin),
				_end(end < begin ? begin : end),
				_associated(associated),
				_midpoint(math::fltPoint{static_cast<flt>((begin.x + end.x) / 2.f), static_cast<flt>((begin.y + end.y) / 2.f)})
			{}

			/**
			 constructor
			 */
			DmsLineSegment(math::fltPoint begin, math::fltPoint end, math::ui16Point associated, math::fltPoint midpoint):
				_begin(end < begin ? end : begin),
				_end(end < begin ? begin : end),
				_associated(associated),
				_midpoint(midpoint)
			{}

			/**
			 deconstructor
			 */
			~DmsLineSegment() {}

			/**
			 gets the begin points
			 */
			inline math::fltPoint Begin() const
			{
				return _begin;
			}

			/**
			 gets the end point
			 */
			inline math::fltPoint End() const
			{
				return _end;
			}

			/**
			 gets the associated point
			 */
			inline math::ui16Point AssociatedPoint() const
			{
				return _associated;
			}

			/**
			 gets the midpoint
			 */
			inline math::fltPoint Midpoint() const
			{
				return _midpoint;
			}

			/**
			 equals operator
			 */
			inline bl operator==(const DmsLineSegment& other) const
			{
				return Begin() == other.Begin() && End() == other.End();
			}

			/**
			 serialization method for us to write
			 we require our objects to know which filepath they are a part of
			 */
			virtual ostrm& Insertion(ostrm& os, str filepath) const override
			{
				// TODO: finish Insertion method
				return os;
			}

			/**
			 deserialization method for us to read
			 we require our objects to know which filepath they are a part of
			 */
			virtual istrm& Extraction(istrm& is, str filepath) override
			{
				// TODO: finish Extraction method
				return is;
			}

			/**
			 save oursellves inside the given dirpath
			 return if the save was successful or not
			 */
			virtual bl SaveTo(str dirpath) const override
			{
				// TODO: finish saveto
				// return Climate::template SaveAs<Climate>(fs::Append(dirpath, AsFilename), this);
				return false;
			}

			/**
			 load outselves from the given dirpath
			 return if the load was successful or not
			 */
			virtual bl LoadFrom(str dirpath) override
			{
				// TODO: finish loadfrom
				// return Climate::template LoadAs<Climate>(fs::Append(dirpath, AsFilename), this);
				return false;
			}
		};

		/**
		 hash for DmsLineSegment
		 */
		struct DmsLineSegmentHash
		{
			ui64 operator()(const DmsLineSegment& line) const noexcept
			{
				flt xf = line.Midpoint().x;
				flt yf = line.Midpoint().y;

				ui32 xi, yi;
				memory::CopyBytes(&xi, &xf, sizeof(flt));
				memory::CopyBytes(&yi, &yf, sizeof(flt));

				return ((ui64)xi << 32) | (ui64)yi;
			}
		};

		using DmsLineSegmentUset = container::uset<DmsLineSegment, DmsLineSegmentHash>;
		using DmsLineSegmentVector = container::vector<DmsLineSegment>;
		using DmsLineSegmentVector2D = container::vector<DmsLineSegmentVector>;
	} // namespace core
} // namespace np

#endif /* NP_ENGINE_DMS_LINE_SEGMENT_HPP */
