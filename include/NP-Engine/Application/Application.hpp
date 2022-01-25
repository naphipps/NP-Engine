//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/9/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_APPLICATION_HPP
#define NP_ENGINE_APPLICATION_HPP

#include "NP-Engine/Foundation/Foundation.hpp"

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Time/Time.hpp"
#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/Graphics/Graphics.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Event/Event.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Concurrency/Concurrency.hpp"
#include "NP-Engine/Time/Time.hpp"
#include "NP-Engine/Window/Window.hpp"

#include "ApplicationEvents.hpp"
#include "Layer.hpp"
#include "WindowLayer.hpp"
#include "GraphicsLayer.hpp"
#include "Popup.hpp"

namespace np::app
{
	namespace __detail
	{
		static inline void HandleTerminate() noexcept
		{
			::std::cerr << "The terminate function was called.\nLog file can be found here : " +
					insight::Log::GetFileLoggerFilePath()
						<< "\n";
			Popup::Show("NP-Engine Terminate Function Called",
						"Probably an unhandled exception was thrown.\nLog file can be found here : " +
							insight::Log::GetFileLoggerFilePath(),
						Popup::Style::Error, Popup::Buttons::OK);
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

			str message = signal_string + " was raised.\nLog file can be found here : " + insight::Log::GetFileLoggerFilePath();

			::std::cerr << message << "\n";
			Popup::Show("NP-Engine Signal Raised", message, Popup::Style::Error, Popup::Buttons::OK);
		}
	} // namespace __detail

	class Application : public Layer
	{
	public:
		struct Properties
		{
			str Title = "";
		};

	protected:
		event::EventQueue _event_queue;
		event::EventSubmitter _application_event_submitter;
		Properties _properties;
		WindowLayer _window_layer;
		GraphicsLayer _graphics_layer;
		container::vector<Layer*> _layers;
		container::vector<Layer*> _overlays;
		atm_bl _running;

		Application(const Application::Properties& application_properties):
			_application_event_submitter(_event_queue), // order matters
			Layer(_application_event_submitter),
			_properties(application_properties),
			_window_layer(_application_event_submitter),
			_graphics_layer(_application_event_submitter),
			_running(false)
		{
			system::SetTerminateHandler(__detail::HandleTerminate);
			system::SetSignalHandler(__detail::HandleSignal);

			_layers.emplace_back(this);
			_layers.emplace_back(memory::AddressOf(_window_layer));
			_layers.emplace_back(memory::AddressOf(_graphics_layer));
		}

		virtual WindowLayer& GetWindowLayer()
		{
			return _window_layer;
		}

		virtual GraphicsLayer& GetGraphicsLayer()
		{
			return _graphics_layer;
		}

		void HandlePopup(event::Event& e)
		{
			ApplicationPopupEvent::DataType& data = e.RetrieveData<ApplicationPopupEvent::DataType>();
			data.select = Popup::Show(GetTitle(), data.message, data.style, data.buttons);
			e.InvokeConnectedFunction();
			e.SetHandled();
		}

		void HandleEvent(event::Event& e) override
		{
			switch (e.GetType())
			{
			case event::EventType::ApplicationClose:
				StopRunning();
				e.SetHandled();
				break;
			case event::EventType::ApplicationPopup:
				HandlePopup(e);
				break;
			default:
				break;
			}
		}

		void PushLayer(Layer* layer)
		{
			_layers.emplace_back(layer);
		}

		void PushOverlay(Layer* overlay)
		{
			_overlays.emplace_back(overlay);
		}

	public:
		void Run()
		{
			Run(0, nullptr);
		}

		/*
			inspired by the following:
				- http://www.gameprogrammingpatterns.com/game-loop.html
				- https://gafferongames.com/post/fix_your_timestep/
		*/
		virtual void Run(i32 argc, chr** argv)
		{
			// TODO: this into a fixed step loop - aka, set this to loop at 60fps or something with 0 being infinitely fast

			_running.store(true, mo_release);
			const time::DurationMilliseconds max_loop_duration(NP_ENGINE_APPLICATION_LOOP_DURATION);

			time::SteadyTimestamp loop_next_timestamp = time::SteadyClock::now();
			time::SteadyTimestamp loop_prev_timestamp = time::SteadyClock::now();
			time::DurationMilliseconds loop_duration(0);

			time::SteadyTimestamp update_next_timestamp = time::SteadyClock::now();
			time::SteadyTimestamp update_prev_timestamp = time::SteadyClock::now();
			time::DurationMilliseconds update_duration(0);

			while (_running.load(mo_acquire))
			{
				update_next_timestamp = time::SteadyClock::now();
				update_duration = update_next_timestamp - update_prev_timestamp;
				update_prev_timestamp = update_next_timestamp;

				for (Layer* layer : _layers)
					layer->BeforeUdpate();
				for (Layer* overlay : _overlays)
					overlay->BeforeUdpate();

				for (Layer* layer : _layers)
					layer->Update(update_duration);
				for (Layer* overlay : _overlays)
					overlay->Update(update_duration);

				for (Layer* layer : _layers)
					layer->AfterUdpate();
				for (Layer* overlay : _overlays)
					overlay->AfterUdpate();

				for (event::Event* e = _event_queue.PopOther(); e != nullptr; e = _event_queue.PopOther())
				{
					e->SetCanBeHandled(false);

					for (auto it = _overlays.rbegin(); !e->IsHandled() && it != _overlays.rend(); it++)
						(*it)->OnEvent(*e);

					for (auto it = _layers.rbegin(); !e->IsHandled() && it != _layers.rend(); it++)
						(*it)->OnEvent(*e);

					if (!e->CanBeHandled())
						_event_queue.DestroyEvent(e);
					else
						_event_queue.Emplace(e);
				}
				_event_queue.SwapBuffers();

				for (Layer* layer : _layers)
					layer->Cleanup();
				for (Layer* overlay : _overlays)
					overlay->Cleanup();

				_graphics_layer.Draw();

				loop_next_timestamp = time::SteadyClock::now();
				loop_duration = loop_next_timestamp - loop_prev_timestamp;
				loop_prev_timestamp = loop_next_timestamp;
				if (loop_duration < max_loop_duration)
				{
					concurrency::ThisThread::yield();
					concurrency::ThisThread::sleep_for(max_loop_duration - loop_duration);
				}
			}
		}

		Properties GetProperties() const
		{
			return _properties;
		}

		str GetTitle() const
		{
			return _properties.Title;
		}

		void StopRunning()
		{
			_running.store(false, mo_release);
		}

		event::EventCategory GetHandledCategories() const override
		{
			return event::EventCategory::Application;
		}

		bl IsRunning() const
		{
			return _running.load(mo_acquire);
		}
	};

	Application* CreateApplication(memory::Allocator& application_allocator);
} // namespace np::app

#endif /* NP_ENGINE_APPLICATION_HPP */
