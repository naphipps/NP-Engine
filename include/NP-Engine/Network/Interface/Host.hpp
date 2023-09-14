//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/12/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_NETWORK_INTERFACE_HOST_HPP
#define NP_ENGINE_NETWORK_INTERFACE_HOST_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/String/String.hpp"

#include "Ip.hpp"

namespace np::net
{
	struct Host
	{
		str name = "";
		con::uset<str> aliases;
		con::uset<::std::pair<Ipv4, ui16>> ipv4s;
		con::uset<::std::pair<Ipv6, ui16>> ipv6s;

		void Clear()
		{
			name = "";
			aliases.clear();
			ipv4s.clear();
			ipv6s.clear();
		}
	};
} // namespace np::net

#endif /* NP_ENGINE_NETWORK_INTERFACE_HOST_HPP */