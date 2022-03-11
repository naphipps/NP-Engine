//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/21/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_ASSERT_HPP
#define NP_ENGINE_ASSERT_HPP

#include "ConstantsSettings.hpp"
#include "Constants.hpp"

#if defined(NP_ENGINE_ENABLE_ASSERT) && NP_ENGINE_ENABLE_ASSERT
	#include <cassert>

	#ifndef NP_ENGINE_ASSERT
		#include <iostream>
		#include <string>

namespace np::__detail
{
	static void NpEngineAssert(bool expression, ::std::string message)
	{
		if (!expression)
		{
			::std::cerr << message;
			assert(false);
		}
	}
} // namespace np::__detail

		#define NP_ENGINE_ASSERT(expression, message) ::np::__detail::NpEngineAssert(expression, message)
	#endif

	#ifndef NP_ENGINE_STATIC_ASSERT
		#define NP_ENGINE_STATIC_ASSERT(expression, message) static_assert(expression, message)
	#endif

#else
	#define NP_ENGINE_ASSERT(expression, message)
	#define NP_ENGINE_STATIC_ASSERT(expression, message)
#endif

#endif /* NP_ENGINE_ASSERT_HPP */
