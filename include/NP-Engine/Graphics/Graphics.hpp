//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/12/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_HPP
#define NP_ENGINE_GRAPHICS_HPP

#include "NP-Engine/Foundation/Foundation.hpp"

//TODO: move all contents of this file to Graphics.hpp, and Graphics.cpp

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/System/System.hpp"
#include "NP-Engine/String/String.hpp"

#include "RPI/RPI.hpp"
#include "GraphicsEvents.hpp"

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>//TODO: move this to math

namespace np::graphics
{
	static void Init()
	{

	}

	static void Shutdown()
	{

	}
}

#endif /* NP_ENGINE_GRAPHICS_HPP */
