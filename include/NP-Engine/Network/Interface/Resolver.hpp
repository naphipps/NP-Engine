//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/9/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_NETWORK_INTERFACE_RESOVLER_HPP
#define NP_ENGINE_NETWORK_INTERFACE_RESOVLER_HPP

#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/String/String.hpp"

#include "Context.hpp"
#include "Ip.hpp"
#include "Host.hpp"

namespace np::net
{
	class Resolver
	{
	protected:
		mem::sptr<Context> _context;

		Resolver(mem::sptr<Context> context): _context(context) {}

	public:
		static mem::sptr<Resolver> Create(mem::sptr<Context> context);

		virtual ~Resolver() = default;

		virtual Host GetHost(str name) = 0;

		virtual Host GetHost(const Ip& ip) = 0;
	};
}

#endif /* NP_ENGINE_NETWORK_INTERFACE_RESOVLER_HPP */