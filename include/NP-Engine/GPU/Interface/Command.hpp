//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_COMMAND_HPP
#define NP_ENGINE_GPU_INTERFACE_COMMAND_HPP

#include "DetailType.hpp"

namespace np::gpu
{
	struct Command
	{
		virtual DetailType GetDetailType() const = 0;
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_COMMAND_HPP */