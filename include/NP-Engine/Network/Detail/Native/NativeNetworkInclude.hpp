//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/10/23
//
//##===----------------------------------------------------------------------===##//

/*
	Reference:
		- <https://learn.microsoft.com/en-us/windows/win32/api/winsock2/>
		- <https://learn.microsoft.com/en-us/windows/win32/api/ws2tcpip/>
		- <https://stackoverflow.com/questions/5100718/integer-to-hex-string-in-c>
*/

#ifndef NP_ENGINE_NETWORK_NETWORK_INCLUDE_HPP
#define NP_ENGINE_NETWORK_NETWORK_INCLUDE_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#if NP_ENGINE_PLATFORM_IS_WINDOWS
	#include <Winsock2.h>
	#include <WS2tcpip.h>
	#undef max
	#undef min

#elif NP_ENGINE_PLATFORM_IS_LINUX
	#include <sys/types.h>
	#include <unistd.h>
	#include <sys/socket.h>
	#include <netdb.h>
	#include <arpa/inet.h>

	#define INVALID_SOCKET ((ui64)-1)
	#define SD_RECEIVE SHUT_RD
	#define SD_SEND SHUT_WR
	#define SD_BOTH SHUT_RDWR

#else
	#error implement native networking

#endif

namespace np::net
{
	namespace __detail
	{
		static void PopulateSaddrin(sockaddr_in& saddrin, const Ipv4 ipv4, const ui16 port)
		{
			saddrin.sin_family = AF_INET;
			mem::CopyBytes(&saddrin.sin_addr.s_addr, ipv4.bytes.data(), ipv4.bytes.size());
			saddrin.sin_port = htons(port);
		}

		static void PopulateSaddrin(sockaddr_in6& saddrin, const Ipv6 ipv6, const ui16 port)
		{
			saddrin.sin6_family = AF_INET6;
			mem::CopyBytes(&saddrin.sin6_addr.s6_addr, ipv6.shorts.data(), ipv6.shorts.size() * sizeof(ui16));
			saddrin.sin6_port = htons(port);
		}
	}

	static ::std::pair<sockaddr*, siz> ToSaddrin(const Ip& ip, ui16 port, sockaddr_in& saddrin4, sockaddr_in6& saddrin6)
	{
		::std::pair<sockaddr*, siz> saddrin{nullptr, 0};
		switch (ip.GetType())
		{
		case IpType::V4:
			__detail::PopulateSaddrin(saddrin4, (Ipv4&)ip, port);
			saddrin = {(sockaddr*)&saddrin4, sizeof(sockaddr_in)};
			break;

		case IpType::V6:
			__detail::PopulateSaddrin(saddrin6, (Ipv6&)ip, port);
			saddrin = {(sockaddr*)&saddrin6, sizeof(sockaddr_in6)};
			break;

		default:
			break;
		}
		return saddrin;
	}


	static void PopulateHost(const sockaddr_in& saddrin, Host& host)
	{
		Ipv4 ipv4;
		mem::CopyBytes(ipv4.bytes.data(), &saddrin.sin_addr.s_addr, ipv4.bytes.size());
		host.ipv4s.emplace(ipv4, ntohs(saddrin.sin_port));
	}

	static void PopulateHost(const sockaddr_in6& saddrin, Host& host)
	{
		Ipv6 ipv6;
		mem::CopyBytes(ipv6.shorts.data(), &saddrin.sin6_addr.s6_addr, ipv6.shorts.size() * sizeof(ui16));
		host.ipv6s.emplace(ipv6, ntohs(saddrin.sin6_port));
	}
} // namespace np::net

#endif /* NP_ENGINE_NETWORK_NETWORK_INCLUDE_HPP */