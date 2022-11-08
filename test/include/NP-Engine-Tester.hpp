//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/1/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_TESTER_HPP
#define NP_ENGINE_TESTER_HPP

#include <iostream>

#include <NP-Engine/NP-Engine.hpp>

namespace np::app
{
	class GameLayer : public Layer
	{
	private:
		WindowLayer& _window_layer;
		GraphicsLayer& _graphics_layer;
		win::Window* _window;
		gfx::Renderer* _renderer;
		gfx::Scene* _scene;
		gfx::Camera _camera;
		str _model_filename;
		str _model_texture_filename;
		gfx::Model _model;
		gfx::RenderableModel* _renderable_model;
		ecs::Entity _model_entity;
		tim::SteadyTimestamp _start_timestamp;
		flt _rate = 1.f;

		void AdjustForWindowClosingProcedure(mem::Delegate& d)
		{
			if (_window == d.GetData<win::Window*>())
			{
				_scene = nullptr;
				_renderer = nullptr;
				_window = nullptr;
			}
		}

		virtual void AdjustForWindowClosing(evnt::Event& e)
		{
			win::WindowClosingEvent::DataType& closing_data = e.GetData<win::WindowClosingEvent::DataType>();

			jsys::Job* adjust_job = _services.GetJobSystem().CreateJob();
			adjust_job->GetDelegate().SetData<win::Window*>(closing_data.window);
			adjust_job->GetDelegate().Connect<GameLayer, &GameLayer::AdjustForWindowClosingProcedure>(this);

			closing_data.job->AddDependency(*adjust_job);
			_services.GetJobSystem().SubmitJob(jsys::JobPriority::Higher, adjust_job);
		}

		void AdjustForWindowClosedProcedure(mem::Delegate& d)
		{
			win::Window* window = d.GetData<win::Window*>();

			// TODO: destroy content for scene
			_model_entity.clear();
		}

		virtual void AdjustForWindowClosed(evnt::Event& e)
		{
			win::WindowClosedEvent::DataType& closing_data = e.GetData<win::WindowClosedEvent::DataType>();

			jsys::Job* adjust_job = _services.GetJobSystem().CreateJob();
			adjust_job->GetDelegate().SetData<win::Window*>(closing_data.window);
			adjust_job->GetDelegate().Connect<GameLayer, &GameLayer::AdjustForWindowClosedProcedure>(this);

			_services.GetJobSystem().SubmitJob(jsys::JobPriority::Higher, adjust_job);
		}

		virtual void HandleEvent(evnt::Event& e) override
		{
			switch (e.GetType())
			{
			case evnt::EventType::WindowClosing:
				AdjustForWindowClosing(e);
				break;

			case evnt::EventType::WindowClosed:
				AdjustForWindowClosed(e);
				break;

			default:
				break;
			}
		}

		void SceneOnDraw(mem::Delegate& d)
		{
			nput::InputQueue& input = _services.GetInputQueue();
			const nput::KeyCodeStates& key_states = input.GetKeyCodeStates();

			if (key_states[nput::KeyCode::A].IsActive())
				_camera.Position.x -= _rate;

			if (key_states[nput::KeyCode::D].IsActive())
				_camera.Position.x += _rate;

			if (key_states[nput::KeyCode::W].IsActive())
				_camera.Position.y += _rate;

			if (key_states[nput::KeyCode::S].IsActive())
				_camera.Position.y -= _rate;

			if (key_states[nput::KeyCode::Q].IsActive())
				_camera.Position.z += _rate;

			if (key_states[nput::KeyCode::E].IsActive())
				_camera.Position.z -= _rate;

			if (key_states[nput::KeyCode::R].IsActive() && _rate < DBL_MAX)
				_rate += 0.1;

			if (key_states[nput::KeyCode::T].IsActive() && _rate > 0)
				_rate -= 0.1;

			if (key_states[nput::KeyCode::O].IsActive())
				_camera.SetProjectionType(gfx::Camera::ProjectionType::Orthographic);

			if (key_states[nput::KeyCode::P].IsActive())
				_camera.SetProjectionType(gfx::Camera::ProjectionType::Perspective);

			if (_scene)
				_scene->SetCamera(_camera);
		}

		void UpdateMetaValuesOnFrame(mem::Delegate& d)
		{
			gfx::RenderableMetaValues& meta_values = _renderable_model->GetMetaValues();
			flt scale = _camera.GetProjectionType() == gfx::Camera::ProjectionType::Perspective ? 10.f : 100.f;

			meta_values.object.Model = glm::mat4(1.0f);
			meta_values.object.Model = ::glm::scale(meta_values.object.Model, ::glm::vec3(scale, scale, scale));
			// meta_values.object.Model = ::glm::translate(meta_values.object.Model, ::glm::vec3(-10, -10, -10));
			meta_values.object.Model = ::glm::rotate(meta_values.object.Model, 90.f, _camera.Up);
		}

	public:
		GameLayer(srvc::Services& services, WindowLayer& window_layer, GraphicsLayer& graphics_layer) :
			Layer(services),
			_window_layer(window_layer),
			_graphics_layer(graphics_layer),
			_window(nullptr),
			_renderer(nullptr),
			_scene(nullptr),
			_model_filename(
				fsys::Append(fsys::Append(fsys::Append(NP_ENGINE_WORKING_DIR, "test"), "assets"), "viking_room.obj")),
			_model_texture_filename(
				fsys::Append(fsys::Append(fsys::Append(NP_ENGINE_WORKING_DIR, "test"), "assets"), "viking_room.png")),
			_model(_model_filename, _model_texture_filename, true),
			_renderable_model(gfx::RenderableModel::Create(_services, _model)),
			_model_entity(_services.GetEcsRegistry()),
			_start_timestamp(tim::SteadyClock::now())
		{
			_model.GetTexture().SetHotReloadable();
			_renderable_model->GetUpdateMetaValuesOnFrameDelegate().Connect<GameLayer, &GameLayer::UpdateMetaValuesOnFrame>(
				this);

			_model_entity.add<gfx::RenderableObject*>(_renderable_model);

			_camera.Eye = {30.f, 30.f, 30.f};
			_camera.Fovy = 70.f;
			_camera.NearPlane = 0.01f;
			_camera.FarPlane = 1000.0f;
		}

		~GameLayer()
		{
			mem::Destroy<gfx::RenderableModel>(_services.GetAllocator(), _renderable_model);
		}

		void PrepareForRun()
		{
			//TODO: I think CreateWindowScene should be replaced with WindowLayer.CreateWindow and GraphicsLayer.CreateScene
			//TODO: ^ this should be like the following:
			/*

				win::Window* window = _window_layer.CreateWindow(properties);
				gfx::Renderer* renderer = _graphics_layer.CreateRenderer(*window);
				gfx::Scene* scene = _graphics_layer.CreateScene(*renderer);
				//set our viewport shapes for each scene so we can have minimaps, etc

			*/

			void* caller = mem::AddressOf(_services.GetInputQueue());
			win::Window::Properties window_properties;
			window_properties.Title = "My Game Window >:D";

			_window = _window_layer.CreateWindow(window_properties);
			_window->SetKeyCodeCallback(caller, nput::InputListener::SubmitKeyCodeState);
			_window->SetMouseCodeCallback(caller, nput::InputListener::SubmitMouseCodeState);
			_window->SetMousePositionCallback(caller, nput::InputListener::SubmitMousePosition);
			_window->SetControllerCodeCallback(caller, nput::InputListener::SubmitControllerCodeState);

			_renderer = _graphics_layer.CreateRenderer(*_window);
			_scene = _graphics_layer.CreateScene(*_renderer);


			// TODO: init scene, and destroy content for last scene??
			_scene->GetOnDrawDelegate().Connect<GameLayer, &GameLayer::SceneOnDraw>(this); //TODO: do we need this??
			_scene->Prepare();
		}

		void Update(tim::DblMilliseconds time_delta) override {}

		virtual evnt::EventCategory GetHandledCategories() const override
		{
			return evnt::EventCategory::Window;
		}
	};

	class GameApp : public Application
	{
	private:
		GameLayer _game_layer;

	public:
		GameApp(::np::srvc::Services& app_services):
			Application(Application::Properties{"My Game App"}, app_services),
			_game_layer(app_services, _window_layer, _graphics_layer)
		{
			PushLayer(mem::AddressOf(_game_layer));
		}

		void Run(i32 argc, chr** argv) override
		{
			NP_ENGINE_LOG_INFO("Hello world from my game app! My title is '" + GetTitle() + "'");
			_game_layer.PrepareForRun();
			Application::Run(argc, argv);
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_TESTER_HPP */