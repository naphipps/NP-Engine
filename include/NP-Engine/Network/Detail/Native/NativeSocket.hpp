//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/9/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_NETWORK_WINDOWS_SOCKET_HPP
#define NP_ENGINE_NETWORK_WINDOWS_SOCKET_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "NP-Engine/Network/Interface/Interface.hpp"

#include "NativeNetworkInclude.hpp"

namespace np::net::__detail
{
	class NativeSocket : public Socket
	{
	protected:
		ui64 _socket;
		Protocol _protocol; // TODO: do we need this?? I admit, it makes things easier
		atm_bl _keep_receiving;

		void SendBytes(const chr* src, siz byte_count)
		{
			NP_ENGINE_PROFILE_FUNCTION();
			for (siz total = 0; total < byte_count;)
			{
				i32 sent = send(_socket, src + total, byte_count - total, 0);
				if (sent < 0)
				{
					Close();
					break;
				}
				total += sent;
			}
		}

		void RecvBytes(chr* dst, siz byte_count)
		{
			for (ui32 total = 0; total < byte_count;)
			{
				i32 recvd = recv(_socket, dst + total, byte_count - total, 0);
				if (recvd < 0)
				{
					Close();
					break;
				}
				total += recvd;
			}
		}

		virtual void DetailSend(Message msg) override
		{
			NP_ENGINE_PROFILE_FUNCTION();
			if (IsOpen() && msg)
			{
				SendBytes((chr*)&msg.header, sizeof(MessageHeader));
				if (msg.header.bodySize > 0)
					SendBytes((chr*)msg.body->GetData(), msg.header.bodySize);
			}
		}

		static void ReceivingCallback(void* caller, mem::Delegate& d)
		{
			((NativeSocket*)caller)->ReceivingProcedure();
		}

		void ReceivingProcedure()
		{
			Message msg;
			RecvBytes((chr*)&msg.header, sizeof(MessageHeader));

			if (msg.header.bodySize > 0)
			{
				switch (msg.header.type)
				{
				case MessageType::Text:
					msg.body = mem::create_sptr<TextMessageBody>(GetServices()->GetAllocator());
					msg.body->SetSize(msg.header.bodySize);
					break;

				default:
					break;
				}

				RecvBytes((chr*)msg.body->GetData(), msg.header.bodySize);
			}

			if (msg)
			{
				_inbox.Push(msg);
				if (_keep_receiving.load(mo_acquire))
					SubmitReceivingJob();
			}
			else if (_keep_receiving.load(mo_acquire))
			{
				thr::ThisThread::sleep_for(tim::DblMilliseconds(NP_ENGINE_NETWORK_SOCKET_RECEIVING_SLEEP_DURATION));
				SubmitReceivingJob();
			}
		}

		void SubmitReceivingJob()
		{
			mem::sptr<jsys::Job> job = GetServices()->GetJobSystem().CreateJob();
			job->GetDelegate().SetCallback(this, ReceivingCallback);
			GetServices()->GetJobSystem().SubmitJob(jsys::JobPriority::Normal, job);
		}

	public:
		NativeSocket(mem::sptr<Context> context): Socket(context), _protocol(Protocol::None), _keep_receiving(false)
		{
			Close();
		}

		virtual ~NativeSocket()
		{
			Close();
		}

		virtual void Open(Protocol protocol) override
		{
			NP_ENGINE_PROFILE_FUNCTION();
			Close();
			_protocol = protocol;

			i32 af = 0;
			i32 type = 0;
			i32 proto = 0;

			switch (_protocol)
			{
			case Protocol::Tcp:
				af = AF_INET;
				type = SOCK_STREAM;
				proto = IPPROTO_TCP;
				break;

			case Protocol::Udp: // TODO: NOT IMPLEMENTED YET
				af = AF_INET;
				type = SOCK_DGRAM;
				proto = IPPROTO_UDP;
				break;

			default:
				NP_ENGINE_ASSERT(false, "NativeSocket received unsupported protocol: " + to_str((ui32)_protocol));
				break;
			}

			con::vector<ui64> rejections;
			while (_socket == INVALID_SOCKET)
			{
				_socket = socket(af, type, proto);
				// linux may return 0/1/2(stdin/out/err) so we reject them on all platforms
				if (_socket == 0 || _socket == 1 || _socket == 2)
				{
					rejections.emplace_back(_socket);
					_socket = INVALID_SOCKET;
				}
			}

			for (ui64 r : rejections)
			{
#if NP_ENGINE_PLATOFORM_IS_WINDOWS
				closesocket(r);
#elif NP_ENGINE_PLATFORM_IS_LINUX
				close(r);
#else
	#error // TODO: implement native networking
#endif
			}
		}

		virtual void Close() override
		{
			NP_ENGINE_PROFILE_FUNCTION();
			StopReceiving();

			if (IsOpen())
			{
				shutdown(_socket, SD_BOTH);
#if NP_ENGINE_PLATFORM_IS_WINDOWS
				closesocket(_socket);
#elif NP_ENGINE_PLATFORM_IS_LINUX
				close(_socket);
#else
	#error // TODO: implement native networking
#endif
			}

			_socket = INVALID_SOCKET;
			_protocol = Protocol::None;
		}

		virtual void Enable(con::vector<SocketOptions> options) override
		{
			const i32 enable = 1;
			const siz enable_size = sizeof(i32);
			for (auto it = options.begin(); it != options.end(); it++)
			{
				switch (*it)
				{
				case SocketOptions::ReuseAddress:
				{
					setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &enable, enable_size);
					break;
				}
				case SocketOptions::ReusePort:
				{
					setsockopt(_socket, SOL_SOCKET, SO_REUSEPORT, &enable, enable_size);
					break;
				}
				default:
					break;
				}
			}
		}

		virtual void Disable(con::vector<SocketOptions> options) override
		{
			const i32 disable = 1;
			const siz disable_size = sizeof(i32);
			for (auto it = options.begin(); it != options.end(); it++)
			{
				switch (*it)
				{
				case SocketOptions::ReuseAddress:
				{
					setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &disable, disable_size);
					break;
				}
				case SocketOptions::ReusePort:
				{
					setsockopt(_socket, SOL_SOCKET, SO_REUSEPORT, &disable, disable_size);
					break;
				}
				default:
					break;
				}
			}
		}

		virtual bl IsOpen() const override
		{
			return _socket != INVALID_SOCKET;
		}

		virtual void BindTo(const Ip& ip, ui16 port) override
		{
			NP_ENGINE_PROFILE_FUNCTION();
			if (IsOpen())
			{
				sockaddr_in saddrin{};
				switch (_protocol)
				{
				case Protocol::Tcp:
				{
					saddrin.sin_family = AF_INET;
					Ipv4& ipv4 = (Ipv4&)ip;
					mem::CopyBytes(&saddrin.sin_addr.s_addr, ipv4.bytes.data(), ipv4.bytes.size());
					saddrin.sin_port = htons(port);
					break;
				}
				default:
					break;
				}

				if (bind(_socket, (sockaddr*)&saddrin, sizeof(sockaddr_in)) != 0)
					Close();
			}
		}

		virtual void Listen() override
		{
			NP_ENGINE_PROFILE_FUNCTION();
			if (IsOpen())
			{
				if (listen(_socket, SOMAXCONN) != 0)
					Close();
			}
		}

		virtual mem::sptr<Socket> Accept(bl enable_client_resolution = false) override
		{
			NP_ENGINE_PROFILE_FUNCTION();
			mem::sptr<Socket> client = nullptr;
			if (IsOpen())
			{
				sockaddr_in saddrin{};
#if NP_ENGINE_PLATFORM_IS_WINDOWS
				i32 saddrin_size = sizeof(sockaddr_in);
#elif NP_ENGINE_PLATFORM_IS_LINUX
				ui32 saddrin_size = sizeof(sockaddr_in);
#else
	#error // TODO: implement native networking
#endif
				client = Create(GetContext());
				if (client)
					((NativeSocket&)*client)._socket = accept(_socket, (sockaddr*)&saddrin, &saddrin_size);

				mem::sptr<srvc::Services> services = GetServices();
				mem::Allocator& allocator = services->GetAllocator();
				evnt::EventQueue& event_queue = services->GetEventQueue();

				mem::sptr<Host> host = mem::create_sptr<Host>(allocator);
				ui16 port = ntohs(saddrin.sin_port);

				switch (saddrin.sin_family)
				{
				case AF_INET:
				{
					Ipv4 ipv4;
					mem::CopyBytes(ipv4.bytes.data(), &saddrin.sin_addr.s_addr, ipv4.bytes.size());
					host->ipv4s.emplace(ipv4, port);
					break;
				}
				case AF_INET6: // TODO: NOT IMPLEMENTED YET
				{
					Ipv6 ipv6;
					mem::CopyBytes(ipv6.shorts.data(), &saddrin.sin_addr.s_addr, ipv6.shorts.size() * sizeof(ui16));
					host->ipv6s.emplace(ipv6, port);
					break;
				}
				default:
					break;
				}

				if (enable_client_resolution)
				{
					mem::sptr<Resolver> resolver = net::Resolver::Create(_context);
					if (!host->ipv4s.empty())
						*host = resolver->GetHost(host->ipv4s.begin()->first);
					else if (!host->ipv6s.empty())
						*host = resolver->GetHost(host->ipv6s.begin()->first);
				}

				event_queue.Push(mem::create_sptr<NetworkClientEvent>(allocator, host, client));
			}

			return client;
		}

		virtual void ConnectTo(const Ip& ip, ui16 port) override
		{
			NP_ENGINE_PROFILE_FUNCTION();
			if (IsOpen())
			{
				sockaddr_in saddrin{};
				switch (_protocol)
				{
				case Protocol::Tcp:
				{
					saddrin.sin_family = AF_INET;
					Ipv4& ipv4 = (Ipv4&)ip;
					mem::CopyBytes(&saddrin.sin_addr.s_addr, ipv4.bytes.data(), ipv4.bytes.size());
					saddrin.sin_port = htons(port);
					break;
				}
				default:
					break;
				}

				if (connect(_socket, (sockaddr*)&saddrin, sizeof(sockaddr_in)) != 0)
					Close();
			}
		}

		virtual void StartReceiving() override
		{
			NP_ENGINE_PROFILE_FUNCTION();
			if (IsOpen())
			{
				bl expected = false;
				if (_keep_receiving.compare_exchange_strong(expected, true, mo_release, mo_relaxed))
					SubmitReceivingJob();
			}
		}

		virtual bl IsRecieving() const override
		{
			return _keep_receiving.load(mo_release);
		}

		virtual void StopReceiving() override // TODO: implement
		{
			// TODO: this could affect Close()?
			_keep_receiving.store(false, mo_release);
		}
	};
} // namespace np::net::__detail

#endif /* NP_ENGINE_NETWORK_WINDOWS_SOCKET_HPP */