//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/7/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RENDERER_RPI_RENDERER_HPP
#define NP_ENGINE_RENDERER_RPI_RENDERER_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::renderer::rpi
{
	class Renderer
	{
	public:

		enum class Type
		{
			OpenGL,
			Vulkan,
			Direct3D,
			Metal
		};

		virtual bl Init() = 0; //TODO: maybe we ought to use an IsValid method or something? If I construct a renderer, it should be ready to go or not

		virtual Type GetType() const = 0;

	};
}

#endif /* NP_ENGINE_RENDERER_RPI_RENDERER_HPP */