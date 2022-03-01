//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/2/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine-Tester.hpp"

namespace np::app
{
	class GameLayer : public Layer
	{
	private:
		graphics::Scene* _scene;

		virtual void AdjustForWindowClose(event::Event& e)
		{
			if (_scene != nullptr &&
				_scene->GetRenderer().IsAttachedToWindow(*e.RetrieveData<window::WindowCloseEvent::DataType>().window))
			{
				_scene = nullptr; // TODO: destroy content for scene
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

	public:
		GameLayer(event::EventSubmitter& event_submitter): Layer(event_submitter), _scene(nullptr) {}

		void SetScene(graphics::Scene& scene)
		{
			_scene = memory::AddressOf(scene);
			// TODO: init scene, and destroy content for last scene??
		}

		void Update(time::DurationMilliseconds time_delta) override
		{
			if (_scene != nullptr)
			{
				// TODO: do stuff with the scene?
			}
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
			_game_layer(_event_submitter)
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

	Application* CreateApplication(memory::Allocator& application_allocator)
	{
		::std::cout << "hello world from game create application\n";

		memory::Block block = application_allocator.Allocate(sizeof(GameApp));
		memory::Construct<GameApp>(block, application_allocator);
		return static_cast<GameApp*>(block.ptr);
	}
} // namespace np::app
