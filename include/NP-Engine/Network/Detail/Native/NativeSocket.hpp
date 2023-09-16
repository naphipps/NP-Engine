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
		Protocol _protocol;
		atm_bl _keep_receiving;

		void SendBytes(const chr* src, siz byte_count)
		{
			NP_ENGINE_PROFILE_FUNCTION();
			for (siz total = 0; total < byte_count;)
			{
				i32 sent = send(_socket, src + total, byte_count - total, 0);
				if (sent < 0)
				{
					// NP_ENGINE_LOG_ERROR("SendBytes failed: " + to_str(sent));
					Close();
					break;
				}
				total += sent;
			}
		}

		void SendBytesTo(const chr* src, siz byte_count, const Ip& ip, ui16 port)
		{
			NP_ENGINE_PROFILE_FUNCTION();
			sockaddr_in saddrin = ToSaddrin(ip, port);
			for (siz total = 0; total < byte_count;)
			{
				i32 sent = sendto(_socket, src + total, byte_count - total, 0, (sockaddr*)&saddrin, sizeof(sockaddr_in));
				if (sent < 0)
				{
					// NP_ENGINE_LOG_ERROR("SendBytesTo failed: " + to_str(sent));
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
				i32 recvd = -1;
				switch (_protocol)
				{
				case Protocol::Tcp:
					recvd = recv(_socket, dst + total, byte_count - total, 0);
					break;
				case Protocol::Udp:
					recvd = recvfrom(_socket, dst + total, byte_count - total, 0, nullptr, nullptr);
					break;
				default:
					break;
				}

				if (recvd < 0)
				{
					// NP_ENGINE_LOG_ERROR("RecvBytes failed: " + to_str(recvd));
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

		virtual void DetailSendTo(Message msg, const Ip& ip, ui16 port) override
		{
			NP_ENGINE_PROFILE_FUNCTION();
			if (IsOpen() && msg)
			{
				SendBytesTo((chr*)&msg.header, sizeof(MessageHeader), ip, port);
				if (msg.header.bodySize > 0)
					SendBytesTo((chr*)msg.body->GetData(), msg.header.bodySize, ip, port);
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

			case Protocol::Udp:
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
#if NP_ENGINE_PLATFORM_IS_WINDOWS
				closesocket(r);
#elif NP_ENGINE_PLATFORM_IS_LINUX
				close(r);
#else
	#error implement native networking
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
	#error implement native networking
#endif
			}

			_socket = INVALID_SOCKET;
			_protocol = Protocol::None;
		}

		virtual void Enable(con::vector<SocketOptions> options) override
		{
#if NP_ENGINE_PLATFORM_IS_WINDOWS
			const chr enable = 1;
			const i32 enable_size = sizeof(i32);

#elif NP_ENGINE_PLATFORM_IS_LINUX
			const i32 enable = 1;
			const siz enable_size = sizeof(i32);

#else
	#error implement native networking
#endif
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
#if NP_ENGINE_PLATFORM_IS_WINDOWS
					setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &enable, enable_size);
#elif NP_ENGINE_PLATFORM_IS_LINUX
					setsockopt(_socket, SOL_SOCKET, SO_REUSEPORT, &enable, enable_size);
#else
	#error implement native networking
#endif
					break;
				}
				default:
					break;
				}
			}
		}

		virtual void Disable(con::vector<SocketOptions> options) override
		{
#if NP_ENGINE_PLATFORM_IS_WINDOWS
			const chr disable = 1;
			const i32 disable_size = sizeof(i32);

#elif NP_ENGINE_PLATFORM_IS_LINUX
			const i32 disable = 1;
			const siz disable_size = sizeof(i32);

#else
	#error implement native networking
#endif
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
#if NP_ENGINE_PLATFORM_IS_WINDOWS
					setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &disable, disable_size);
#elif NP_ENGINE_PLATFORM_IS_LINUX
					setsockopt(_socket, SOL_SOCKET, SO_REUSEPORT, &disable, disable_size);
#else
	#error implement native networking
#endif
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
				sockaddr_in saddrin = ToSaddrin(ip, port);
				i32 err = bind(_socket, (sockaddr*)&saddrin, sizeof(sockaddr_in));
				if (err)
				{
					// NP_ENGINE_LOG_ERROR("BindTo failed: " + to_str(err));
					Close();
				}
			}
		}

		virtual void Listen() override
		{
			NP_ENGINE_PROFILE_FUNCTION();
			if (IsOpen())
			{
				i32 err = listen(_socket, SOMAXCONN);
				if (err)
				{
					// NP_ENGINE_LOG_ERROR("Listen failed: " + to_str(err));
					Close();
				}
			}
		}

		virtual mem::sptr<Socket> Accept(bl enable_client_resolution = false) override
		{
			NP_ENGINE_PROFILE_FUNCTION();
			mem::sptr<Socket> client = nullptr;
			if (IsOpen())
			{
				mem::sptr<srvc::Services> services = GetServices();
				mem::Allocator& allocator = services->GetAllocator();
				evnt::EventQueue& event_queue = services->GetEventQueue();
				mem::sptr<Host> host = mem::create_sptr<Host>(allocator);

				client = Create(GetContext());
				if (client)
				{
					sockaddr_in saddrin{};
#if NP_ENGINE_PLATFORM_IS_WINDOWS
					i32 saddrin_size = sizeof(sockaddr_in);
#elif NP_ENGINE_PLATFORM_IS_LINUX
					ui32 saddrin_size = sizeof(sockaddr_in);
#else
	#error implement native networking
#endif
					NativeSocket& native_client = (NativeSocket&)*client;
					native_client._socket = accept(_socket, (sockaddr*)&saddrin, &saddrin_size);
					native_client._protocol = Protocol::Tcp;

					if (enable_client_resolution && native_client)
					{
						PopulateHost(saddrin, *host);
						mem::sptr<Resolver> resolver = net::Resolver::Create(_context);

						if (!host->ipv4s.empty())
							*host = resolver->GetHost(host->ipv4s.begin()->first);
						else if (!host->ipv6s.empty())
							*host = resolver->GetHost(host->ipv6s.begin()->first);
					}
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
				sockaddr_in saddrin = ToSaddrin(ip, port);
				i32 err = connect(_socket, (sockaddr*)&saddrin, sizeof(sockaddr_in));
				if (err)
				{
					// NP_ENGINE_LOG_ERROR("ConnectTo failed: " + to_str(err));
					Close();
				}
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

		virtual void StopReceiving() override
		{
			_keep_receiving.store(false, mo_release);
		}
	};
} // namespace np::net::__detail

#endif /* NP_ENGINE_NETWORK_WINDOWS_SOCKET_HPP */