//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/5/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VENDOR_ENTT_INCLUDE_HPP
#define NP_ENGINE_VENDOR_ENTT_INCLUDE_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Platform/Platform.hpp"

// github.com/skypjack/entt/wiki/Frequently-Asked-Questions#warning-c4003-the-min-the-max-and-the-macro
#if NP_ENGINE_PLATFORM_IS_WINDOWS
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
#endif

#include <entt/entt.hpp>

#endif /* NP_ENGINE_VENDOR_ENTT_INCLUDE_HPP */