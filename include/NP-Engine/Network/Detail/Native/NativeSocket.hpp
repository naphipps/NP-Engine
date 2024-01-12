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
		atm_bl _direct_mode;

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
			sockaddr_in saddrin4{};
			sockaddr_in6 saddrin6{};
			auto saddrin = ToSaddrin(ip, port, saddrin4, saddrin6);

			for (siz total = 0; total < byte_count;)
			{
				i32 sent = sendto(_socket, src + total, byte_count - total, 0, saddrin.first, saddrin.second);
				if (sent < 0)
				{
					// NP_ENGINE_LOG_ERROR("SendBytesTo failed: " + to_str(sent));
					Close();
					break;
				}
				total += sent;
			}
		}

		i32 RecvBytes(chr* dst, siz byte_count, const bl direct_mode = false)
		{
			i32 total = 0;
			while (total < byte_count)
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
					total = -1;
					break;
				}
				total += recvd;

				if (direct_mode)
					break;
			}
			return total;
		}

		virtual void DetailSend(Message msg) override
		{
			NP_ENGINE_PROFILE_FUNCTION();
			if (IsOpen() && msg)
			{
				if (_direct_mode.load(mo_acquire))
				{
					if (msg.header.bodySize > 0)
						SendBytes((chr*)msg.body->GetData(), msg.header.bodySize);
				}
				else
				{
					SendBytes((chr*)&msg.header, sizeof(MessageHeader));
					if (msg.header.bodySize > 0)
						SendBytes((chr*)msg.body->GetData(), msg.header.bodySize);
				}
			}
		}

		virtual void DetailSendTo(Message msg, const Ip& ip, ui16 port) override
		{
			NP_ENGINE_PROFILE_FUNCTION();
			if (IsOpen() && msg)
			{
				if (_direct_mode.load(mo_acquire))
				{
					if (msg.header.bodySize > 0)
						SendBytesTo((chr*)msg.body->GetData(), msg.header.bodySize, ip, port);
				}
				else
				{
					SendBytesTo((chr*)&msg.header, sizeof(MessageHeader), ip, port);
					if (msg.header.bodySize > 0)
						SendBytesTo((chr*)msg.body->GetData(), msg.header.bodySize, ip, port);
				}
			}
		}

		static void ReceivingCallback(mem::Delegate& d)
		{
			NativeSocket& self = *((NativeSocket*)d.GetPayload());
			Message msg;
			if (self._direct_mode.load(mo_acquire))
			{
				msg.header.type = MessageType::Blob;
				msg.header.bodySize = NP_ENGINE_NETWORK_MAX_MESSAGE_BODY_SIZE;
				msg.body = mem::create_sptr<BlobMessageBody>(self.GetServices()->GetAllocator());
				msg.body->SetSize(msg.header.bodySize);

				i32 recvd = self.RecvBytes((chr*)msg.body->GetData(), msg.header.bodySize, true);
				if (recvd > 0)
				{
					msg.header.bodySize = recvd;
					msg.body->SetSize(msg.header.bodySize);
				}
				else
				{
					msg.Invalidate();
				}
			}
			else
			{
				i32 recvd = self.RecvBytes((chr*)&msg.header, sizeof(MessageHeader));
				if (recvd > 0 && msg.header.bodySize > 0)
				{
					bl supported = true;
					switch (msg.header.type)
					{
					case MessageType::Text:
						msg.body = mem::create_sptr<TextMessageBody>(self.GetServices()->GetAllocator());
						msg.body->SetSize(msg.header.bodySize);
						break;

					case MessageType::Blob:
						msg.body = mem::create_sptr<BlobMessageBody>(self.GetServices()->GetAllocator());
						msg.body->SetSize(msg.header.bodySize);
						break;

					default:
						NP_ENGINE_LOG_CRITICAL("ReceivingProcedure found unsupported MessageType: " +
											   to_str((ui32)msg.header.type));
						supported = false;
						break;
					}

					if (supported)
						self.RecvBytes((chr*)msg.body->GetData(), msg.header.bodySize);
				}
			}

			if (msg)
			{
				self._inbox.Push(msg);
				if (self._keep_receiving.load(mo_acquire))
					self.SubmitReceivingJob();
			}
			else if (self._keep_receiving.load(mo_acquire))
			{
				thr::ThisThread::sleep_for(tim::DblMilliseconds(NP_ENGINE_NETWORK_SOCKET_RECEIVING_SLEEP_DURATION));
				self.SubmitReceivingJob();
			}
		}

		void SubmitReceivingJob()
		{
			mem::sptr<jsys::Job> job = GetServices()->GetJobSystem().CreateJob();
			job->GetDelegate().SetPayload(this);
			job->GetDelegate().SetCallback(ReceivingCallback);
			GetServices()->GetJobSystem().SubmitJob(jsys::JobPriority::Normal, job);
		}

	public:
		NativeSocket(mem::sptr<Context> context):
			Socket(context),
			_protocol(Protocol::None),
			_keep_receiving(false),
			_direct_mode(false)
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

		virtual void Enable(con::vector<SocketOptions> options, const bl enable = true) override
		{
			if (IsOpen())
			{
				for (auto it = options.begin(); it != options.end(); it++)
				{
					switch (*it)
					{
					case SocketOptions::ReuseAddress:
					{
						setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (chr*)&enable, sizeof(bl));
						break;
					}
					case SocketOptions::ReusePort:
					{
#if NP_ENGINE_PLATFORM_IS_WINDOWS
						i32 optname = SO_REUSEADDR;
#elif NP_ENGINE_PLATFORM_IS_LINUX
						i32 optname = SO_REUSEPORT;
#else
	#error implement native networking
#endif
						setsockopt(_socket, SOL_SOCKET, optname, (chr*)&enable, sizeof(bl));
						break;
					}
					case SocketOptions::Direct:
					{
						_direct_mode.store(true, mo_release);
						break;
					}
					default:
						break;
					}
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
				sockaddr_in saddrin4{};
				sockaddr_in6 saddrin6{};
				auto saddrin = ToSaddrin(ip, port, saddrin4, saddrin6);

				i32 err = bind(_socket, saddrin.first, saddrin.second);
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
				sockaddr_in saddrin4{};
				sockaddr_in6 saddrin6{};
				auto saddrin = ToSaddrin(ip, port, saddrin4, saddrin6);

				i32 err = connect(_socket, saddrin.first, saddrin.second);
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