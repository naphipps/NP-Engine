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
		gfx::Scene* _scene;
		gfx::Camera _camera;
		str _model_filename;
		str _model_texture_filename;
		gfx::Model _model;
		gfx::RenderableModel* _renderable_model;
		ecs::Entity _model_entity;
		tim::SteadyTimestamp _start_timestamp;

		virtual void AdjustForWindowClose(evnt::Event& e)
		{
			if (_scene != nullptr &&
				_scene->GetRenderer().IsAttachedToWindow(*e.GetData<win::WindowCloseEvent::DataType>().window))
			{
				_scene->Dispose();
				_scene = nullptr; // TODO: destroy content for scene
				_model_entity.Clear();
			}
		}

		virtual void HandleEvent(evnt::Event& e) override
		{
			switch (e.GetType())
			{
			case evnt::EventType::WindowClose:
				AdjustForWindowClose(e);
				break;
			default:
				break;
			}
		}

		void SceneOnDraw(mem::Delegate& d)
		{
			tim::DurationMilliseconds duration = tim::SteadyClock::now() - _start_timestamp;
			flt seconds = duration.count() / 1000.0f;
			flt s = ::std::sinf(seconds * 1.2f);
			flt e = s + 3.f;
			flt f = ::glm::radians(45.f + 20.f * (s + 1.f));

			_camera.Eye = {e, e, e};
			_camera.Fovy = f;

			if (_scene != nullptr)
			{
				_scene->SetCamera(_camera);
			}
		}

		void UpdateMetaValuesOnFrame(mem::Delegate& d)
		{
			gfx::RenderableMetaValues& meta_values = _renderable_model->GetMetaValues();
			tim::DurationMilliseconds duration = tim::SteadyClock::now() - _start_timestamp;
			flt seconds = duration.count() / 1000.0f;
			meta_values.object.Model =
				::glm::rotate(glm::mat4(1.0f), seconds * glm::radians(90.0f) / 4.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		}

	public:
		GameLayer(srvc::Services& services):
			Layer(services),
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

			_model_entity.Add<gfx::RenderableObject*>(_renderable_model);

			_camera.Eye = {2.0f, 2.0f, 2.0f};
			_camera.Fovy = ::glm::radians(45.0f);
			_camera.NearPlane = 0.1f;
			_camera.FarPlane = 10.0f;
		}

		~GameLayer()
		{
			mem::Destroy<gfx::RenderableModel>(_services.GetAllocator(), _renderable_model);
		}

		void SetScene(gfx::Scene& scene)
		{
			_scene = mem::AddressOf(scene);
			// TODO: init scene, and destroy content for last scene??

			_scene->GetOnDrawDelegate().Connect<GameLayer, &GameLayer::SceneOnDraw>(this);
			_scene->Prepare();
		}

		void Update(tim::DurationMilliseconds time_delta) override {}

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
			_game_layer(app_services)
		{
			PushLayer(mem::AddressOf(_game_layer));
		}

		void Run(i32 argc, chr** argv) override
		{
			NP_ENGINE_LOG_INFO("Hello world from my game app! My title is '" + GetTitle() + "'");

			win::Window::Properties window_properties;
			window_properties.Title = "My Game Window >:D";
			gfx::Scene* scene = CreateWindowScene(window_properties);
			_game_layer.SetScene(*scene);

			Application::Run(argc, argv);
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_TESTER_HPP */