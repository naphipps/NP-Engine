//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/20/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_CONSTANTS_SETTINGS_HPP
#define NP_ENGINE_CONSTANTS_SETTINGS_HPP

#include "NP-Engine/Platform/Platform.hpp"

#include "cmake_include.hpp"

#if DEBUG
	#ifndef NP_ENGINE_PROFILE_ENABLE
		#define NP_ENGINE_PROFILE_ENABLE true
	#endif
	#ifndef NP_ENGINE_LOG_ENABLE
		#define NP_ENGINE_LOG_ENABLE true
	#endif
	#ifndef NP_ENGINE_ENABLE_ASSERT
		#define NP_ENGINE_ENABLE_ASSERT true
	#endif
#endif

#ifndef NP_ENGINE_APPLICATION_LOOP_DURATION
	#define NP_ENGINE_APPLICATION_LOOP_DURATION 4 //1 // milliseconds
#endif

#ifndef NP_ENGINE_RENDERING_LOOP_DURATION
	#define NP_ENGINE_RENDERING_LOOP_DURATION 4 //1.66666667 // milliseconds
#endif

#ifndef NP_ENGINE_WINDOW_LOOP_DURATION
	#define NP_ENGINE_WINDOW_LOOP_DURATION 8 //4 // milliseconds
#endif

#endif /* NP_ENGINE_CONSTANTS_SETTINGS_HPP */
