//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/20/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_CONSTANTS_SETTINGS_HPP
#define NP_ENGINE_CONSTANTS_SETTINGS_HPP

#include "cmake_include.hpp"

#define B2_USE_16_BIT_PARTICLE_INDICES

#ifndef NP_ENGINE_MAIN_MEMORY_SIZE
	#define NP_ENGINE_MAIN_MEMORY_SIZE (2000000000)
#endif

#if DEBUG
	#define NP_PROFILE_ENABLE 1
	#define NP_LOG_ENABLE 1
	#define NP_ASSERT_ENABLE 1
#endif

#endif /* NP_ENGINE_CONSTANTS_SETTINGS_HPP */
