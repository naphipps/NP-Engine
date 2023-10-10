//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/27/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_FLOOD_FILL_IMAGE_HPP
#define NP_ENGINE_FLOOD_FILL_IMAGE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "ImageSubview.hpp"

namespace np::alg
{
	class FloodFillImage
	{
	public:
		using Point = ImageSubview::Point;

		enum class PointRelation : ui32
		{
			None = 0,
			Upper = BIT(0),
			Lower = BIT(1),
			Right = BIT(2),
			Left = BIT(3),
			UpperRight = Upper | Right,
			UpperLeft = Upper | Left,
			LowerRight = Lower | Right,
			LowerLeft = Lower | Left
		};

		static bl PointRelationContains(PointRelation a, PointRelation b)
		{
			return ((ui32)a & (ui32)b) != (ui32)PointRelation::None;
		}

		struct Payload
		{
			ImageSubview* imageSubview = nullptr;
			Point point;
			PointRelation relation = PointRelation::None;
			bl enableDiagonal = false;
		};

	private:
		ImageSubview _image_subview;
		mem::BlDelegate _is_approved;
		mem::VoidDelegate _approved_action;
		mem::VoidDelegate _rejected_action;

	public:
		FloodFillImage(const ImageSubview& image_subview): _image_subview(image_subview) {}

		FloodFillImage(ImageSubview&& image_subview): _image_subview(::std::move(image_subview)) {}

		FloodFillImage(gpu::Image& image): _image_subview(image) {}

		FloodFillImage(gpu::Image& image, Subview subview): _image_subview(image, subview) {}

		virtual ~FloodFillImage() = default;

		mem::BlDelegate& GetIsApprovedDelegate()
		{
			return _is_approved;
		}

		const mem::BlDelegate& GetIsApprovedDelegate() const
		{
			return _is_approved;
		}

		mem::VoidDelegate& GetApprovedActionDelegate()
		{
			return _approved_action;
		}

		const mem::VoidDelegate& GetApprovedActionDelegate() const
		{
			return _approved_action;
		}

		mem::VoidDelegate& GetRejectedActionDelegate()
		{
			return _rejected_action;
		}

		const mem::VoidDelegate& GetRejectedActionDelegate() const
		{
			return _rejected_action;
		}

		void Fill(Payload& payload)
		{
			ImageSubview* prev_image_subview = payload.imageSubview;
			if (!prev_image_subview)
				payload.imageSubview = mem::AddressOf(_image_subview);

			payload.relation = PointRelation::None;

			_is_approved.ConstructData<Payload*>(mem::AddressOf(payload));
			_approved_action.ConstructData<Payload*>(mem::AddressOf(payload));
			_rejected_action.ConstructData<Payload*>(mem::AddressOf(payload));

			ui32 height = _image_subview.GetHeight();
			ui32 width = _image_subview.GetWidth();
			con::queue<Point> flood;

			if (_is_approved())
				flood.emplace(payload.point);
			else
				_rejected_action();

			while (!flood.empty())
			{
				payload.point = flood.front();
				payload.relation = PointRelation::None;
				_approved_action();

				payload.relation = PointRelation::Upper;
				if (payload.point.y < height)
				{
					if (_is_approved())
						flood.emplace(payload.point.x, payload.point.y + 1);
					else
						_rejected_action();
				}
				else
				{
					_rejected_action();
				}

				payload.relation = PointRelation::Lower;
				if (payload.point.y >= 0)
				{
					if (_is_approved())
						flood.emplace(payload.point.x, payload.point.y - 1);
					else
						_rejected_action();
				}
				else
				{
					_rejected_action();
				}

				payload.relation = PointRelation::Right;
				if (payload.point.x < width)
				{
					if (_is_approved())
						flood.emplace(payload.point.x + 1, payload.point.y);
					else
						_rejected_action();
				}
				else
				{
					_rejected_action();
				}

				payload.relation = PointRelation::Left;
				if (payload.point.x >= 0)
				{
					if (_is_approved())
						flood.emplace(payload.point.x - 1, payload.point.y);
					else
						_rejected_action();
				}
				else
				{
					_rejected_action();
				}

				if (payload.enableDiagonal)
				{
					payload.relation = PointRelation::UpperRight;
					if (payload.point.x < width && payload.point.y < height)
					{
						if (_is_approved())
							flood.emplace(payload.point.x + 1, payload.point.y + 1);
						else
							_rejected_action();
					}
					else
					{
						_rejected_action();
					}

					payload.relation = PointRelation::UpperLeft;
					if (payload.point.x >= 0 && payload.point.y < height)
					{
						if (_is_approved())
							flood.emplace(payload.point.x - 1, payload.point.y + 1);
						else
							_rejected_action();
					}
					else
					{
						_rejected_action();
					}

					payload.relation = PointRelation::LowerRight;
					if (payload.point.x < width && payload.point.y >= 0)
					{
						if (_is_approved())
							flood.emplace(payload.point.x + 1, payload.point.y - 1);
						else
							_rejected_action();
					}
					else
					{
						_rejected_action();
					}

					payload.relation = PointRelation::LowerLeft;
					if (payload.point.x >= 0 && payload.point.y >= 0)
					{
						if (_is_approved())
							flood.emplace(payload.point.x - 1, payload.point.y - 1);
						else
							_rejected_action();
					}
					else
					{
						_rejected_action();
					}
				}

				flood.pop();
			}

			_is_approved.DestructData<Payload*>();
			_approved_action.DestructData<Payload*>();
			_rejected_action.DestructData<Payload*>();

			payload.imageSubview = prev_image_subview;
		}
	};
} // namespace np::alg

#endif /* NP_ENGINE_FLOOD_FILL_IMAGE_HPP */