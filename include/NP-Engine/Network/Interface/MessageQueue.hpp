//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/8/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_NETWORK_INTERFACE_MESSAGE_QUEUE_HPP
#define NP_ENGINE_NETWORK_INTERFACE_MESSAGE_QUEUE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "Message.hpp"

namespace np::net
{
	class MessageQueue
	{
	protected:
		using MessagesQueue = mutexed_wrapper<con::queue<Message>>;
		atm_bl _flag;
		con::array<MessagesQueue, 2> _messages_queues;

		MessagesQueue& GetQueue(bl flag)
		{
			return flag ? _messages_queues.front() : _messages_queues.back();
		}

	public:
		MessageQueue(): _flag(true) {}

		void ToggleState()
		{
			bl flag = _flag.load(mo_acquire);
			while (!_flag.compare_exchange_weak(flag, !flag, mo_release, mo_relaxed))
			{}
		}

		void Push(Message msg)
		{
			GetQueue(_flag.load(mo_acquire)).get_access()->emplace(msg);
		}

		Message Pop()
		{
			Message msg;
			auto queue = GetQueue(!_flag.load(mo_acquire)).get_access();
			if (!queue->empty())
			{
				msg = queue->front();
				queue->pop();
			}
			return msg;
		}

		void Clear()
		{
			for (auto it = _messages_queues.begin(); it != _messages_queues.end(); it++)
			{
				auto queue = it->get_access();
				while (!queue->empty())
					queue->pop();
			}
		}
	};
} // namespace np::net

#endif /* NP_ENGINE_NETWORK_INTERFACE_MESSAGE_QUEUE_HPP */