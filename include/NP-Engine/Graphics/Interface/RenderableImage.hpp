//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/28/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_RENDERABLE_IMAGE_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_RENDERABLE_IMAGE_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Services/Services.hpp"

#include "GraphicsDetailType.hpp"
#include "RenderableObject.hpp"
#include "RenderableType.hpp"
#include "Image.hpp"

namespace np::gfx
{
	class RenderableImage : public RenderableObject
	{
	protected:
		srvc::Services& _services;
		Image& _image;

		RenderableImage(srvc::Services& services, Image& image): _services(services), _image(image) {}

	public:
		static RenderableImage* Create(srvc::Services& services, Image& image);

		virtual ~RenderableImage() {}

		virtual RenderableType GetType() const override
		{
			return RenderableType::Image;
		}

		Image& GetImage()
		{
			return _image;
		}

		const Image& GetImage() const
		{
			return _image;
		}
	};
} // namespace np::gfx

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_RENDERABLE_IMAGE_HPP */