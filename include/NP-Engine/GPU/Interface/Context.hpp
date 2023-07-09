//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/12/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_CONTEXT_HPP
#define NP_ENGINE_GPU_INTERFACE_CONTEXT_HPP

#include "NP-Engine/Services/Services.hpp"

#include "DetailType.hpp"

namespace np::gpu
{
	class Context
	{
		// TODO: I think our other contexts inherit from this?

	protected:
		Context() {}

	public:
		virtual DetailType GetDetailType() const = 0;
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_CONTEXT_HPP */