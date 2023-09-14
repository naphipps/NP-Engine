//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/9/23
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Foundation/Foundation.hpp"

#include "NP-Engine/Network/Interface/Interface.hpp"

#include "NP-Engine/Network/Detail/Native/NativeResolver.hpp"

namespace np::net
{
	mem::sptr<Resolver> Resolver::Create(mem::sptr<Context> context)
	{
		mem::sptr<Resolver> resolver = nullptr;

		switch (context->GetDetailType())
		{
		case DetailType::Native:
			resolver = mem::create_sptr<__detail::NativeResolver>(context->GetServices()->GetAllocator(), context);
			break;

		default:
			break;
		}

		return resolver;
	}

}