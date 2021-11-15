//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/13/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_EVENT_MANAGER_HPP
#define NP_ENGINE_EVENT_MANAGER_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "EventImpl.hpp"
#include "EventQueue.hpp"
#include "EventHandler.hpp"

//TODO: add summary comments

namespace np::event
{
	class EventManager : public EventQueue
	{
	private:
		container::vector<EventHandler*> _handlers;

	public:

		void RegisterHandler(const EventHandler& handler)
		{
			_handlers.emplace_back(const_cast<EventHandler*>(memory::AddressOf(handler)));
		}

		void UnregisterHandler(const EventHandler& handler)
		{
			EventHandler* handler_address = const_cast<EventHandler*>(memory::AddressOf(handler));
			
			if (!_handlers.empty())
			{
				for (i64 i = _handlers.size() - 1; i >= 0; i--)
				{
					if (_handlers[i] == handler_address)
					{
						_handlers.erase(_handlers.begin() + i);
					}
				}
			}
		}

		void DispatchEvents()
		{
			for (Event* event = PopOther(); event != nullptr; event = PopOther())
			{
				for (auto it = _handlers.begin(); it != _handlers.end(); it++)
				{
					(*it)->OnEvent(*event);
				}

				if (event->IsHandled())
				{
					DestroyEvent(event);
				}
				else
				{
					if (!GetBuffer().enqueue(event))
					{
						NP_ASSERT(false, "Dispatched event was not handled and could not go to current buffer");
					}
				}
			}

			SwapBuffers();
		}
	};
}

#endif /* NP_ENGINE_EVENT_MANAGER_HPP */