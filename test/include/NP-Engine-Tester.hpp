//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/1/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_TESTER_HPP
#define NP_ENGINE_TESTER_HPP

//#define NP_ENGINE_ACCUMULATING_ALLOCATOR_BLOCK_SIZE (GIGABYTE_SIZE * 2) //TODO: test

#include <iostream>

#include <NP-Engine/NP-Engine.hpp>

namespace np::app
{
	class GameLayer : public Layer
	{
	private:
		WindowLayer& _window_layer;
		GraphicsLayer& _graphics_layer;
		mem::sptr<win::Window> _window;
		mem::sptr<gfx::Scene> _scene;
		gfx::Camera _camera;
		str _model_filename;
		str _model_texture_filename;
		mem::sptr<gfx::Model> _model;
		mem::sptr<uid::UidHandle> _model_handle;
		tim::SteadyTimestamp _start_timestamp;
		flt _rate = 10.f;

		static void AdjustForWindowClosingCallback(void* caller, mem::Delegate& d)
		{
			((GameLayer*)caller)->AdjustForWindowClosingProcedure(d);
		}

		void AdjustForWindowClosingProcedure(mem::Delegate& d)
		{
			uid::Uid windowId = d.GetData<uid::Uid>();

			//NP_ENGINE_LOG_INFO("reseting scene");

			if (_scene->GetRenderTarget()->GetWindow()->GetUid() == windowId)
				_scene.reset();

			if (_window->GetUid() == windowId)
				_window.reset();

			d.DestructData<uid::Uid>();
		}

		virtual void AdjustForWindowClosing(mem::sptr<evnt::Event> e)
		{
			win::WindowClosingEvent& closing_event = (win::WindowClosingEvent&)(*e);
			win::WindowClosingEvent::DataType& closing_data = closing_event.GetData();

			if (_window->GetUid() == closing_data.windowId)
			{
				// window ownership has gone to the closing job
				mem::sptr<win::Window>& closing_job_window = closing_data.job->GetDelegate().GetData<mem::sptr<win::Window>>();
				closing_job_window = _window;
			}

			mem::sptr<jsys::Job> adjust_job = _services->GetJobSystem().CreateJob();
			adjust_job->GetDelegate().ConstructData<uid::Uid>(closing_data.windowId);
			adjust_job->GetDelegate().SetCallback(this, AdjustForWindowClosingCallback);
			jsys::Job::AddDependency(closing_data.job, adjust_job);
			_services->GetJobSystem().SubmitJob(jsys::JobPriority::Higher, adjust_job);
		}

		virtual void HandleEvent(mem::sptr<evnt::Event> e) override
		{
			switch (e->GetType())
			{
			case evnt::EventType::WindowClosing:
				AdjustForWindowClosing(e);
				break;

			default:
				break;
			}
		}

		static void SceneOnDrawCallback(void* caller, mem::Delegate& d)
		{
			((GameLayer*)caller)->SceneOnDraw(d);
		}

		void SceneOnDraw(mem::Delegate& d)
		{
			if (_scene)
				_scene->SetCamera(_camera);
		}

	public:
		GameLayer(mem::sptr<srvc::Services> services, WindowLayer& window_layer, GraphicsLayer& graphics_layer):
			Layer(services),
			_window_layer(window_layer),
			_graphics_layer(graphics_layer),
			_window(nullptr),
			_scene(nullptr),
			_model_filename(
				fsys::Append(fsys::Append(fsys::Append(NP_ENGINE_WORKING_DIR, "test"), "assets"), "viking_room.obj")),
			_model_texture_filename(
				fsys::Append(fsys::Append(fsys::Append(NP_ENGINE_WORKING_DIR, "test"), "assets"), "viking_room.png")),
			_model(mem::create_sptr<gfx::Model>(_services->GetAllocator(), _model_filename, _model_texture_filename, true)),
			_model_handle(nullptr),
			_start_timestamp(tim::SteadyClock::now())
		{
			//_model->GetTexture().SetHotReloadable();
			//_renderable_model->GetUpdateMetaValuesOnFrameDelegate().SetCallback(this, UpdateMetaValuesOnFrameCallback);

			_camera.Eye = { 30.f, 30.f, 30.f };
			_camera.Fovy = 70.f;
			_camera.NearPlane = 0.01f;
			_camera.FarPlane = 1000.0f;
			_camera.LookAt = { 0, 0, 0 };

			//-----------------------------------------------------------

			win::Window::Properties window_properties;
			_window = win::Window::Create(win::WindowDetailType::Glfw, _services, window_properties);
			_window_layer.RegisterWindow(_window);

			mem::sptr<gfx::DetailInstance> detail_instance = gfx::DetailInstance::Create(gfx::GraphicsDetailType::Vulkan, _services);
			mem::sptr<gfx::RenderTarget> render_target = gfx::RenderTarget::Create(detail_instance, _window); //TODO: make sure we handle when window is closing
			mem::sptr<gfx::RenderDevice> render_device = gfx::RenderDevice::Create(render_target);
			mem::sptr<gfx::RenderContext> render_context = gfx::RenderContext::Create(render_device);
			mem::sptr<gfx::RenderPass> render_pass = gfx::RenderPass::Create(render_context);
			mem::sptr<gfx::Framebuffers> framebuffers = gfx::Framebuffers::Create(render_pass);

			gfx::Shader::Properties vertex_shader_properties;
			vertex_shader_properties.type = gfx::Shader::Type::Vertex;
			vertex_shader_properties.entrypoint = "main";
			vertex_shader_properties.filename = fsys::Append(fsys::Append("Vulkan", "shaders"), "object_vertex.glsl");
			mem::sptr<gfx::RenderShader> vertex_shader = gfx::RenderShader::Create(render_device, vertex_shader_properties);

			gfx::Shader::Properties fragment_shader_properties;
			fragment_shader_properties.type = gfx::Shader::Type::Fragment;
			fragment_shader_properties.entrypoint = "main";
			fragment_shader_properties.filename = fsys::Append(fsys::Append("Vulkan", "shaders"), "object_fragment.glsl");
			mem::sptr<gfx::RenderShader> fragment_shader = gfx::RenderShader::Create(render_device, fragment_shader_properties);

			gfx::RenderPipeline::Properties render_pipeline_properties{framebuffers, vertex_shader, fragment_shader};
			mem::sptr<gfx::RenderPipeline> render_pipeline = gfx::RenderPipeline::Create(render_pipeline_properties);

			gfx::Scene::Properties scene_properties{ render_pipeline, _camera };
			_scene = gfx::Scene::Create(scene_properties);
			_graphics_layer.RegisterScene(_scene);

			_model_handle = _services->GetUidSystem().CreateUidHandle();
			uid::Uid model_id = _services->GetUidSystem().GetUid(_model_handle);
			mem::sptr<gfx::VisibleObject> model_visible = mem::create_sptr<gfx::VisibleObject>(_services->GetAllocator());

			_scene->Register(model_id, model_visible, _model);
			_scene->GetOnRenderDelegate().SetCallback(this, SceneOnDrawCallback);

			mem::sptr<gfx::Resource> resource = _scene->GetResource(model_id);
			if (resource && resource->GetType() == gfx::ResourceType::RenderableModel)
			{
				gfx::RenderableModel& renderable_model = (gfx::RenderableModel&)(*resource);
				mem::sptr<gfx::Model> model = renderable_model.GetModel();
				NP_ENGINE_ASSERT(model == _model, "these models should be the same!");

				//TODO: improve meta values
				gfx::RenderableMetaValues& meta_values = renderable_model.GetMetaValues();
				flt scale = _camera.GetProjectionType() == gfx::Camera::ProjectionType::Perspective ? 10.f : 100.f;

				meta_values.object.Model = glm::mat4(1.0f);
				meta_values.object.Model = ::glm::scale(meta_values.object.Model, ::glm::vec3(scale, scale, scale));
				// meta_values.object.Model = ::glm::translate(meta_values.object.Model, ::glm::vec3(-10, -10, -10));
				meta_values.object.Model = ::glm::rotate(meta_values.object.Model, 90.f, _camera.Up);
			}

			geom::FltAabb3D model_aabb = _model->GetAabb();
		}

		static void LogSubmitKeyState(void*, const nput::KeyCodeState&)
		{
			NP_ENGINE_LOG_INFO("key code callback");
		}

		static void LogSubmitMouseState(void*, const nput::MouseCodeState&)
		{
			NP_ENGINE_LOG_INFO("mouse code callback");
		}

		static void LogSubmitMousePosition(void*, const nput::MousePosition&)
		{
			NP_ENGINE_LOG_INFO("mouse position callback");
		}

		static void LogSubmitControllerState(void*, const nput::ControllerCodeState&)
		{
			NP_ENGINE_LOG_INFO("controller code callback");
		}

		static void LogFocus(void*, bl)
		{
			NP_ENGINE_LOG_INFO("focus callback");
		}

		static void LogMaximize(void*, bl)
		{
			NP_ENGINE_LOG_INFO("maximize callback");
		}
		static void LogMinimize(void*, bl)
		{
			NP_ENGINE_LOG_INFO("minimize callback");
		}

		static void LogFramebuffer(void*, ui32, ui32)
		{
			NP_ENGINE_LOG_INFO("framebuffer callback");
		}

		static void LogPosition(void*, i32, i32)
		{
			NP_ENGINE_LOG_INFO("position callback");
		}

		static void LogResize(void*, ui32, ui32)
		{
			NP_ENGINE_LOG_INFO("resize callback");
		}

		void PrepareForRun()
		{
			NP_ENGINE_PROFILE_FUNCTION();

			void* input_queue = mem::AddressOf(_services->GetInputQueue());
			win::Window::Properties window_properties;
			window_properties.title = "My Game Window >:D";

			_window->SetKeyCallback(input_queue, nput::InputListener::SubmitKeyState);
			_window->SetMouseCallback(input_queue, nput::InputListener::SubmitMouseState);
			_window->SetMousePositionCallback(input_queue, nput::InputListener::SubmitMousePosition);
			_window->SetControllerCallback(input_queue, nput::InputListener::SubmitControllerState);

			//*
			_window->SetKeyCallback(this, LogSubmitKeyState);
			_window->SetMouseCallback(this, LogSubmitMouseState);
			_window->SetMousePositionCallback(this, LogSubmitMousePosition);
			_window->SetControllerCallback(this, LogSubmitControllerState);
			_window->SetFocusCallback(this, LogFocus);
			_window->SetFramebufferCallback(this, LogFramebuffer);
			_window->SetMaximizeCallback(this, LogMaximize);
			_window->SetMinimizeCallback(this, LogMinimize);
			_window->SetPositionCallback(this, LogPosition);
			_window->SetResizeCallback(this, LogResize);
			//*/
			
			//TODO: create scene??

			// TODO: init scene, and destroy content for last scene??
			//_scene->GetOnDrawDelegate().SetCallback(this, SceneOnDrawCallback); // TODO: do we need this?? I don't think so
			//_scene->Prepare();
		}

		void Update(tim::DblMilliseconds time_delta) override
		{
			nput::InputQueue& input = _services->GetInputQueue();
			const nput::KeyCodeStates& key_states = input.GetKeyCodeStates();

			//rate = unit / second

			tim::DblSeconds seconds = time_delta;
			dbl s = seconds.count();

			if (key_states[nput::KeyCode::A].IsActive())
				_camera.Position.x -= _rate * s;

			if (key_states[nput::KeyCode::D].IsActive())
				_camera.Position.x += _rate * s;

			if (key_states[nput::KeyCode::W].IsActive())
				_camera.Position.y += _rate * s;

			if (key_states[nput::KeyCode::S].IsActive())
				_camera.Position.y -= _rate * s;

			if (key_states[nput::KeyCode::Q].IsActive())
				_camera.Position.z += _rate * s;

			if (key_states[nput::KeyCode::E].IsActive())
				_camera.Position.z -= _rate * s;

			if (key_states[nput::KeyCode::R].IsActive() && _rate < DBL_MAX)
				_rate += 0.1;

			if (key_states[nput::KeyCode::T].IsActive() && _rate > 0)
				_rate -= 0.1;

			if (key_states[nput::KeyCode::O].IsActive())
				_camera.SetProjectionType(gfx::Camera::ProjectionType::Orthographic);

			if (key_states[nput::KeyCode::P].IsActive())
				_camera.SetProjectionType(gfx::Camera::ProjectionType::Perspective);
		}

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
		GameApp(mem::sptr<srvc::Services> services):
			Application(Application::Properties{"My Game App"}, services),
			_game_layer(services, _window_layer, _graphics_layer)
		{
			NP_ENGINE_PROFILE_FUNCTION();
			PushLayer(mem::AddressOf(_game_layer));
		}

		void Run(i32 argc, chr** argv) override
		{
			NP_ENGINE_PROFILE_FUNCTION();
			NP_ENGINE_LOG_INFO("Hello world from my game app! My title is '" + GetTitle() + "'");
			_game_layer.PrepareForRun();
			Application::Run(argc, argv);
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_TESTER_HPP */