//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/28/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RPI_RENDERABLE_IMAGE_HPP
#define NP_ENGINE_RPI_RENDERABLE_IMAGE_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Services/Services.hpp"

#include "RhiType.hpp"
#include "RenderableObject.hpp"
#include "RenderableType.hpp"
#include "Image.hpp"

namespace np::graphics
{
	class RenderableImage : public RenderableObject
	{
	protected:
		services::Services& _services;
		Image& _image;

		RenderableImage(services::Services& services, Image& image): _services(services), _image(image) {}

	public:
		static RenderableImage* Create(services::Services& services, Image& image);

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
} // namespace np::graphics

#endif /* NP_ENGINE_RPI_RENDERABLE_IMAGE_HPP */