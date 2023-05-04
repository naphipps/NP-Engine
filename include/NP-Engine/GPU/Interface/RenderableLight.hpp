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
#include "Light.hpp"

namespace np::gpu
{
	class RenderableLight : public RenderableLightObject
	{
	protected:
		mem::sptr<srvc::Services> _services;
		mem::sptr<Light> _light;

		RenderableLight(mem::sptr<srvc::Services> services, mem::sptr<Light> light): _services(services), _light(light) {}

	public:
		static RenderableLight* Create(mem::sptr<srvc::Services> services, mem::sptr<Light> light);

		virtual ~RenderableLight() {}

		virtual ResourceType GetType() const override
		{
			return ResourceType::RenderableLight;
		}

		mem::sptr<Light> GetLight() const
		{
			return _light;
		}
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_RENDERABLE_LIGHT_HPP */