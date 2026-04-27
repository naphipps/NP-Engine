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
		mem::trait_allocator _allocator;
		evnt::EventType _type;

		NetworkEvent(evnt::EventType type): evnt::Event(), _type(evnt::EventType::Network | type) {}

	public:
		virtual ~NetworkEvent()
		{
			mem::destroy<T>(_allocator, (T*)GetPayload());
		}

		T& GetData()
		{
			return *((T*)GetPayload());
		}

		const T& GetData() const
		{
			return *((T*)GetPayload());
		}

		virtual evnt::EventType GetEventType() const override
		{
			return _type;
		}
	};

	struct NetworkClientEventData
	{
		mem::sptr<Host> host;
		mem::sptr<Socket> socket;
	};

	class NetworkClientEvent : public NetworkEvent<NetworkClientEventData>
	{
	public:
		NetworkClientEvent(evnt::EventType intention, mem::sptr<Host> host, mem::sptr<Socket> socket):
			NetworkEvent<NetworkClientEventData>(evnt::EventType::Client | intention.GetIntention())
		{
			SetPayload(mem::create<NetworkClientEventData>(_allocator, host, socket));
		}
	};
} // namespace np::net

#endif /* NP_ENGINE_NETWORK_INTERFACE_EVENTS_HPP */