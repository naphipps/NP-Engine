//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/17/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_DRAWABLE_IMAGE_HPP
#define NP_ENGINE_DRAWABLE_IMAGE_HPP

#include <utility>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Algorithms/Algorithms.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Math/Math.hpp"
#include "NP-Engine/Geometry/Geometry.hpp"
#include "NP-Engine/GPU/GPU.hpp"

#include "ImageSubview.hpp"
#include "FloodFillImage.hpp"

// TODO: support drawing shapes that have specific colors at their points

namespace np::gpualg
{
	class DrawableImage
	{
	public:
		using Point = ImageSubview::Point;
		using OutsidePoint = ImageSubview::OutsidePoint;
		using Line = geom::Line2D<Point::value_type>;
		using Polygon = geom::Polygon2D<Point::value_type>;
		using Circle = geom::Circle<Point::value_type>;

		struct Payload : public FloodFillImage::Payload
		{
			gpu::Color oldColor;
			gpu::Color newColor;
			con::uset<OutsidePoint>* outsideEdgePoints = nullptr;
			con::uset<Point>* edgePoints = nullptr;
		};

	private:
		ImageSubview _image_subview;

		static bl IsOldColor(void* caller, mem::BlDelegate& d)
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

			return payload.imageSubview->Get(point) == payload.oldColor;
		}

		static void SetToNewColor(void* caller, mem::VoidDelegate& d)
		{
			Payload& payload = *d.GetData<Payload*>();
			payload.imageSubview->Set(payload.point, payload.newColor);
		}

		static void GetOutsideEdgePoints(void* caller, mem::VoidDelegate& d)
		{
			using Relation = FloodFillImage::PointRelation;
			Payload& payload = *d.GetData<Payload*>();
			OutsidePoint point = payload.point;

			if (FloodFillImage::PointRelationContains(payload.relation, Relation::Upper))
				point.y++;
			if (FloodFillImage::PointRelationContains(payload.relation, Relation::Lower))
				point.y--;
			if (FloodFillImage::PointRelationContains(payload.relation, Relation::Right))
				point.x++;
			if (FloodFillImage::PointRelationContains(payload.relation, Relation::Left))
				point.x--;

			payload.outsideEdgePoints->emplace(point);
		}

		static void GetEdgePoints(void* caller, mem::VoidDelegate& d)
		{
			Payload& payload = *d.GetData<Payload*>();
			payload.edgePoints->emplace(payload.point);
		}

	public:
		DrawableImage(const ImageSubview& image_subview): _image_subview(image_subview) {}

		DrawableImage(ImageSubview&& image_subview): _image_subview(::std::move(image_subview)) {}

		DrawableImage(gpu::Image& image): _image_subview(image) {}

		DrawableImage(gpu::Image& image, Subview subview): _image_subview(image, subview) {}

		ImageSubview& GetImageSubview()
		{
			return _image_subview;
		}

		const ImageSubview& GetImageSubview() const
		{
			return _image_subview;
		}

		void Draw(const Line& line, gpu::Color color)
		{
			con::vector<Point> points = ::np::alg::GetBresenhamLinePoints<Point::value_type>(line.Begin, line.End);
			for (Point& point : points)
				_image_subview.Set(point, color);
		}

		void Draw(const Polygon& polygon, gpu::Color color)
		{
			con::vector<Point> line;

			for (siz i = 0; i < polygon.Points.size() - 1; i++)
			{
				const Point& begin = polygon.Points[i];
				const Point& end = polygon.Points[i + 1];
				line = ::np::alg::GetBresenhamLinePoints<Point::value_type>(begin, end);

				for (Point& point : line)
					_image_subview.Set(point, color);
			}

			line = ::np::alg::GetBresenhamLinePoints<Point::value_type>(polygon.Points.back(), polygon.Points.front());
			for (Point& point : line)
				_image_subview.Set(point, color);
		}

		void Draw(const Circle& circle, gpu::Color color)
		{
			con::vector<Point> points = ::np::alg::GetMidpointCirclePoints<Point::value_type>(circle.Radius);

			for (Point& point : points)
				_image_subview.Set(point + circle.Center, color);
		}

		void FloodFill(const Point& point, gpu::Color old_color, gpu::Color new_color, bl enable_diagonal = false)
		{
			Payload payload{};
			payload.oldColor = old_color;
			payload.newColor = new_color;
			payload.point = point;
			payload.enableDiagonal = enable_diagonal;

			FloodFillImage flood(_image_subview);
			flood.GetIsApprovedDelegate().SetCallback(IsOldColor);
			flood.GetApprovedActionDelegate().SetCallback(SetToNewColor);
			flood.GetRejectedActionDelegate().UnsetCallback();
			flood.Fill(payload);
		}

		con::uset<OutsidePoint> FloodFillGetOutsideEdgePoints(const Point& point, gpu::Color old_color, gpu::Color new_color,
															  bl enable_diagonal = false)
		{
			con::uset<OutsidePoint> outside_edge_points;

			Payload payload{};
			payload.oldColor = old_color;
			payload.newColor = new_color;
			payload.point = point;
			payload.enableDiagonal = enable_diagonal;
			payload.outsideEdgePoints = &outside_edge_points;

			FloodFillImage flood(_image_subview);
			flood.GetIsApprovedDelegate().SetCallback(IsOldColor);
			flood.GetApprovedActionDelegate().SetCallback(SetToNewColor);
			flood.GetRejectedActionDelegate().SetCallback(GetOutsideEdgePoints);
			flood.Fill(payload);

			return outside_edge_points;
		}

		con::uset<Point> FloodFillGetEdgePoints(const Point& point, gpu::Color old_color, gpu::Color new_color,
												bl enable_diagonal = false)
		{
			con::uset<Point> edge_points;

			Payload payload{};
			payload.oldColor = old_color;
			payload.newColor = new_color;
			payload.point = point;
			payload.enableDiagonal = enable_diagonal;
			payload.edgePoints = &edge_points;

			FloodFillImage flood(_image_subview);
			flood.GetIsApprovedDelegate().SetCallback(IsOldColor);
			flood.GetApprovedActionDelegate().SetCallback(SetToNewColor);
			flood.GetRejectedActionDelegate().SetCallback(GetEdgePoints);
			flood.Fill(payload);

			return edge_points;
		}
	};
} // namespace np::gpualg

#endif /* NP_ENGINE_DRAWABLE_IMAGE_HPP */
