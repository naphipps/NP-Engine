//
//  Constants.hpp
//  Project Space
//
//  Created by Nathan Phipps on 5/20/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_CONSTANTS_HPP
#define NP_ENGINE_CONSTANTS_HPP

//--------------------------------------------------------------------------------

#ifndef NP_ENGINE_CONSTANTS_SETTINGS_HPP
	#error constants settings hpp has not been included before this file
#endif

//--------------------------------------------------------------------------------

#define MACRO_TO_STR(X) #X
#define MACRO_VAL_TO_STR(X) MACRO_TO_STR(X)
#define NP_FILE_AND_LINE __FILE__ ":" MACRO_VAL_TO_STR(__LINE__)

//--------------------------------------------------------------------------------

// below from Boost <https://www.boost.org/doc/libs/1_58_0/boost/current_function.hpp>
#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || \
	defined(__ghs__)
	#define NP_FUNCTION __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
	#define NP_FUNCTION __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
	#define NP_FUNCTION __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
	#define NP_FUNCTION __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
	#define NP_FUNCTION __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
	#define NP_FUNCTION __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
	#define NP_FUNCTION __func__
#else
	#define NP_FUNCTION NP_FILE_AND_LINE
#endif // NP_FUNCTION
// concat NP_FUNCTION via str(NP_FUNCTION)

//--------------------------------------------------------------------------------

#endif /* NP_ENGINE_CONSTANTS_HPP */
