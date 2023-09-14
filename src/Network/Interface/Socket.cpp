//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/7/23
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Foundation/Foundation.hpp"

#include "NP-Engine/Network/Interface/Interface.hpp"

#include "NP-Engine/Network/Detail/Native/NativeSocket.hpp"

namespace np::net
{
	mem::sptr<Socket> Socket::Create(mem::sptr<Context> context)
	{
		mem::sptr<Socket> socket = nullptr;

		switch (context->GetDetailType())
		{
		case DetailType::Native:
			socket = mem::create_sptr<__detail::NativeSocket>(context->GetServices()->GetAllocator(), context);
			break;

		default:
			break;
		}

		return socket;
	}
}