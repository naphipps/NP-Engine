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
					nsit::Log::GetFileLoggerFilePath()
						<< "\n";
			Popup::Show("NP-Engine Terminate Function Called",
						"Probably an unhandled exception was thrown.\nLog file can be found here : " +
							nsit::Log::GetFileLoggerFilePath(),
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

			str message = signal_string + " was raised.\nLog file can be found here : " + nsit::Log::GetFileLoggerFilePath();

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

		Application(const Application::Properties& app_properties, srvc::Services& app_services):
			Layer(app_services),
			_properties(app_properties),
			_window_layer(app_services),
			_graphics_layer(app_services),
			_running(false)
		{
			sys::SetTerminateHandler(__detail::HandleTerminate);
			sys::SetSignalHandler(__detail::HandleSignal);

			_layers.emplace_back(this);
			_layers.emplace_back(mem::AddressOf(_window_layer));
			_layers.emplace_back(mem::AddressOf(_graphics_layer));
		}

		void HandlePopup(evnt::Event& e)
		{
			ApplicationPopupEvent::DataType& data = e.GetData<ApplicationPopupEvent::DataType>();
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

		//TODO: I think CreateWindowScene should be replaced with WindowLayer.CreateWindow and GraphicsLayer.CreateScene
		//TODO: ^ this should be like the following:
		/*
		
			win::Window* window = _window_layer.CreateWindow(properties);
			gfx::Renderer* renderer = _graphics_layer.CreateRenderer(*window);
			gfx::Scene* scene = _graphics_layer.CreateScene(*renderer);
			//set our viewport shapes for each scene so we can have minimaps, etc

		*/

		gfx::Scene* CreateWindowScene(win::Window::Properties& window_properties)
		{
			win::Window* w = _window_layer.CreateWindow(window_properties);

			w->SetListener(mem::AddressOf(_services.GetInputQueue()));

			_graphics_layer.CreateRenderer(*w);
			return _graphics_layer.AcquireScene();
		}

		void DestroyWindowScene(gfx::Scene& scene)
		{
			// TODO: do we need this??
		}

		virtual void CustomizeJobSystem()
		{
			jsys::JobSystem& job_system = _services.GetJobSystem();
			con::vector<jsys::JobWorker>& job_workers = job_system.GetJobWorkers();

			using Fetch = jsys::JobWorker::Fetch;
			con::array<Fetch, 3> default_order{Fetch::Immediate, Fetch::PriorityBased, Fetch::Steal};
			con::array<Fetch, 3> thief_order{Fetch::Steal, Fetch::Immediate, Fetch::None};
			con::array<Fetch, 3> priority_order{Fetch::PriorityBased, Fetch::None, Fetch::None};
			con::array<Fetch, 3> immediate_order{Fetch::Immediate, Fetch::Steal, Fetch::None};

			if (job_workers.size())
			{
				// TODO: we can customize how our job workers operate depending on the number of them
			}
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

			//TODO: turn our loop and rendering into a fixed step loop
			//TODO: ^ so we can set it to 60fps or something with 0 being infinite
		*/
		virtual void Run(i32 argc, chr** argv)
		{
			NP_ENGINE_PROFILE_SCOPE("application run");

			_running.store(true, mo_release);
			evnt::EventQueue& event_queue = _services.GetEventQueue();
			jsys::JobSystem& job_system = _services.GetJobSystem();
			nput::InputQueue& input_queue = _services.GetInputQueue();

			tim::SteadyTimestamp next = tim::SteadyClock::now();
			tim::SteadyTimestamp prev = next;
			const tim::DblMilliseconds min_duration(NP_ENGINE_APPLICATION_LOOP_DURATION);
			tim::SteadyTimestamp update_next = next;
			tim::SteadyTimestamp update_prev = next;
			tim::DblMilliseconds update_delta(0);

			evnt::Event* e = nullptr;
			siz i = 0;

			CustomizeJobSystem();
			job_system.Start();
			while (_running.load(mo_acquire))
			{
				NP_ENGINE_PROFILE_SCOPE("loop");

				for (e = event_queue.Pop(); e; e = event_queue.Pop())
				{
					e->SetCanBeHandled(false);

					for (i = _overlays.size() - 1; !e->IsHandled() && i < _overlays.size(); i--)
						_overlays[i]->OnEvent(*e);

					for (i = _layers.size() - 1; !e->IsHandled() && i < _layers.size(); i--)
						_layers[i]->OnEvent(*e);

					if (!e->CanBeHandled())
						event_queue.DestroyEvent(e);
					else if (!event_queue.Emplace(e))
						NP_ENGINE_ASSERT(false, "all events must emplace successfully here");
				}
				event_queue.SwapBuffers();

				if (!_running.load(mo_acquire))
					break;

				update_next = tim::SteadyClock::now();
				update_delta = update_next - update_prev;
				update_prev = update_next;

				input_queue.ApplySubmissions();

				for (i = 0; i < _layers.size(); i++)
					_layers[i]->BeforeUdpate();
				for (i = 0; i < _overlays.size(); i++)
					_overlays[i]->BeforeUdpate();

				for (i = 0; i < _layers.size(); i++)
					_layers[i]->Update(update_delta);
				for (i = 0; i < _overlays.size(); i++)
					_overlays[i]->Update(update_delta);

				for (i = 0; i < _layers.size(); i++)
					_layers[i]->AfterUdpate();
				for (i = 0; i < _overlays.size(); i++)
					_overlays[i]->AfterUdpate();

				for (i = 0; i < _layers.size(); i++)
					_layers[i]->Cleanup();
				for (i = 0; i < _overlays.size(); i++)
					_overlays[i]->Cleanup();

				_graphics_layer.Render();

				for (next = tim::SteadyClock::now(); next - prev < min_duration; next = tim::SteadyClock::now())
					thr::ThisThread::yield();

				prev = next;
			}

			job_system.Stop();
			job_system.Dispose();
			event_queue.Clear();
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

	Application* CreateApplication(srvc::Services& app_services);
} // namespace np::app

#endif /* NP_ENGINE_APPLICATION_HPP */
