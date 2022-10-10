//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/9/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RPI_VISIBLE_OBJECT_HPP
#define NP_ENGINE_RPI_VISIBLE_OBJECT_HPP

#include "NP-Engine/Geometry/Geometry.hpp"

#include "RenderableLight.hpp"
#include "RenderableModel.hpp"
#include "RenderableImage.hpp"
#include "RenderableType.hpp"

namespace np::gfx
{
	class VisibleObject
	{
	private:
		RenderableType _renderable_type = RenderableType::None;
		void* _payload = nullptr;

	public:
		geom::FltSphere BoundingSphere{}; // TODO: i think bounding AABB are going to be the way we go

		void* GetPayload() const
		{
			return _payload;
		}

		void SetPayload(RenderableModel* renderable_model)
		{
			_renderable_type = RenderableType::Model;
			_payload = renderable_model;
		}

		void SetPayload(RenderableLight* renderable_light)
		{
			_renderable_type = RenderableType::Light;
			_payload = renderable_light;
		}

		void SetPayload(RenderableImage* renderable_image)
		{
			_renderable_type = RenderableType::Image;
			_payload = renderable_image;
		}

		void ClearPayload()
		{
			_renderable_type = RenderableType::None;
			_payload = nullptr;
		}

		RenderableType GetRenderableType() const
		{
			return _renderable_type;
		}
	};
} // namespace np::gfx

#endif /* NP_ENGINE_RPI_VISIBLE_OBJECT_HPP */