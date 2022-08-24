//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/20/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_CONSTANTS_SETTINGS_HPP
#define NP_ENGINE_CONSTANTS_SETTINGS_HPP

#include "NP-Engine/Platform/Platform.hpp"

#include "cmake_include.hpp"

#ifndef NP_ENGINE_MAIN_MEMORY_SIZE
	#define NP_ENGINE_MAIN_MEMORY_SIZE (2000000000)
#endif

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
	#define NP_ENGINE_APPLICATION_LOOP_DURATION 4 // milliseconds
#endif

#ifndef NP_ENGINE_WINDOW_LOOP_DURATION
	#define NP_ENGINE_WINDOW_LOOP_DURATION 8 // milliseconds
#endif

#endif /* NP_ENGINE_CONSTANTS_SETTINGS_HPP */
