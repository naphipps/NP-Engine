//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/15/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_RENDERABLE_LIGHT_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_RENDERABLE_LIGHT_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Services/Services.hpp"

#include "GraphicsDetailType.hpp"
#include "RenderableLightObject.hpp"
#include "RenderableType.hpp"
#include "Light.hpp"

namespace np::gfx
{
	class RenderableLight : public RenderableLightObject
	{
	protected:
		srvc::Services& _services;
		Light& _light;

		RenderableLight(srvc::Services& services, Light& light): _services(services), _light(light) {}

	public:
		static RenderableLight* Create(srvc::Services& services, Light& light);

		virtual ~RenderableLight() {}

		virtual RenderableType GetType() const override
		{
			return RenderableType::Light;
		}

		Light& GetLight()
		{
			return _light;
		}

		const Light& GetLight() const
		{
			return _light;
		}
	};
} // namespace np::gfx

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_RENDERABLE_LIGHT_HPP */