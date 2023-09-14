//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/11/23
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Foundation/Foundation.hpp"

#include "NP-Engine/Network/Interface/Interface.hpp"

#include "NP-Engine/Network/Detail/Native/NativeNetworkInclude.hpp"

namespace np::net
{
	void Init(DetailType detail_type)
	{
		switch (detail_type)
		{
		case DetailType::Native:
		{
#if NP_ENGINE_PLATFORM_IS_WINDOWS
			WSADATA wsa_data{};
			WORD expected_version = MAKEWORD(2, 2);
			i32 err = WSAStartup(expected_version, &wsa_data);
			NP_ENGINE_ASSERT(err == 0, "NativeContext err on WSAStartup: " + to_str(err));

			NP_ENGINE_ASSERT(LOBYTE(wsa_data.wVersion) == 2 || HIBYTE(wsa_data.wVersion) == 2,
				"NativeContext err with WSAStartup: Found winsock " + to_str(wsa_data.wVersion) +
				" when expecting " + to_str(expected_version));
#else
#error //TODO: implement native networking
#endif
			break;
		}
		default:
			break;
		}
	}

	void Update(DetailType detail_type)
	{
		switch (detail_type)
		{
		case DetailType::Native:
		{
#if NP_ENGINE_PLATFORM_IS_WINDOWS
#else
#error //TODO: implement native networking
#endif
			break;
		}
		default:
			break;
		}
	}

	void Terminate(DetailType detail_type)
	{
		switch (detail_type)
		{
		case DetailType::Native:
		{
#if NP_ENGINE_PLATFORM_IS_WINDOWS
			WSACleanup();
#else
#error //TODO: implement native networking
#endif
			break;
		}
		default:
			break;
		}
	}
}