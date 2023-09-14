//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/9/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_NETWORK_CONTEXT_HPP
#define NP_ENGINE_NETWORK_CONTEXT_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/String/String.hpp"

#include "NP-Engine/Network/Interface/Interface.hpp"

#include "NativeNetworkInclude.hpp"

namespace np::net::__detail
{
	class NativeContext : public Context
	{
	public:
		NativeContext(mem::sptr<srvc::Services> services): Context(services) {}

		virtual DetailType GetDetailType() const override
		{
			return DetailType::Native;
		}
	};
} // namespace np::net::__detail

#endif /* NP_ENGINE_NETWORK_CONTEXT_HPP */