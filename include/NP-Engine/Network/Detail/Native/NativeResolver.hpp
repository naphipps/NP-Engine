//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/9/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_NETWORK_WINDOWS_RESOLVER_HPP
#define NP_ENGINE_NETWORK_WINDOWS_RESOLVER_HPP

#include "NP-Engine/Foundation/Foundation.hpp"

#include "NP-Engine/Network/Interface/Interface.hpp"

#include "NativeNetworkInclude.hpp"

namespace np::net::__detail
{
	class NativeResolver : public Resolver
	{
	public:
		NativeResolver(mem::sptr<Context> context): Resolver(context) {}

		virtual Host GetHost(str name) override
		{
			Host host;
			host.name = name;

			addrinfo hints{};
			hints.ai_flags = AI_CANONNAME;

			addrinfo* info = nullptr;
			i32 res = getaddrinfo(host.name.c_str(), nullptr, &hints, &info);

			if (!res)
			{
				host.name = info->ai_canonname;

				for (addrinfo* it = info; it; it = it->ai_next)
				{
					switch (it->ai_family)
					{
					case AF_INET:
					{
						sockaddr_in* saddrin = (sockaddr_in*)it->ai_addr;
						ui16 port = ntohs(saddrin->sin_port);

						Ipv4 ipv4;
						mem::CopyBytes(ipv4.bytes.data(), &saddrin->sin_addr.s_addr, ipv4.bytes.size());
						host.ipv4s.emplace(ipv4, port);
						break;
					}
					case AF_INET6: // TODO: NOT IMPLEMENTEDD YET
					{
						break;
					}
					default:
						break;
					}
				}
			}

			if (info)
				freeaddrinfo(info);

			host.aliases.emplace(name);
			host.aliases.emplace(host.name);
			return host;
		}

		virtual Host GetHost(const Ip& ip) override
		{
			Host host;
			chr name[NI_MAXHOST]{};
			i32 res = -1;

			switch (ip.GetType())
			{
			case IpType::V4:
			{
				Ipv4& ipv4 = (Ipv4&)ip;
				host.ipv4s.emplace(ipv4, 0);

				sockaddr_in saddrin{};
				saddrin.sin_family = AF_INET;
				mem::CopyBytes(&saddrin.sin_addr.s_addr, ipv4.bytes.data(), ipv4.bytes.size());

				res = getnameinfo((sockaddr*)&saddrin, sizeof(sockaddr_in), name, NI_MAXHOST, nullptr, 0, 0);
				break;
			}
			case IpType::V6: // TODO: NOT IMPLEMENTEDD YET
			{
				break;
			}
			default:
				break;
			}

			if (!res)
				host.name = name;

			host.aliases.emplace(host.name);
			return host;
		}
	};
} // namespace np::net::__detail

#endif /* NP_ENGINE_NETWORK_WINDOWS_RESOLVER_HPP */