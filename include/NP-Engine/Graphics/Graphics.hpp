//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/12/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_HPP
#define NP_ENGINE_GRAPHICS_HPP

#include "NP-Engine/Foundation/Foundation.hpp"

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/System/System.hpp"
#include "NP-Engine/String/String.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"
#include "NP-Engine/Vendor/GlmInclude.hpp"

#include "RPI/RPI.hpp"
#include "GraphicsEvents.hpp"

namespace np::graphics
{
	// TODO: figure out if we need these functions, and if so, should be put them in Graphics.cpp?
	static void Init() {}

	static void Shutdown() {}
} // namespace np::graphics

#endif /* NP_ENGINE_GRAPHICS_HPP */
