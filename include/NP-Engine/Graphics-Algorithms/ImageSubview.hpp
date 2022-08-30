//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/27/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_IMAGE_SUBVIEW_HPP
#define NP_ENGINE_IMAGE_SUBVIEW_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Geometry/Geometry.hpp"
#include "NP-Engine/Graphics/Graphics.hpp"

#include "Subview.hpp"

namespace np::gfxalg
{
	class ImageSubview
	{
	public:
		using Color = gfx::Color;
		using Image = gfx::Image;
		using Point = Image::Point;
		using OutsidePoint = ::glm::vec<2, i64>;
		using Line = geom::Line2D<Point::value_type>;
		using Polygon = geom::Polygon2D<Point::value_type>;
		using Circle = geom::Circle<Point::value_type>;

	private:
		Image& _image;
		Subview _subview;

	public:
		ImageSubview(Image& image): _image(image)
		{
			_subview.Origin = {0, 0};
			_subview.Boundary = {_image.GetWidth(), _image.GetHeight()};
		}

		ImageSubview(Image& image, Subview subview): _image(image), _subview(subview) {}

		void SetSubview(Subview subview)
		{
			NP_ENGINE_ASSERT(subview.Origin.x <= _image.GetWidth(), "subview Origin needs to be within image width");
			NP_ENGINE_ASSERT(subview.Origin.y <= _image.GetHeight(), "subview Origin needs to be within image height");
			NP_ENGINE_ASSERT(subview.Boundary.x <= _image.GetWidth(), "subview Boundary needs to be within image width");
			NP_ENGINE_ASSERT(subview.Boundary.y <= _image.GetHeight(), "subview Boundary needs to be within image height");
			_subview = subview;
		}

		Subview GetSubview() const
		{
			return _subview;
		}

		ui32 GetWidth() const
		{
			return _subview.Boundary.x;
		}

		ui32 GetHeight() const
		{
			return _subview.Boundary.y;
		}

		void Set(const Point& point, Color color)
		{
			Point local = point + _subview.Origin;
			NP_ENGINE_ASSERT(_subview.Contains(local), "our subview needs to contain given point");
			_image.Set(local, color);
		}

		Color Get(const Point& point) const
		{
			Point local = point + _subview.Origin;
			NP_ENGINE_ASSERT(_subview.Contains(local), "our subview needs to contain given point");
			return _image.Get(local);
		}
	};
} // namespace np::gfxalg

#endif /* NP_ENGINE_IMAGE_SUBVIEW_HPP */