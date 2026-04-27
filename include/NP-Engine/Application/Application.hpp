//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/9/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_APPLICATION_HPP
#define NP_ENGINE_APPLICATION_HPP

#ifndef NP_ENGINE_APPLICATION_LOOP_DURATION
	#define NP_ENGINE_APPLICATION_LOOP_DURATION 2 // milliseconds (4 -> 250 loops per second)
#endif

#include "NP-Engine/Foundation/Foundation.hpp"

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Time/Time.hpp"
#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/GPU/GPU.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Event/Event.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Thread/Thread.hpp"
#include "NP-Engine/Window/Window.hpp"
#include "NP-Engine/Services/Services.hpp"
#include "NP-Engine/System/System.hpp"

#include "ApplicationEvents.hpp"
#include "Layer.hpp"
#include "WindowLayer.hpp"
#include "AudioLayer.hpp"

namespace np::app
{
	namespace __detail
	{
		static inline void HandleTerminate() noexcept
		{
			::std::cerr << "The terminate function was called.\nLog file can be found here : " +
					nsit::log::get_file_logger_file_path()
						<< "\n";
			win::Popup::Show(nullptr, "NP-Engine Terminate Function Called",
						"Probably an unhandled exception was thrown.\nLog file can be found here : " +
							nsit::log::get_file_logger_file_path(),
						win::PopupStyle::Error, win::PopupButtons::Ok);
		}

		static inline void HandleSignal(i32 signal) noexcept
		{
			str signal_string = "";
			switch (signal)
			{
			case SIGINT:
				signal_string = "SIGINT";
				break;
			case SIGILL:
				signal_string = "SIGILL";
				break;
			case SIGFPE:
				signal_string = "SIGFPE";
				break;
			case SIGSEGV:
				signal_string = "SIGSEGV";
				break;
			case SIGTERM:
				signal_string = "SIGTERM";
				break;
			case SIGABRT:
				signal_string = "SIGABRT";
				break;
			default:
				signal_string = "UNKNOWN SIGNAL";
				break;
			}

			str message = signal_string + " was raised.\nLog file can be found here : " + nsit::log::get_file_logger_file_path();

			::std::cerr << message << "\n";
			win::Popup::Show(nullptr, "NP-Engine Signal Raised", message, win::PopupStyle::Error, win::PopupButtons::Ok);
		}
	} // namespace __detail

	class Application : public Layer
	{
	protected:
		str _title;
		WindowLayer _window_layer;
		AudioLayer _audio_layer;
		con::vector<Layer*> _layers;

		Application(str title, mem::sptr<srvc::Services> services):
			Layer(services),
			_title(title),
			_window_layer(services),
			_audio_layer(services)
		{
			NP_ENGINE_PROFILE_FUNCTION();
			sys::set_terminate_handler(__detail::HandleTerminate);
			sys::set_signal_handler(__detail::HandleSignal);

			PushLayer(this);
			PushLayer(_window_layer);
		}

		virtual void HandleApplicationCloseEvent(mem::sptr<evnt::Event> e)
		{
			StopRunning();
		}

		virtual void HandleEvent(mem::sptr<evnt::Event> e) override
		{
			evnt::EventType type = e->GetEventType();
			if (type.Contains(evnt::EventType::Close))
				HandleApplicationCloseEvent(e);
		}

		virtual void PushLayer(Layer* layer)
		{
			_layers.emplace_back(layer);
		}

		virtual void PushLayer(Layer& layer)
		{
			PushLayer(mem::address_of(layer));
		}

		/*
			publishes events bottom-up
		*/
		virtual void PublishEvents()
		{
			evnt::EventQueue& event_queue = _services->GetEventQueue();
			con::vector<Layer*>::reverse_iterator it{};

			event_queue.ToggleState();
			for (mem::sptr<evnt::Event> e = event_queue.Pop(); e; e = event_queue.Pop())
			{
				e->SetCanBeHandled(false);

				for (it = _layers.rbegin(); !e->IsHandled() && it != _layers.rend(); it++)
					(*it)->OnEvent(e);

				if (e->CanBeHandled())
					event_queue.Push(e);
			}
		}

		virtual tim::milliseconds GetPlatformDefaultApplicationLoopDuration() const;

	public:
		virtual ~Application() {}

		virtual str GetTitle() const
		{
			return _title;
		}

		virtual void Run(i32 argc, chr** argv) = 0;

		virtual void StopRunning() = 0;

		virtual bl IsRunning() const = 0;

		virtual bl CanHandle(evnt::EventType type) const override
		{
			return type.Contains(evnt::EventType::Application);
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_APPLICATION_HPP */
