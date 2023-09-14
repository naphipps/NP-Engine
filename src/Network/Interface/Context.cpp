//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/7/23
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Foundation/Foundation.hpp"

#include "NP-Engine/Network/Interface/Interface.hpp"

#include "NP-Engine/Network/Detail/Native/NativeContext.hpp"

namespace np::net
{
	mem::sptr<Context> Context::Create(DetailType detail_type, mem::sptr<srvc::Services> services)
	{
		mem::sptr<Context> context = nullptr;

		switch (detail_type)
		{
		case DetailType::Native:
			context = mem::create_sptr<__detail::NativeContext>(services->GetAllocator(), services);
			break;

		default:
			break;
		}

		return context;
	}
}