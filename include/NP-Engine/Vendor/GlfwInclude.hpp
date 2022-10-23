//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/5/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VENDOR_GLFW_INCLUDE_HPP
#define NP_ENGINE_VENDOR_GLFW_INCLUDE_HPP

#include "NP-Engine/Foundation/Foundation.hpp"

#if NP_ENGINE_PLATFORM_IS_WINDOWS
	#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#define GLFW_INCLUDE_VULKAN // required so glfw supports vulkan

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>


#if NP_ENGINE_PLATFORM_IS_WINDOWS
/*
	Windows doth thinkest best to spell almighty macros with capitalizations the lay use.
	Fools!
	Windows, now is the time we declare this SHALL NOT PASS.
*/
	#undef CreateWindow
	#undef max
	#undef min
#endif

//TODO: add glfw allocator

#endif /* NP_ENGINE_VENDOR_GLFW_INCLUDE_HPP */