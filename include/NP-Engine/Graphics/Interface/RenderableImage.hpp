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
#include "Image.hpp"

namespace np::gpu
{
	class RenderableImage : public RenderableObject
	{
	protected:
		mem::sptr<srvc::Services> _services;
		mem::sptr<Image> _image;

		RenderableImage(mem::sptr<srvc::Services> services, mem::sptr<Image> image): _services(services), _image(image) {}

	public:
		static RenderableImage* Create(srvc::Services& services, Image& image);

		virtual ~RenderableImage() {}

		virtual ResourceType GetType() const override
		{
			return ResourceType::RenderableImage;
		}

		mem::sptr<Image> GetImage() const
		{
			return _image;
		}
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_RENDERABLE_IMAGE_HPP */