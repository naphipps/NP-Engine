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
	#error // TODO: implement native networking

#endif

#endif /* NP_ENGINE_NETWORK_NETWORK_INCLUDE_HPP */