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
		#define NP_ENGINE_PROFILE_ENABLE false
	#endif
	#ifndef NP_ENGINE_LOG_ENABLE
		#define NP_ENGINE_LOG_ENABLE true
	#endif
	#ifndef NP_ENGINE_ASSERT_ENABLE
		#define NP_ENGINE_ASSERT_ENABLE true
	#endif
#endif

#endif /* NP_ENGINE_CONSTANTS_SETTINGS_HPP */
