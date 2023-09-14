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

#if NP_ENGINE_PLATFORM_IS_WINDOWS
	#include <Winsock2.h>
	#include <WS2tcpip.h>
	#undef max
	#undef min

#else
	#error //TODO: implement native networking

#endif

#endif /* NP_ENGINE_NETWORK_NETWORK_INCLUDE_HPP */