//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/27/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_FLOOD_FILL_IMAGE_HPP
#define NP_ENGINE_GPU_INTERFACE_FLOOD_FILL_IMAGE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "ImageSubview.hpp"

namespace np::alg
{
	class FloodFillImage
	{
	public:
		using Point = ImageSubview::Point;

		class PointRelation : public enm_ui32
		{
		public:
			constexpr static ui32 Upper = BIT(0);
			constexpr static ui32 Lower = BIT(1);
			constexpr static ui32 Right = BIT(2);
			constexpr static ui32 Left = BIT(3);
			constexpr static ui32 UpperRight = Upper | Right;
			constexpr static ui32 UpperLeft = Upper | Left;
			constexpr static ui32 LowerRight = Lower | Right;
			constexpr static ui32 LowerLeft = Lower | Left;

			PointRelation(ui32 value): enm_ui32(value) {}
		};

		struct Payload
		{
			ImageSubview* imageSubview = nullptr;
			Point point;
			PointRelation relation = PointRelation::None;
			bl enableDiagonal = false;
		};

	private:
		ImageSubview _image_subview;
		mem::delegate_bl _is_approved;
		mem::delegate_void _approved_action;
		mem::delegate_void _rejected_action;

	public:
		FloodFillImage(const ImageSubview& image_subview): _image_subview(image_subview) {}

		FloodFillImage(ImageSubview&& image_subview): _image_subview(::std::move(image_subview)) {}

		FloodFillImage(gpu::Image& image): _image_subview(image) {}

		FloodFillImage(gpu::Image& image, Subview subview): _image_subview(image, subview) {}

		virtual ~FloodFillImage() = default;

		mem::delegate_bl& GetIsApprovedDelegate()
		{
			return _is_approved;
		}

		const mem::delegate_bl& GetIsApprovedDelegate() const
		{
			return _is_approved;
		}

		mem::delegate_void& GetApprovedActionDelegate()
		{
			return _approved_action;
		}

		const mem::delegate_void& GetApprovedActionDelegate() const
		{
			return _approved_action;
		}

		mem::delegate_void& GetRejectedActionDelegate()
		{
			return _rejected_action;
		}

		const mem::delegate_void& GetRejectedActionDelegate() const
		{
			return _rejected_action;
		}

		void Fill(Payload& payload)
		{
			ImageSubview* prev_image_subview = payload.imageSubview;
			if (!prev_image_subview)
				payload.imageSubview = mem::address_of(_image_subview);

			payload.relation = PointRelation::None;

			_is_approved.SetPayload(mem::address_of(payload));
			_approved_action.SetPayload(mem::address_of(payload));
			_rejected_action.SetPayload(mem::address_of(payload));

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

			_is_approved.SetPayload(nullptr);
			_approved_action.SetPayload(nullptr);
			_rejected_action.SetPayload(nullptr);

			payload.imageSubview = prev_image_subview;
		}
	};
} // namespace np::alg

#endif /* NP_ENGINE_GPU_INTERFACE_FLOOD_FILL_IMAGE_HPP */