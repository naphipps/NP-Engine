//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/7/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_NETWORK_INTERFACE_CONTEXT_HPP
#define NP_ENGINE_NETWORK_INTERFACE_CONTEXT_HPP

#include "NP-Engine/Services/Services.hpp"

#include "DetailType.hpp"

namespace np::net
{
	class Context
	{
	protected:
		mem::sptr<srvc::Services> _services;

		Context(mem::sptr<srvc::Services> services): _services(services) {}

	public:
		static mem::sptr<Context> Create(DetailType detail_type, mem::sptr<srvc::Services> services);

		virtual ~Context() = default;

		virtual DetailType GetDetailType() const
		{
			return DetailType::None;
		}

		virtual mem::sptr<srvc::Services> GetServices() const
		{
			return _services;
		}
	};
} // namespace np::net

#endif /* NP_ENGINE_NETWORK_INTERFACE_CONTEXT_HPP */