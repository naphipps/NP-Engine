//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/6/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_NETWORK_HPP
#define NP_ENGINE_NETWORK_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Services/Services.hpp"

namespace np::net
{
    //TODO: implement networking interface

    enum class DetailType : ui32
    {
        None = 0,
        Asio,
        Curl
    };

    enum class MessageType : ui32
    {
        None = 0,
        Ping,
        Packet
    };

    using Packet = mem::SizedBlock<KILOBYTE_SIZE / 4>;

    struct Message
    {
        MessageType messageType = MessageType::None;
        mem::sptr<Packet> packet;
    };

    enum class Protocol : ui32
    {
        None = 0,
        Udp,
        Tcp
    };

    static str GetHostName()
    {
        return "";
    }

    struct IP {};

    struct IPv4 : public IP
    {
        con::array<ui8, 4> octave;

        operator str() const
        {
            str s = to_str(octave[0]);
            for (auto it = octave.begin(); it != octave.end(); it++)
                s += "." + to_str(*it);

            return s;
        }
    };

    struct IPv6 : public IP
    {
        con::array<ui8, 16> octave;

        operator str() const
        {
            str s = to_str(octave[0]);
            for (auto it = octave.begin(); it != octave.end(); it++)
                s += ":" + to_str(*it);

            return s;
        }
    };

    static void ResolveName(str name, IPv4& ipv4, IPv6& ipv6)
    {
        //TODO: get ip for name, store in ipv4 and ipv6
    }

    class Context
    {
    protected:
        mem::sptr<srvc::Services> _services;

        Context() {}

    public:
        static mem::sptr<Context> Create(DetailType detail_type, mem::sptr<srvc::Services> services);

        virtual DetailType GetDetailType() const
		{
			return DetailType::None;
		}

		virtual mem::sptr<srvc::Services> GetServices() const
		{
			return _services;
		}
    };

    class Socket //TODO: what is this??
    {
    protected:
        mem::sptr<Context> _context;
        mutexed_wrapper<con::queue<Message>> _incoming_messages;
        mutexed_wrapper<con::queue<Message>> _outgoing_messages;

        Socket(mem::sptr<Context> context): _context(context) {}

    public:
        static mem::sptr<Socket> Create(mem::sptr<Context> context, str host, ui16 port);

        ~Socket()
        {
            Disconnect();
        }

        void Disconnect()
        {
            //TODO: ??
        }

        void Send(Message msg)
        {
            //TODO:
        }

        Message Receive()
        {
            return {}; //TODO: 
        }
        
        operator bl() const
        {
            return true;
        }
    };

    class Network
    {
    protected:
        mem::sptr<Context> _context;
        con::vector<mem::sptr<Socket>> _sockets;
        
        Network(mem::sptr<Context> context) : _context(context)
        {

        }

    public:

        static mem::sptr<Network> Create(mem::sptr<Context> context);

        mem::sptr<ConnectionHandle> Connect(str host, ui16 port)
        {
            mem::sptr<Socket> socket = Socket::Create(_context, host, port); //TODO: make socket to host::port
            _sockets.emplace_back(socket);

            #error //TODO: we need to return a valid handle for this connection - just like UidSystem
        }

        void Update()
        {
            for (auto it = _sockets.begin(); it != _sockets.end();)
            {
                if (*it)
                {
                    //TODO: do stuff on socket - any incoming messages??
                    
                    it++;
                }
                else
                {
                    it = _sockets.erase(it);
                }
            }
        }

        void Send(mem::sptr<ConnectionHandle> connection, Message msg) //Syncronous send
        {
            //TODO: which connection to send message??
            
            if (msg.packet)
            {
                //TODO: prepare packet to send
            }



        }

        void Send(str url)
        {
            //TODO: I'd like to support just firing off messages at urls and calling callbacks to handle responses (async)
        }
    };

    

    /*
        

        Objects:
            IP {V4, V6}
            Protocal {Udp, Tcp}

        Name

        NameResolver

        Message
        {
            Header {TypeEnum}
            Body {void*} //size depends on TypeEnum
        }


        Network
        {
            Connect(Network& other);

            SendTo(Network& other, Message msg);

            ReceiveFrom(Network& other, Message msg);
        }

        Socket // What is this?!

    */
}

#endif /* NP_ENGINE_NETWORK_HPP */