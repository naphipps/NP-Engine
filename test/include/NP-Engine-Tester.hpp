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
		mem::sptr<uid::UidHandle> _window_id_handle;
		mem::sptr<win::Window> _window;
		mutexed_wrapper<mem::sptr<gpu::Scene>> _scene;
		gpu::Camera _camera;
		str _model_filename;
		str _model_texture_filename;
		mem::sptr<gpu::Model> _model;
		mem::sptr<uid::UidHandle> _model_handle;
		tim::SteadyTimestamp _start_timestamp;
		flt _rate = 10.f;

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

		static void LogSize(void*, ui32, ui32)
		{
			NP_ENGINE_LOG_INFO("size callback");
		}

		static void DestroySceneCallback(void* caller, mem::Delegate& d)
		{
			d.DestructData<mem::sptr<gpu::Scene>>();
		}

		static void AdjustForWindowClosingCallback(void* caller, mem::Delegate& d)
		{
			((GameLayer*)caller)->AdjustForWindowClosingProcedure(d);
		}

		void AdjustForWindowClosingProcedure(mem::Delegate& d)
		{
			uid::Uid windowId = d.GetData<uid::Uid>();

			if (_window->GetUid() == windowId)
			{
				_window_id_handle.reset();
				_window.reset();
			}

			auto scene = _scene.get_access();
			if ((*scene)->GetRenderTarget()->GetWindow()->GetUid() == windowId)
			{
				jsys::JobSystem& job_system = _services->GetJobSystem();
				mem::sptr<jsys::Job> destroy_scene_job = job_system.CreateJob();
				destroy_scene_job->GetDelegate().ConstructData<mem::sptr<gpu::Scene>>(::std::move(*scene));
				destroy_scene_job->GetDelegate().SetCallback(DestroySceneCallback);
				job_system.SubmitJob(jsys::JobPriority::Higher, destroy_scene_job);
			}

			d.DestructData<uid::Uid>();
		}

		void AdjustForWindowClosing(mem::sptr<evnt::Event> e)
		{
			win::WindowClosingEvent& closing_event = (win::WindowClosingEvent&)(*e);
			win::WindowClosingEventData& closing_data = closing_event.GetData();

			mem::sptr<jsys::Job> adjust_job = _services->GetJobSystem().CreateJob();
			adjust_job->GetDelegate().ConstructData<uid::Uid>(closing_data.windowId);
			adjust_job->GetDelegate().SetCallback(this, AdjustForWindowClosingCallback);
			jsys::Job::AddDependency(closing_data.job, adjust_job);
			_services->GetJobSystem().SubmitJob(jsys::JobPriority::Higher, adjust_job);
		}

		void HandleEvent(mem::sptr<evnt::Event> e) override
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

		static void SceneOnRenderCallback(void* caller, mem::Delegate& d)
		{
			((GameLayer*)caller)->SceneOnRender(d);
		}

		void SceneOnRender(mem::Delegate& d)
		{
			gpu::Scene* scene = d.GetData<gpu::Scene*>();
			if (scene)
				scene->SetCamera(_camera);
		}

		static void CreateSceneCallback(void* caller, mem::Delegate& d)
		{
			((GameLayer*)caller)->CreateSceneProcedure();
		}

		void CreateSceneProcedure()
		{
			_window->SetTitle("My Game Window >:D");

			void* input_queue = mem::AddressOf(_services->GetInputQueue());
			_window->SetKeyCallback(input_queue, nput::InputListener::SubmitKeyState);
			_window->SetMouseCallback(input_queue, nput::InputListener::SubmitMouseState);
			_window->SetMousePositionCallback(input_queue, nput::InputListener::SubmitMousePosition);
			_window->SetControllerCallback(input_queue, nput::InputListener::SubmitControllerState);
			/*
			_window->SetKeyCallback(this, LogSubmitKeyState);
			_window->SetMouseCallback(this, LogSubmitMouseState);
			_window->SetMousePositionCallback(this, LogSubmitMousePosition);
			_window->SetControllerCallback(this, LogSubmitControllerState);
			_window->SetFocusCallback(this, LogFocus);
			_window->SetFramebufferCallback(this, LogFramebuffer);
			_window->SetMaximizeCallback(this, LogMaximize);
			_window->SetMinimizeCallback(this, LogMinimize);
			_window->SetPositionCallback(this, LogPosition);
			_window->SetSizeCallback(this, LogSize);
			//*/

			mem::sptr<gpu::DetailInstance> detail_instance = gpu::DetailInstance::Create(gpu::DetailType::Vulkan, _services);
			mem::sptr<gpu::RenderTarget> render_target = gpu::RenderTarget::Create(detail_instance, _window);
			mem::sptr<gpu::RenderDevice> render_device = gpu::RenderDevice::Create(render_target);
			mem::sptr<gpu::RenderContext> render_context = gpu::RenderContext::Create(render_device);
			mem::sptr<gpu::RenderPass> render_pass = gpu::RenderPass::Create(render_context);
			mem::sptr<gpu::Framebuffers> framebuffers = gpu::Framebuffers::Create(render_pass);

			gpu::Shader::Properties vertex_shader_properties;
			vertex_shader_properties.type = gpu::Shader::Type::Vertex;
			vertex_shader_properties.entrypoint = "main";
			vertex_shader_properties.filename = fsys::Append(fsys::Append("Vulkan", "shaders"), "object_vertex.glsl");
			mem::sptr<gpu::RenderShader> vertex_shader = gpu::RenderShader::Create(render_device, vertex_shader_properties);

			gpu::Shader::Properties fragment_shader_properties;
			fragment_shader_properties.type = gpu::Shader::Type::Fragment;
			fragment_shader_properties.entrypoint = "main";
			fragment_shader_properties.filename = fsys::Append(fsys::Append("Vulkan", "shaders"), "object_fragment.glsl");
			mem::sptr<gpu::RenderShader> fragment_shader = gpu::RenderShader::Create(render_device, fragment_shader_properties);

			gpu::RenderPipeline::Properties render_pipeline_properties{ framebuffers, vertex_shader, fragment_shader };
			mem::sptr<gpu::RenderPipeline> render_pipeline = gpu::RenderPipeline::Create(render_pipeline_properties);

			gpu::Scene::Properties scene_properties{ render_pipeline, _camera };
			auto scene = _scene.get_access();
			*scene = gpu::Scene::Create(scene_properties);

			_model_handle = _services->GetUidSystem().CreateUidHandle();
			uid::Uid model_id = _services->GetUidSystem().GetUid(_model_handle);
			mem::sptr<gpu::VisibleObject> model_visible = mem::create_sptr<gpu::VisibleObject>(_services->GetAllocator());

			(*scene)->Register(model_id, model_visible, _model);
			(*scene)->GetOnRenderDelegate().SetCallback(this, SceneOnRenderCallback);

			mem::sptr<gpu::Resource> resource = (*scene)->GetResource(model_id);
			if (resource && resource->GetType() == gpu::ResourceType::RenderableModel)
			{
				gpu::RenderableModel& renderable_model = (gpu::RenderableModel&)(*resource);
				mem::sptr<gpu::Model> model = renderable_model.GetModel();
				NP_ENGINE_ASSERT(model == _model, "these models should be the same!");

				//TODO: improve meta values
				gpu::RenderableMetaValues& meta_values = renderable_model.GetMetaValues();
				flt scale = _camera.projectionType == gpu::Camera::ProjectionType::Perspective ? 10.f : 100.f;

				//meta_values.object.Model = glm::mat4(1.0f);
				//meta_values.object.Model = ::glm::scale(meta_values.object.Model, ::glm::vec3(scale, scale, scale));
				// meta_values.object.Model = ::glm::translate(meta_values.object.Model, ::glm::vec3(-10, -10, -10));
				//meta_values.object.Model = ::glm::rotate(meta_values.object.Model, 90.f, _camera.Up);
			}

			geom::FltAabb3D model_aabb = _model->GetAabb(); //TODO: we need to update this
		}

		void SubmitCreateSceneJob()
		{
			NP_ENGINE_ASSERT(_window, "require valid window");
			mem::sptr<jsys::Job> create_scene_job = _services->GetJobSystem().CreateJob();
			create_scene_job->GetDelegate().SetCallback(this, CreateSceneCallback);
			_services->GetJobSystem().SubmitJob(jsys::JobPriority::Higher, create_scene_job);
		}

		nput::KeyCodeStates _prev_keys;
		nput::MouseCodeStates _prev_mouse;
		nput::MousePosition _prev_mouse_position;

		bl _mouse_is_dragging = false;

		void DigestInput(tim::DblMilliseconds time_delta)
		{
			using Key = nput::KeyCode;
			using Mouse = nput::MouseCode;

			nput::InputQueue& input = _services->GetInputQueue();
			const nput::KeyCodeStates& keys = input.GetKeyCodeStates();
			const nput::MouseCodeStates& mouse = input.GetMouseCodeStates();
			const nput::MousePosition& mouse_position = input.GetMousePosition();

			//rate = unit / second

			//TODO: improve camera controls and get the camera/visible thing working

			tim::DblSeconds seconds = time_delta;
			dbl s = seconds.count();

			if (keys[Key::A].IsActive())
				_camera.position.x -= _rate * s;

			if (keys[Key::D].IsActive())
				_camera.position.x += _rate * s;

			if (keys[Key::W].IsActive())
			{
				::glm::vec3 direction = (_camera.lookAt - _camera.eye) / ::glm::distance(_camera.eye, _camera.lookAt);
				direction *= _rate * s;

				::glm::vec4 pos = { _camera.position.x, _camera.position.y, _camera.position.z, 1 };
				pos = ::glm::translate(::glm::mat4{ 1.f }, direction) * pos;
				_camera.position = { pos.x, pos.y, pos.z };
				_camera.NormalizeLookAt();
			}

			if (keys[Key::S].IsActive())
			{
				::glm::vec3 direction = _camera.lookAt - _camera.eye;
				direction /= ::glm::distance(_camera.eye, _camera.lookAt);
				direction *= -_rate * s;

				::glm::vec4 new_position = { _camera.position.x, _camera.position.y, _camera.position.z, 1 };
				::glm::mat4 identity{ 1.f };
				::glm::mat4 trans = ::glm::translate(identity, direction);
				new_position = trans * new_position;
				_camera.position = { new_position.x, new_position.y, new_position.z };
				_camera.NormalizeLookAt();
			}

			if (keys[Key::Q].IsActive())
				_camera.position.z += _rate * s;

			if (keys[Key::E].IsActive())
				_camera.position.z -= _rate * s;

			if (keys[Key::R].IsActive() && _rate < DBL_MAX)
				_rate += 0.1;

			if (keys[Key::T].IsActive() && _rate > 0)
				_rate -= 0.1;

			if (keys[Key::O].IsActive())
				_camera.projectionType = gpu::Camera::ProjectionType::Orthographic;

			if (keys[Key::P].IsActive())
				_camera.projectionType = gpu::Camera::ProjectionType::Perspective;

			if (keys[Key::C].IsActive() && !_prev_keys[Key::C].IsActive())
			{
				_camera._contains = false;
				//(*_scene.get_access())->UnregisterVisible(_services->GetUidSystem().GetUid(_model_handle));
				//_model_handle.reset();
				//(*_scene.get_access())->UnregisterResource(_services->GetUidSystem().GetUid(_model_handle));
			}

			if (keys[Key::V].IsActive() && !_prev_keys[Key::V].IsActive())
			{
				_camera._contains = true;
				//uid::Uid model_id = _services->GetUidSystem().GetUid(_model_handle);
				//(*_scene.get_access())->Register(model_id, mem::create_sptr<gpu::VisibleObject>(_services->GetAllocator()));
			}

			if (keys[Key::Z].IsActive())
			{
				//turn camera to origin
				_camera.lookAt = { 0, 0, 0 };
				_camera.NormalizeLookAt();

				//make the camera lookat a unit vector
			}




			if (mouse[Mouse::LeftButton].IsActive())
			{
				if (!_prev_mouse[Mouse::LeftButton].IsActive())
					_mouse_is_dragging = true;

				if (_mouse_is_dragging)
				{
					if (mouse_position.GetPosition() != _prev_mouse_position.GetPosition())
					{
						/*
							TODO: implement camera rotations when dragging mouse
								- <https://www.youtube.com/watch?v=MZuYmG1GBFk>
								- <https://www.youtube.com/results?search_query=glm+quaterion+camera>
								- <https://en.wikipedia.org/wiki/Aircraft_principal_axes>
						*/

						NP_ENGINE_LOG_INFO("mouse dragging");

						flt scale = 0.002;
						//::glm::vec2 diff = _prev_mouse_position.GetPosition() - mouse_position.GetPosition();
						//diff = ::glm::radians(-diff);
						//diff *= scale;
						//flt sign = diff.x < 0 ? -1 : 1;
						//diff.x /= 20.f;
						//diff.y /= 50.f;

						::glm::vec2 curr = mouse_position.GetPosition();
						curr = ::glm::radians(curr);
						curr *= scale;

						
						::glm::quat rot_q{ curr.x, _camera.up };
						//rot_q = ::glm::normalize(rot_q);

						::glm::vec3 temp{ 1.f, 0.f, 0.f };
						::glm::quat res = rot_q * temp * ::glm::conjugate(rot_q);
						temp = { res.x, res.y, res.z };
						//temp = ::glm::normalize(temp);

						::glm::vec3 right = ::glm::cross(_camera.up, temp);
						rot_q = { curr.y, right };
						//rot_q = ::glm::normalize(rot_q);
						res = rot_q * temp * ::glm::conjugate(rot_q);
						temp = { res.x, res.y, res.z };

						_camera.lookAt = _camera.eye + temp;

						/*
							::glm::quat rot_q{ diff.x, _camera.up };
							rot_q = ::glm::normalize(rot_q);
							::glm::quat con_q = ::glm::conjugate(rot_q);
							::glm::vec3 direction = ::glm::normalize(_camera.GetLookDirection());
							::glm::quat res = rot_q * direction * con_q;
							direction = { res.x, res.y, res.z };
							_camera.lookAt = _camera.eye + direction;
						*/
						/*
							quaternion rotation:
								::glm::quat q, q_;
								::glm::vec3 p, p_;
								p_ = q * p * q_;
						*/

						_camera.NormalizeLookAt();
					}
				}
			}
			else
			{
				_mouse_is_dragging = false;
			}






			for (siz i = 0; i < keys.size(); i++)
				_prev_keys[i] = keys[i];

			for (siz i = 0; i < mouse.size(); i++)
				_prev_mouse[i] = mouse[i];

			_prev_mouse_position = mouse_position;
		}

	public:
		GameLayer(mem::sptr<srvc::Services> services, WindowLayer& window_layer):
			Layer(services),
			_window_layer(window_layer),
			_window_id_handle(_services->GetUidSystem().CreateUidHandle()),
			_window(_window_layer.Create(win::DetailType::Glfw, _services->GetUidSystem().GetUid(_window_id_handle))),
			_scene(nullptr),
			_model_filename(
				fsys::Append(fsys::Append(fsys::Append(NP_ENGINE_WORKING_DIR, "test"), "assets"), "viking_room.obj")),
			_model_texture_filename(
				fsys::Append(fsys::Append(fsys::Append(NP_ENGINE_WORKING_DIR, "test"), "assets"), "viking_room.png")),
			_model(mem::create_sptr<gpu::Model>(_services->GetAllocator(), _model_filename, _model_texture_filename, true)),
			_model_handle(nullptr),
			_start_timestamp(tim::SteadyClock::now())
		{
			//_model->GetTexture().SetHotReloadable();
			//_renderable_model->GetUpdateMetaValuesOnFrameDelegate().SetCallback(this, UpdateMetaValuesOnFrameCallback);

			_camera.eye = { 10.f, 10.f, 10.f };
			_camera.fovy = 70.f;
			_camera.nearPlane = 0.01f;
			_camera.farPlane = 100.0f;
			_camera.lookAt = { 0, 0, 0 };

			//-----------------------------------------------------------

			if (_window)
				SubmitCreateSceneJob();
		}

		

		void Update(tim::DblMilliseconds time_delta) override
		{
			DigestInput(time_delta);

			//just in case window is not created in constructor
			if (!_window && _window_id_handle && _services->GetUidSystem().Has(_services->GetUidSystem().GetUid(_window_id_handle)))
			{
				_window = _window_layer.Get(_services->GetUidSystem().GetUid(_window_id_handle));
				if (_window)
					SubmitCreateSceneJob();
			}

			{
				auto scene = _scene.get_access();
				if (scene && *scene)
					(*scene)->Render();
			}

			
		}

		void Cleanup() override
		{
			{
				auto scene = _scene.get_access();
				if (scene && *scene)
					(*scene)->CleanupVisibles();
			}
		}

		evnt::EventCategory GetHandledCategories() const override
		{
			return evnt::EventCategory::All;
		}
	};

	class GameApp : public Application
	{
	private:
		GameLayer _game_layer;

		void CustomizeJobSystem()
		{
			jsys::JobSystem& job_system = _services->GetJobSystem();
			con::vector<jsys::JobWorker>& job_workers = job_system.GetJobWorkers();
			using Fetch = jsys::JobWorker::Fetch;

			NP_ENGINE_ASSERT(thr::Thread::HardwareConcurrency() >= 4, "NP Engine Test requires at least four cores");

			/*
				Examples:
					FetchOrderArray = jsys::JobWorker::FetchOrderArray;
					FetchOrderArray default_order{Fetch::Immediate, Fetch::PriorityBased, Fetch::Steal};
					FetchOrderArray thief_order{Fetch::Steal, Fetch::Immediate, Fetch::None};
					FetchOrderArray priority_order{Fetch::PriorityBased, Fetch::None, Fetch::None};
					FetchOrderArray immediate_order{Fetch::Immediate, Fetch::Steal, Fetch::None};
					FetchOrderArray only_immediate_order{ Fetch::Immediate, Fetch::Immediate, Fetch::Immediate };

					//disable deep sleep on workers - better performance from workers, but 100% CPU
					for (auto it = job_workers.begin(); it != job_workers.end(); it++)
						it->DisableDeepSleep();
			*/
		}

	public:
		GameApp(mem::sptr<srvc::Services> services):
			Application(Application::Properties{"My Game App"}, services),
			_game_layer(services, _window_layer)
		{
			PushLayer(mem::AddressOf(_game_layer));
		}

		void Run(i32 argc, chr** argv) override
		{
			NP_ENGINE_PROFILE_FUNCTION();
			NP_ENGINE_LOG_INFO("Hello world from my game app! My title is '" + GetTitle() + "'");
			CustomizeJobSystem();
			Application::Run(argc, argv);
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_TESTER_HPP */