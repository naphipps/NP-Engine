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
	static sockaddr_in ToSaddrin(const Ip& ip, ui16 port)
	{
		sockaddr_in saddrin{};
		switch (ip.GetType())
		{
		case IpType::V4:
		{
			saddrin.sin_family = AF_INET;
			Ipv4& ipv4 = (Ipv4&)ip;
			mem::CopyBytes(&saddrin.sin_addr.s_addr, ipv4.bytes.data(), ipv4.bytes.size());
			saddrin.sin_port = htons(port);
			break;
		}
		case IpType::V6: // TODO: implement -- I think we'll have to overload this function since sockaddr_in6 is so different
		{
			NP_ENGINE_ASSERT(false, "ToSaddrin using IpType::V6 is not supported yet");
			break;
		}
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