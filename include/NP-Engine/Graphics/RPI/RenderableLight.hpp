//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/15/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RPI_RENDERABLE_LIGHT_HPP
#define NP_ENGINE_RPI_RENDERABLE_LIGHT_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "RhiType.hpp"
#include "RenderableLightObject.hpp"
#include "Light.hpp"

namespace np::graphics
{
	class RenderableLight : public RenderableLightObject
	{
	protected:
		Light& _light;

		RenderableLight(Light& light): _light(light) {}

	public:
		static RenderableLight* Create(memory::Allocator& allocator, Light& light);

		Light& GetLight()
		{
			return _light;
		}

		const Light& GetLight() const
		{
			return _light;
		}
	};
} // namespace np::graphics

#endif /* NP_ENGINE_RPI_RENDERABLE_LIGHT_HPP */