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

#ifndef NP_ENGINE_APPLICATION_LOOP_DURATION
	#define NP_ENGINE_APPLICATION_LOOP_DURATION 0 // milliseconds
#endif

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

		Application(const Application::Properties& app_properties, srvc::Services& services):
			Layer(services),
			_properties(app_properties),
			_window_layer(services),
			_graphics_layer(services),
			_running(false)
		{
			NP_ENGINE_PROFILE_FUNCTION();
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
			e();
			e.SetHandled();
		}

		void HandleApplicationClose(evnt::Event& e)
		{
			StopRunning();
			e.SetHandled();
		}

		void HandleEvent(evnt::Event& e) override
		{
			switch (e.GetType())
			{
			case evnt::EventType::ApplicationClose:
				HandleApplicationClose(e);
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

		virtual void CustomizeJobSystem()
		{
			jsys::JobSystem& job_system = _services.GetJobSystem();
			con::vector<jsys::JobWorker>& job_workers = job_system.GetJobWorkers();

			NP_ENGINE_ASSERT(thr::Thread::HardwareConcurrency() >= 4, "NP Engine requires at least four cores");

			using Fetch = jsys::JobWorker::Fetch;
			// using FetchOrderArray = jsys::JobWorker::FetchOrderArray;
			/*
				Examples:
					FetchOrderArray default_order{Fetch::Immediate, Fetch::PriorityBased, Fetch::Steal};
					FetchOrderArray thief_order{Fetch::Steal, Fetch::Immediate, Fetch::None};
					FetchOrderArray priority_order{Fetch::PriorityBased, Fetch::None, Fetch::None};
					FetchOrderArray immediate_order{Fetch::Immediate, Fetch::Steal, Fetch::None};
					FetchOrderArray only_immediate_order{ Fetch::Immediate, Fetch::Immediate, Fetch::Immediate };
			*/

			// rendering loop
			job_workers[0].SetFetchOrder({Fetch::Immediate, Fetch::None, Fetch::None});
			_graphics_layer.SetJobWorkerIndex(0);

			// all other workers
			for (siz i = 1; i < job_workers.size(); i++)
				job_workers[i].SetFetchOrder({Fetch::Immediate, Fetch::PriorityBased, Fetch::Steal});
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

			// app::Popup::Show("Application Start Of Run", "We're about to run");

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
			i64 i = 0;

			CustomizeJobSystem();
			_graphics_layer.SubmitRenderingJob();
			job_system.Start();

			while (_running.load(mo_acquire))
			{
				NP_ENGINE_PROFILE_SCOPE("loop");

				for (e = event_queue.Pop(); e; e = event_queue.Pop())
				{
					e->SetCanBeHandled(false);

					for (i = _overlays.size() - 1; !e->IsHandled() && i >= 0; i--)
						_overlays[i]->OnEvent(*e);

					for (i = _layers.size() - 1; !e->IsHandled() && i >= 0; i--)
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

				for (next = tim::SteadyClock::now(); next - prev < min_duration; next = tim::SteadyClock::now())
					thr::ThisThread::yield();
				prev = next;
			}

			_graphics_layer.StopRenderingJob();
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

	extern mem::sptr<Application> CreateApplication(srvc::Services& services);
} // namespace np::app

#endif /* NP_ENGINE_APPLICATION_HPP */
