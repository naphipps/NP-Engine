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
#include "NP-Engine/Graphics/Graphics.hpp"

#include "ImageSubview.hpp"
#include "FloodFillImage.hpp"

// TODO: support drawing shapes that have specific colors at their points

namespace np::gfxalg
{
	class DrawableImage
	{
	public:
		using Point = ImageSubview::Point;
		using OutsidePoint = ImageSubview::OutsidePoint;
		using Line = geom::Line2D<Point::value_type>;
		using Polygon = geom::Polygon2D<Point::value_type>;
		using Circle = geom::Circle<Point::value_type>;

	private:
		ImageSubview _image_subview;

		struct FloodFillUserData
		{
			DrawableImage* self = nullptr;
			gfx::Color old_color;
			gfx::Color new_color;
			con::uset<OutsidePoint>* outside_edge_points = nullptr;
			con::uset<Point>* edge_points = nullptr;
		};

		static bl IsOldColor(mem::BlDelegate& d)
		{
			using Relation = FloodFillImage::PointRelation;
			FloodFillImage::Payload& payload = *d.GetData<FloodFillImage::Payload*>();
			FloodFillUserData& user_data = *((FloodFillUserData*)payload.user_data);
			Point point = payload.point;

			if (FloodFillImage::PointRelationContains(payload.relation, Relation::Upper))
				point.y++;
			if (FloodFillImage::PointRelationContains(payload.relation, Relation::Lower))
				point.y--;
			if (FloodFillImage::PointRelationContains(payload.relation, Relation::Right))
				point.x++;
			if (FloodFillImage::PointRelationContains(payload.relation, Relation::Left))
				point.x--;

			return user_data.self->_image_subview.Get(point) == user_data.old_color;
		}

		static void SetToNewColor(mem::VoidDelegate& d)
		{
			FloodFillImage::Payload& payload = *d.GetData<FloodFillImage::Payload*>();
			FloodFillUserData& user_data = *((FloodFillUserData*)payload.user_data);
			user_data.self->_image_subview.Set(payload.point, user_data.new_color);
		}

		static void GetOutsideEdgePoints(mem::VoidDelegate& d)
		{
			using Relation = FloodFillImage::PointRelation;
			FloodFillImage::Payload& payload = *d.GetData<FloodFillImage::Payload*>();
			FloodFillUserData& user_data = *((FloodFillUserData*)payload.user_data);
			OutsidePoint point = payload.point;

			if (FloodFillImage::PointRelationContains(payload.relation, Relation::Upper))
				point.y++;
			if (FloodFillImage::PointRelationContains(payload.relation, Relation::Lower))
				point.y--;
			if (FloodFillImage::PointRelationContains(payload.relation, Relation::Right))
				point.x++;
			if (FloodFillImage::PointRelationContains(payload.relation, Relation::Left))
				point.x--;

			user_data.outside_edge_points->emplace(point);
		}

		static void GetEdgePoints(mem::VoidDelegate& d)
		{
			FloodFillImage::Payload& payload = *d.GetData<FloodFillImage::Payload*>();
			FloodFillUserData& user_data = *((FloodFillUserData*)payload.user_data);
			user_data.edge_points->emplace(payload.point);
		}

	public:
		DrawableImage(const ImageSubview& image_subview): _image_subview(image_subview) {}

		DrawableImage(ImageSubview&& image_subview): _image_subview(::std::move(image_subview)) {}

		DrawableImage(gfx::Image& image): _image_subview(image) {}

		DrawableImage(gfx::Image& image, Subview subview): _image_subview(image, subview) {}

		ImageSubview& GetImageSubview()
		{
			return _image_subview;
		}

		const ImageSubview& GetImageSubview() const
		{
			return _image_subview;
		}

		void Draw(const Line& line, gfx::Color color)
		{
			con::vector<Point> points = ::np::alg::GetBresenhamLinePoints<Point::value_type>(line.Begin, line.End);
			for (Point& point : points)
				_image_subview.Set(point, color);
		}

		void Draw(const Polygon& polygon, gfx::Color color)
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

		void Draw(const Circle& circle, gfx::Color color)
		{
			con::vector<Point> points = ::np::alg::GetMidpointCirclePoints<Point::value_type>(circle.Radius);

			for (Point& point : points)
				_image_subview.Set(point + circle.Center, color);
		}

		void FloodFill(const Point& point, gfx::Color old_color, gfx::Color new_color, bl enable_diagonal = false)
		{
			FloodFillUserData user_data{};
			user_data.self = this;
			user_data.old_color = old_color;
			user_data.new_color = new_color;

			FloodFillImage::Payload payload{};
			payload.user_data = &user_data;
			payload.point = point;
			payload.enable_diagonal = enable_diagonal;

			FloodFillImage flood(_image_subview);
			flood.GetIsApprovedDelegate().Connect<&DrawableImage::IsOldColor>();
			flood.GetApprovedActionDelegate().Connect<&DrawableImage::SetToNewColor>();
			flood.GetRejectedActionDelegate().DisconnectFunction();
			flood.Fill(payload);
		}

		con::uset<OutsidePoint> FloodFillGetOutsideEdgePoints(const Point& point, gfx::Color old_color, gfx::Color new_color,
															  bl enable_diagonal = false)
		{
			con::uset<OutsidePoint> outside_edge_points;

			FloodFillUserData user_data;
			user_data.self = this;
			user_data.old_color = old_color;
			user_data.new_color = new_color;
			user_data.outside_edge_points = &outside_edge_points;

			FloodFillImage::Payload payload;
			payload.user_data = &user_data;
			payload.point = point;
			payload.enable_diagonal = enable_diagonal;

			FloodFillImage flood(_image_subview);
			flood.GetIsApprovedDelegate().Connect<&DrawableImage::IsOldColor>();
			flood.GetApprovedActionDelegate().Connect<&DrawableImage::SetToNewColor>();
			flood.GetRejectedActionDelegate().Connect<&DrawableImage::GetOutsideEdgePoints>();
			flood.Fill(payload);

			return outside_edge_points;
		}

		con::uset<Point> FloodFillGetEdgePoints(const Point& point, gfx::Color old_color, gfx::Color new_color,
												bl enable_diagonal = false)
		{
			con::uset<Point> edge_points;

			FloodFillUserData user_data;
			user_data.self = this;
			user_data.old_color = old_color;
			user_data.new_color = new_color;
			user_data.edge_points = &edge_points;

			FloodFillImage::Payload payload;
			payload.enable_diagonal = enable_diagonal;
			payload.point = point;
			payload.user_data = &user_data;

			FloodFillImage flood(_image_subview);
			flood.GetIsApprovedDelegate().Connect<&DrawableImage::IsOldColor>();
			flood.GetApprovedActionDelegate().Connect<&DrawableImage::SetToNewColor>();
			flood.GetRejectedActionDelegate().Connect<&DrawableImage::GetEdgePoints>();
			flood.Fill(payload);

			return edge_points;
		}
	};
} // namespace np::gfxalg

#endif /* NP_ENGINE_DRAWABLE_IMAGE_HPP */