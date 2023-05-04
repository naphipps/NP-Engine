//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/17/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_DMS_IMAGE_HPP
#define NP_ENGINE_DMS_IMAGE_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Math/Math.hpp"
#include "NP-Engine/Algorithms/Algorithms.hpp"
#include "NP-Engine/Geometry/Geometry.hpp"
#include "NP-Engine/GPU/GPU.hpp"

#include "ImageSubview.hpp"
#include "DmsLineSegment.hpp"

namespace np::gfxalg
{
	class DmsImage
	{
	public:
		using DmsPoint = DmsLineSegment::DmsPoint;
		using Point = ImageSubview::Point;
		using OutsidePoint = ImageSubview::OutsidePoint;

		using DmsLineSegmentLoop = con::vector<DmsLineSegment>;
		using Extraction = con::vector<con::vector<con::vector<DmsPoint>>>;

		struct Payload : public FloodFillImage::Payload
		{
			dbl isothreshold = 0.5;
			gpu::ColorChannel channel = gpu::ColorChannel::Alpha;
			con::uset<DmsPoint>* edgePoints = nullptr;
			con::uset<DmsPoint>* visited = nullptr;
		};

	private:
		ImageSubview _image_subview;

		static bl IsNotVisitedAndUnderIsothreshold(void* caller, mem::BlDelegate& d)
		{
			using Relation = FloodFillImage::PointRelation;
			Payload& payload = *d.GetData<Payload*>();
			Point point = payload.point;

			if (FloodFillImage::PointRelationContains(payload.relation, Relation::Upper))
				point.y++;
			if (FloodFillImage::PointRelationContains(payload.relation, Relation::Lower))
				point.y--;
			if (FloodFillImage::PointRelationContains(payload.relation, Relation::Right))
				point.x++;
			if (FloodFillImage::PointRelationContains(payload.relation, Relation::Left))
				point.x--;

			return !payload.visited->count(point) &&
				GetIsovalue(*payload.imageSubview, point, payload.channel) < payload.isothreshold;
		}

		static void MarkAsVisited(void* caller, mem::VoidDelegate& d)
		{
			Payload& payload = *d.GetData<Payload*>();
			payload.visited->emplace(payload.point);
		}

		static void ExtractEdgePoint(void* caller, mem::VoidDelegate& d)
		{
			Payload& payload = *d.GetData<Payload*>();
			payload.edgePoints->emplace(payload.point);
		}

		/*
			- gets the dms iso value [0.0 - 1.0] at given point
			- isovalue: fraction of maximum value
				- example: 128 / 255 = 0.501960784313725 isovalue
		*/
		static dbl GetIsovalue(const ImageSubview& image_subview, const Point& point, gpu::ColorChannel channel)
		{
			return ((dbl)image_subview.Get(point).GetChannelValue(channel)) / ((dbl)UINT8_MAX);
		}

		void AddLineSegment(con::uset<DmsLineSegment>& segments, const DmsPoint& a, const DmsPoint& b)
		{
			segments.emplace(mat::RoundTo32nd(a + 0.5), mat::RoundTo32nd(b + 0.5));
		}

		/*
			getting line segments around p
			0 1 2
			3 p 4
			5 6 7
		*/
		void GetLineSegments(con::uset<DmsLineSegment>& segments, dbl isothreshold, gpu::ColorChannel channel,
							 const DmsPoint& p)
		{
			bl has_upper = p.y < _image_subview.GetHeight() - 1;
			bl has_lower = p.y > 0;
			bl has_right = p.x < _image_subview.GetWidth() - 1;
			bl has_left = p.x > 0;
			DmsPoint a, b, c, d;

			// 1, 2, 4, p
			if (has_upper && has_right)
			{
				a = b = c = d = p;
				a.y++;
				b.y++;
				b.x++;
				c.x++;
				GetLineSegments(segments, isothreshold, channel, a, b, c, d);
			}

			// 0, 1, p, 3
			if (has_upper && has_left)
			{
				a = b = c = d = p;
				a.y++;
				a.x--;
				b.y++;
				d.x--;
				GetLineSegments(segments, isothreshold, channel, a, b, c, d);
			}

			// p, 4, 7, 6
			if (has_lower && has_right)
			{
				a = b = c = d = p;
				b.x++;
				c.y--;
				c.x++;
				d.y--;
				GetLineSegments(segments, isothreshold, channel, a, b, c, d);
			}

			// 3, p, 6, 5
			if (has_lower && has_left)
			{
				a = b = c = d = p;
				a.x--;
				c.y--;
				d.y--;
				d.x--;
				GetLineSegments(segments, isothreshold, channel, a, b, c, d);
			}
		}

		/*
			getting line segments of a b c d:
			a b
			d c
		*/
		void GetLineSegments(con::uset<DmsLineSegment>& segments, dbl isothreshold, gpu::ColorChannel channel,
							 const DmsPoint& a, const DmsPoint& b, const DmsPoint& c, const DmsPoint& d)
		{
			dbl a_iso = GetIsovalue(a, channel);
			dbl b_iso = GetIsovalue(b, channel);
			dbl c_iso = GetIsovalue(c, channel);
			dbl d_iso = GetIsovalue(d, channel);

			ui8 segment_type = 0;
			segment_type |= a_iso > isothreshold ? BIT(0) : 0;
			segment_type |= b_iso > isothreshold ? BIT(1) : 0;
			segment_type |= c_iso > isothreshold ? BIT(2) : 0;
			segment_type |= d_iso > isothreshold ? BIT(3) : 0;

			switch (segment_type)
			{
			case 0:
			case 15:
				break;
			case 1:
			case 14:
			{
				dbl ab_t = (isothreshold - a_iso) / (b_iso - a_iso);
				dbl ad_t = (isothreshold - a_iso) / (d_iso - a_iso);
				AddLineSegment(segments, ::glm::lerp(a, b, ab_t), ::glm::lerp(a, d, ad_t));
				break;
			}
			case 2:
			case 13:
			{
				dbl ab_t = (isothreshold - a_iso) / (b_iso - a_iso);
				dbl bc_t = (isothreshold - b_iso) / (c_iso - b_iso);
				AddLineSegment(segments, ::glm::lerp(a, b, ab_t), ::glm::lerp(b, c, bc_t));
				break;
			}
			case 3:
			case 12:
			{
				dbl ad_t = (isothreshold - a_iso) / (d_iso - a_iso);
				dbl bc_t = (isothreshold - b_iso) / (c_iso - b_iso);
				AddLineSegment(segments, ::glm::lerp(a, d, ad_t), ::glm::lerp(b, c, bc_t));
				break;
			}
			case 4:
			case 11:
			{
				dbl bc_t = (isothreshold - b_iso) / (c_iso - b_iso);
				dbl dc_t = (isothreshold - d_iso) / (c_iso - d_iso);
				AddLineSegment(segments, ::glm::lerp(b, c, bc_t), ::glm::lerp(d, c, dc_t));
				break;
			}
			case 5:
			{
				dbl ab_t = (isothreshold - a_iso) / (b_iso - a_iso);
				dbl bc_t = (isothreshold - b_iso) / (c_iso - b_iso);
				dbl dc_t = (isothreshold - d_iso) / (c_iso - d_iso);
				dbl ad_t = (isothreshold - a_iso) / (d_iso - a_iso);
				AddLineSegment(segments, ::glm::lerp(a, d, ad_t), ::glm::lerp(a, b, ab_t));
				AddLineSegment(segments, ::glm::lerp(d, c, dc_t), ::glm::lerp(b, c, bc_t));
				break;
			}
			case 6:
			case 9:
			{
				dbl ab_t = (isothreshold - a_iso) / (b_iso - a_iso);
				dbl dc_t = (isothreshold - d_iso) / (c_iso - d_iso);
				AddLineSegment(segments, ::glm::lerp(a, b, ab_t), ::glm::lerp(d, c, dc_t));
				break;
			}
			case 7:
			case 8:
			{
				dbl ad_t = (isothreshold - a_iso) / (d_iso - a_iso);
				dbl dc_t = (isothreshold - d_iso) / (c_iso - d_iso);
				AddLineSegment(segments, ::glm::lerp(a, d, ad_t), ::glm::lerp(d, c, dc_t));
				break;
			}
			case 10:
			{
				dbl ab_t = (isothreshold - a_iso) / (b_iso - a_iso);
				dbl bc_t = (isothreshold - b_iso) / (c_iso - b_iso);
				dbl dc_t = (isothreshold - d_iso) / (c_iso - d_iso);
				dbl ad_t = (isothreshold - a_iso) / (d_iso - a_iso);
				AddLineSegment(segments, ::glm::lerp(a, b, ab_t), ::glm::lerp(b, c, bc_t));
				AddLineSegment(segments, ::glm::lerp(a, d, ad_t), ::glm::lerp(d, c, dc_t));
				break;
			}
			default:
				NP_ENGINE_ASSERT(false, "unknown dms line segment type");
				break;
			}
		}

	public:
		DmsImage(ImageSubview& image_subview): _image_subview(image_subview) {}

		DmsImage(ImageSubview&& image_subview): _image_subview(::std::move(image_subview)) {}

		DmsImage(gpu::Image& image): _image_subview(image) {}

		DmsImage(gpu::Image& image, Subview subview): _image_subview(image, subview) {}

		dbl GetIsovalue(const Point& point, gpu::ColorChannel channel) const
		{
			return GetIsovalue(_image_subview, point, channel);
		}

		/*
			performs the dual marching squares algorithm
			return object is a vector of shapes with holes
		*/
		Extraction GetExtraction(dbl isothreshold, gpu::ColorChannel channel)
		{
			NP_ENGINE_ASSERT(isothreshold >= 0.0 && isothreshold <= 1.0, "isothreshold must be on range [0, 1]");
			NP_ENGINE_ASSERT(gpu::IsOnlyOneColorChannel(channel), "GetExtraction requires use of only one color channel");

			Extraction extraction;
			ui32 width = _image_subview.GetWidth();
			ui32 height = _image_subview.GetHeight();
			con::uset<DmsPoint> visited;
			con::uset<DmsPoint> edge_points;
			con::uset<DmsLineSegment> segments;
			con::vector<DmsLineSegmentLoop> loops;

			Payload payload{};
			payload.isothreshold = isothreshold;
			payload.channel = channel;
			payload.edgePoints = &edge_points;
			payload.visited = &visited;
			payload.enableDiagonal = true;

			FloodFillImage extract_edge_points_flood(_image_subview);
			extract_edge_points_flood.GetIsApprovedDelegate().SetCallback(IsNotVisitedAndUnderIsothreshold);
			extract_edge_points_flood.GetApprovedActionDelegate().SetCallback(MarkAsVisited);
			extract_edge_points_flood.GetRejectedActionDelegate().SetCallback(ExtractEdgePoint);

			DmsPoint point;
			for (ui32 y = 0; y < height; y++)
			{
				for (ui32 x = 0; x < width; x++)
				{
					point = {x, y};
					if (visited.count(point) || GetIsovalue(point, channel) >= isothreshold)
						continue;

					// extract edge points
					edge_points.clear();
					payload.point = point;
					extract_edge_points_flood.Fill(payload);

					// gather segments from edge points
					segments.clear();
					for (const DmsPoint& edge_point : edge_points)
						GetLineSegments(segments, isothreshold, channel, edge_point);

					// TODO: get segments along the edges of image

					// prime first loop
					// put our first segment into our last loop
					loops.emplace_back();
					loops.back().emplace_back(*segments.begin());
					segments.erase(segments.begin());

					while (segments.size())
					{
						bl found_next = false;
						for (const DmsLineSegment& s : segments)
						{
							if (s.Begin() == loops.back().back().Begin() || s.Begin() == loops.back().back().End() ||
								s.End() == loops.back().back().Begin() || s.End() == loops.back().back().End())
							{
								// seg matches up on back's begin or end
								loops.back().emplace_back(s);
								segments.erase(s);
								found_next = true;
								break;
							}
						}

						if (!found_next || segments.empty())
						{
							// does the segment at the end of our loop match with the segment at the front of the loop?
							if (loops.back().back().Begin() == loops.back().front().Begin() ||
								loops.back().back().Begin() == loops.back().front().End() ||
								loops.back().back().End() == loops.back().front().Begin() ||
								loops.back().back().End() == loops.back().front().End())
							{
								// can we do another loop?
								if (segments.size())
								{
									// prime next loop
									loops.emplace_back();
									loops.back().emplace_back(*segments.begin());
									segments.erase(segments.begin());
								}
							}
						}
					}

					// sort our loops
					::std::sort(loops.begin(), loops.end(),
								[](const con::vector<DmsLineSegment>& a, const con::vector<DmsLineSegment>& b) -> bl
								{
									return a.size() > b.size();
								});

					// get the midpoints of all line segments
					// organize them into a vector of shapes with holes
					// a shape with holes is a vector of polygons (first one being the largest polygon)
					// a polygon is loop of midpoints

					extraction.emplace_back();
					for (DmsLineSegmentLoop& loop : loops)
					{
						extraction.back().emplace_back();
						for (DmsLineSegment& segment : loop)
							extraction.back().back().emplace_back(segment.Midpoint());
					}

					::std::sort(extraction.back().begin(), extraction.back().end(),
								[](const con::vector<DmsPoint>& a, const con::vector<DmsPoint>& b)
								{
									return a.size() > b.size();
								});

					if (extraction.back().back().empty())
						extraction.back().erase(extraction.back().end() - 1);

					if (extraction.back().empty())
						extraction.erase(extraction.end() - 1);
				}
			}

			return extraction;
		}

		con::uset<DmsLineSegment> MarchCubesStride2(dbl isothreshold, gpu::ColorChannel channel)
		{
			NP_ENGINE_ASSERT(isothreshold >= 0.0 && isothreshold <= 1.0, "isothreshold must be on range [0, 1]");
			NP_ENGINE_ASSERT(gpu::IsOnlyOneColorChannel(channel), "GetExtraction requires use of only one color channel");

			ui32 width = _image_subview.GetWidth();
			ui32 height = _image_subview.GetHeight();
			bl width_is_even = !(width & 1);
			bl height_is_even = !(height & 1);
			con::uset<DmsLineSegment> segments;

			for (ui32 y = 0; y < height; y += 2)
				for (ui32 x = 0; x < width; x += 2)
					GetLineSegments(segments, isothreshold, channel, {x, y});

			if (width_is_even)
				for (ui32 y = 0; y < height; y += 2)
					GetLineSegments(segments, isothreshold, channel, {width - 1, y});

			if (height_is_even)
				for (ui32 x = 0; x < width; x += 2)
					GetLineSegments(segments, isothreshold, channel, {x, height - 1});

			if (width_is_even && height_is_even)
				GetLineSegments(segments, isothreshold, channel, {width - 1, height - 1});

			return segments;
		}

		con::uset<DmsLineSegment> MarchCubesFlood(dbl isothreshold, gpu::ColorChannel channel)
		{
			NP_ENGINE_ASSERT(isothreshold >= 0.0 && isothreshold <= 1.0, "isothreshold must be on range [0, 1]");
			NP_ENGINE_ASSERT(gpu::IsOnlyOneColorChannel(channel), "GetExtraction requires use of only one color channel");

			con::uset<DmsPoint> visited;
			con::uset<DmsPoint> edge_points;
			con::uset<DmsLineSegment> segments;

			Payload payload{};
			payload.isothreshold = isothreshold;
			payload.channel = channel;
			payload.edgePoints = &edge_points;
			payload.visited = &visited;
			payload.enableDiagonal = true;

			FloodFillImage extract_edge_points_flood(_image_subview);
			extract_edge_points_flood.GetIsApprovedDelegate().SetCallback(IsNotVisitedAndUnderIsothreshold);
			extract_edge_points_flood.GetApprovedActionDelegate().SetCallback(MarkAsVisited);
			extract_edge_points_flood.GetRejectedActionDelegate().SetCallback(ExtractEdgePoint);

			ui32 width = _image_subview.GetWidth();
			ui32 height = _image_subview.GetHeight();
			DmsPoint point;
			for (ui32 y = 0; y < height; y++)
			{
				for (ui32 x = 0; x < width; x++)
				{
					point = {x, y};
					if (visited.count(point) || GetIsovalue(point, channel) >= isothreshold)
						continue;

					// extract edge points
					edge_points.clear();
					payload.point = point;
					extract_edge_points_flood.Fill(payload);

					// gather segments from edge points
					for (const DmsPoint& edge_point : edge_points)
						GetLineSegments(segments, isothreshold, channel, edge_point);
				}
			}

			return segments;
		}

		Extraction GetExtraction(con::uset<DmsLineSegment>& segments)
		{
			return {}; // TODO: I figure we can make this an addition step?
		}
	};
} // namespace np::gfxalg

#endif /* NP_ENGINE_DMS_IMAGE_HPP */
