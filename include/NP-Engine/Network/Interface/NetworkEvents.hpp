//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/7/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_NETWORK_INTERFACE_EVENTS_HPP
#define NP_ENGINE_NETWORK_INTERFACE_EVENTS_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Event/Event.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "Socket.hpp"
#include "Host.hpp"

namespace np::net
{
	template <typename T>
	class NetworkEvent : public evnt::Event
	{
	protected:
		mem::TraitAllocator _allocator;

		NetworkEvent(): evnt::Event() {}

	public:
		virtual ~NetworkEvent()
		{
			mem::Destroy<T>(_allocator, (T*)GetPayload());
		}

		T& GetData()
		{
			return *((T*)GetPayload());
		}

		const T& GetData() const
		{
			return *((T*)GetPayload());
		}

		evnt::EventCategory GetCategory() const override
		{
			return evnt::EventCategory::Network;
		}
	};

	// Network Client Event
	struct NetworkClientEventData
	{
		mem::sptr<Host> host;
		mem::sptr<Socket> socket;
	};

	class NetworkClientEvent : public NetworkEvent<NetworkClientEventData>
	{
	public:
		NetworkClientEvent(mem::sptr<Host> host, mem::sptr<Socket> socket): NetworkEvent<NetworkClientEventData>()
		{
			SetPayload(mem::Create<NetworkClientEventData>(_allocator, host, socket));
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::NetworkClient;
		}
	};
} // namespace np::net

#endif /* NP_ENGINE_NETWORK_INTERFACE_EVENTS_HPP */