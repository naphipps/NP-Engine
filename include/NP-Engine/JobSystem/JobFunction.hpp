//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/3/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_JOB_FUNCTION_HPP
#define NP_ENGINE_JOB_FUNCTION_HPP

#include "NP-Engine/Memory/Memory.hpp"

namespace np::js
{
	using JobFunction = memory::Delegate; // TODO: consider a design where this is removed
} // namespace np::js

#endif /* NP_ENGINE_JOB_FUNCTION_HPP */
