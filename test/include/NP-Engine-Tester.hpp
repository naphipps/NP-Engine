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
		graphics::Scene* _scene;
		graphics::Camera _camera;
		str _model_filename;
		str _model_texture_filename;
		graphics::Model _model;
		graphics::RenderableModel* _renderable_model;
		ecs::Entity _model_entity;
		time::SteadyTimestamp _start_timestamp;

		virtual void AdjustForWindowClose(event::Event& e)
		{
			if (_scene != nullptr &&
				_scene->GetRenderer().IsAttachedToWindow(*e.RetrieveData<window::WindowCloseEvent::DataType>().window))
			{
				_scene->Dispose();
				_scene = nullptr; // TODO: destroy content for scene
				_model_entity.Clear();
			}
		}

		virtual void HandleEvent(event::Event& e) override
		{
			switch (e.GetType())
			{
			case event::EventType::WindowClose:
				AdjustForWindowClose(e);
				break;
			default:
				break;
			}
		}

		void UpdateMetaValuesOnFrame(memory::Delegate& d)
		{
			graphics::ObjectMetaValues& meta_values = _renderable_model->GetMetaValues();
			
			time::DurationMilliseconds duration = time::SteadyClock::now() - _start_timestamp;
			flt seconds = duration.count() / 1000.0f;
			meta_values.object.Model = glm::rotate(glm::mat4(1.0f), seconds * glm::radians(90.0f) / 4.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		}

	public:
		GameLayer(event::EventSubmitter& event_submitter, ::entt::registry& ecs_registry):
			Layer(event_submitter, ecs_registry),
			_scene(nullptr),
			_model_filename(fs::Append(fs::Append(fs::Append(NP_ENGINE_WORKING_DIR, "test"), "assets"), "viking_room.obj")),
			_model_texture_filename(
				fs::Append(fs::Append(fs::Append(NP_ENGINE_WORKING_DIR, "test"), "assets"), "viking_room.png")),
			_model(_model_filename, graphics::Image(_model_texture_filename)),
			_renderable_model(graphics::RenderableModel::Create(memory::DefaultTraitAllocator, _model)),
			_model_entity(ecs_registry),
			_start_timestamp(time::SteadyClock::now())
		{
			_renderable_model->GetUpdateMetaValuesOnFrameDelegate().Connect<GameLayer, &GameLayer::UpdateMetaValuesOnFrame>(this);
			_model_entity.Add<graphics::RenderableObject*>(_renderable_model);
		}

		~GameLayer()
		{
			// TODO: we might not be able to reach our rhi destructors this way
			memory::Destroy<graphics::RenderableModel>(memory::DefaultTraitAllocator, _renderable_model);
		}

		void SetScene(graphics::Scene& scene)
		{
			_scene = memory::AddressOf(scene);
			// TODO: init scene, and destroy content for last scene??
			_scene->Prepare();
			_scene->SetCamera(_camera);
		}

		void Update(time::DurationMilliseconds time_delta) override
		{
			if (_scene != nullptr)
			{
				_scene->SetCamera(_camera); // TODO: our scene needs a camera
			}
		}

		virtual event::EventCategory GetHandledCategories() const override
		{
			return event::EventCategory::Window;
		}
	};

	class GameApp : public Application
	{
	private:
		memory::Allocator& _allocator;
		GameLayer _game_layer;

	public:
		GameApp(memory::Allocator& application_allocator):
			Application(Application::Properties{"My Game App"}),
			_allocator(application_allocator),
			_game_layer(_event_submitter, _ecs_registry)
		{
			PushLayer(memory::AddressOf(_game_layer));
		}

		void Run(i32 argc, chr** argv) override
		{
			::std::cout << "hello world from my game app!\n";
			::std::cout << "my title is '" << GetTitle() << "'\n";

			window::Window::Properties window_properties;
			window_properties.Title = "My Game Window >:D";
			graphics::Scene* scene = CreateWindowScene(window_properties);
			_game_layer.SetScene(*scene);

			Application::Run(argc, argv);
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_TESTER_HPP */