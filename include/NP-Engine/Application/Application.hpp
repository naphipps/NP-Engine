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
#include "NP-Engine/Thread/Thread.hpp"
#include "NP-Engine/Time/Time.hpp"
#include "NP-Engine/Window/Window.hpp"
#include "NP-Engine/Services/Services.hpp"

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
		Properties _properties;
		WindowLayer _window_layer;
		GraphicsLayer _graphics_layer;
		con::vector<Layer*> _layers;
		con::vector<Layer*> _overlays;
		atm_bl _running;

		Application(const Application::Properties& application_properties, srvc::Services& application_services):
			Layer(application_services),
			_properties(application_properties),
			_window_layer(application_services),
			_graphics_layer(application_services),
			_running(false)
		{
			system::SetTerminateHandler(__detail::HandleTerminate);
			system::SetSignalHandler(__detail::HandleSignal);

			_layers.emplace_back(this);
			_layers.emplace_back(mem::AddressOf(_window_layer));
			_layers.emplace_back(mem::AddressOf(_graphics_layer));
		}

		virtual WindowLayer& GetWindowLayer()
		{
			return _window_layer;
		}

		virtual GraphicsLayer& GetGraphicsLayer()
		{
			return _graphics_layer;
		}

		void HandlePopup(evnt::Event& e)
		{
			ApplicationPopupEvent::DataType& data = e.RetrieveData<ApplicationPopupEvent::DataType>();
			data.select = Popup::Show(GetTitle(), data.message, data.style, data.buttons);
			e.InvokeConnectedFunction();
			e.SetHandled();
		}

		void HandleEvent(evnt::Event& e) override
		{
			switch (e.GetType())
			{
			case evnt::EventType::ApplicationClose:
				StopRunning();
				e.SetHandled();
				break;
			case evnt::EventType::ApplicationPopup:
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

		gfx::Scene* CreateWindowScene(win::Window::Properties& window_properties)
		{
			win::Window* w = _window_layer.CreateWindow(window_properties);
			_graphics_layer.CreateRenderer(*w);
			return _graphics_layer.AcquireScene();
		}

		void DestroyWindowScene(gfx::Scene& scene)
		{
			// TODO: do we need this??
		}

	public:
		virtual ~Application() {}

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
			evnt::EventQueue& event_queue = _services.GetEventQueue();

			const tim::DurationMilliseconds max_loop_duration(NP_ENGINE_APPLICATION_LOOP_DURATION);

			tim::SteadyTimestamp loop_next_timestamp = tim::SteadyClock::now();
			tim::SteadyTimestamp loop_prev_timestamp = tim::SteadyClock::now();
			tim::DurationMilliseconds loop_duration(0);

			tim::SteadyTimestamp update_next_timestamp = tim::SteadyClock::now();
			tim::SteadyTimestamp update_prev_timestamp = tim::SteadyClock::now();
			tim::DurationMilliseconds update_duration(0);

			while (_running.load(mo_acquire))
			{
				update_next_timestamp = tim::SteadyClock::now();
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

				for (evnt::Event* e = event_queue.PopOther(); e != nullptr; e = event_queue.PopOther())
				{
					e->SetCanBeHandled(false);

					for (auto it = _overlays.rbegin(); !e->IsHandled() && it != _overlays.rend(); it++)
						(*it)->OnEvent(*e);

					for (auto it = _layers.rbegin(); !e->IsHandled() && it != _layers.rend(); it++)
						(*it)->OnEvent(*e);

					if (!e->CanBeHandled())
						event_queue.DestroyEvent(e);
					else
						event_queue.Emplace(e);
				}
				event_queue.SwapBuffers();

				for (Layer* layer : _layers)
					layer->Cleanup();
				for (Layer* overlay : _overlays)
					overlay->Cleanup();

				_graphics_layer.Render();

				loop_next_timestamp = tim::SteadyClock::now();
				loop_duration = loop_next_timestamp - loop_prev_timestamp;
				loop_prev_timestamp = loop_next_timestamp;
				if (loop_duration < max_loop_duration)
				{
					thr::ThisThread::yield();
					thr::ThisThread::sleep_for(max_loop_duration - loop_duration);
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

		evnt::EventCategory GetHandledCategories() const override
		{
			return evnt::EventCategory::Application;
		}

		bl IsRunning() const
		{
			return _running.load(mo_acquire);
		}
	};

	Application* CreateApplication(srvc::Services& application_services);
} // namespace np::app

#endif /* NP_ENGINE_APPLICATION_HPP */
