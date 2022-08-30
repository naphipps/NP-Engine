//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/20/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_CONSTANTS_HPP
#define NP_ENGINE_CONSTANTS_HPP

#ifndef NP_ENGINE_CONSTANTS_SETTINGS_HPP
	#error constants settings hpp has not been included before this file
#endif

#define NP_ENGINE_MACRO_TO_STR(X) #X
#define NP_ENGINE_MACRO_VAL_TO_STR(X) NP_ENGINE_MACRO_TO_STR(X)
#define NP_ENGINE_FILE_AND_LINE __FILE__ ":" NP_ENGINE_MACRO_VAL_TO_STR(__LINE__)

// below from Boost <https://www.boost.org/doc/libs/1_58_0/boost/current_function.hpp>
#ifndef NP_ENGINE_FUNCTION
	#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || \
		defined(__ghs__)
		#define NP_ENGINE_FUNCTION __PRETTY_FUNCTION__
	#elif defined(__DMC__) && (__DMC__ >= 0x810)
		#define NP_ENGINE_FUNCTION __PRETTY_FUNCTION__
	#elif defined(__FUNCSIG__)
		#define NP_ENGINE_FUNCTION __FUNCSIG__
	#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
		#define NP_ENGINE_FUNCTION __FUNCTION__
	#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
		#define NP_ENGINE_FUNCTION __FUNC__
	#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
		#define NP_ENGINE_FUNCTION __func__
	#elif defined(__cplusplus) && (__cplusplus >= 201103)
		#define NP_ENGINE_FUNCTION __func__
	#else
		#define NP_ENGINE_FUNCTION NP_ENGINE_FILE_AND_LINE
	#endif
#endif // NP_ENGINE_FUNCTION

#define NP_ENGINE_EXIT_SUCCESS 0
#define NP_ENGINE_EXIT_UNKNOWN_ERROR 1
#define NP_ENGINE_EXIT_MEMORY_ERROR 3
#define NP_ENGINE_EXIT_STD_ERROR 2

#endif /* NP_ENGINE_CONSTANTS_HPP */
