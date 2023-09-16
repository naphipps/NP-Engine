//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/7/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_NETWORK_INTERFACE_SOCKET_HPP
#define NP_ENGINE_NETWORK_INTERFACE_SOCKET_HPP

#define NP_ENGINE_NETWORK_SOCKET_HANDLE_INVALID_KEY 0
#define NP_ENGINE_NETWORK_SOCKET_HANDLE_INVALID_GENERATION 0

#ifndef NP_ENGINE_NETWORK_SOCKET_RECEIVING_SLEEP_DURATION
	#define NP_ENGINE_NETWORK_SOCKET_RECEIVING_SLEEP_DURATION 4
#endif

#include "NP-Engine/Services/Services.hpp"
#include "NP-Engine/Math/Math.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "Context.hpp"
#include "Message.hpp"
#include "Protocol.hpp"
#include "MessageQueue.hpp"
#include "Ip.hpp"

namespace np::net
{
	enum class SocketOptions : ui32
	{
		None = 0,
		ReuseAddress,
		ReusePort,

		Max
	};

	class Socket
	{
	protected:
		mem::sptr<Context> _context;
		MessageQueue _inbox;
		mutexed_wrapper<con::queue<Message>> _outbox;

		virtual void DetailSend(Message msg) = 0;

		virtual void DetailSendTo(Message msg, const Ip& ip, ui16 port) = 0;

		Socket(mem::sptr<Context> context): _context(context) {}

	public:
		static mem::sptr<Socket> Create(mem::sptr<Context> context);

		virtual ~Socket() = default;

		operator bl() const
		{
			return IsOpen();
		}

		virtual void Open(Protocol protocol) = 0;

		virtual void Enable(con::vector<SocketOptions> options) = 0;

		virtual void Disable(con::vector<SocketOptions> options) = 0;

		virtual void Close() = 0;

		virtual bl IsOpen() const = 0;

		virtual void BindTo(const Ip& ip, ui16 port) = 0;

		virtual void Listen() = 0;

		virtual mem::sptr<Socket> Accept(bl enable_client_resolution = false) = 0;

		virtual void ConnectTo(const Ip& ip, ui16 port) = 0;

		void Send(Message msg)
		{
			if (CanSend(msg))
				DetailSend(msg);
		}

		void SendTo(Message msg, const Ip& ip, ui16 port)
		{
			if (CanSend(msg))
				DetailSendTo(msg, ip, port);
		}

		virtual bl CanSend(const Message& msg) const
		{
			NP_ENGINE_ASSERT(msg && msg.header.bodySize <= NP_ENGINE_NETWORK_MAX_MESSAGE_BODY_SIZE,
				"Message.header.bodySize is larger than " + to_str(NP_ENGINE_NETWORK_MAX_MESSAGE_BODY_SIZE));

			bl can = true;
			if (msg && msg.header.bodySize > NP_ENGINE_NETWORK_MAX_MESSAGE_BODY_SIZE)
				can = false;
			return can;
		}

		virtual void StartReceiving() = 0;

		virtual bl IsRecieving() const = 0;

		virtual void StopReceiving() = 0;

		virtual MessageQueue& GetInbox()
		{
			return _inbox;
		}

		virtual const MessageQueue& GetInbox() const
		{
			return _inbox;
		}

		virtual DetailType GetDetailType() const
		{
			return _context->GetDetailType();
		}

		virtual mem::sptr<Context> GetContext() const
		{
			return _context;
		}

		virtual mem::sptr<srvc::Services> GetServices() const
		{
			return _context->GetServices();
		}
	};
} // namespace np::net

#endif /* NP_ENGINE_NETWORK_INTERFACE_SOCKET_HPP */