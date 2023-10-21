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
		flt _rate = 5.f; // units per second

		mem::sptr<net::Context> _network_context;
		mem::sptr<net::Socket> _tcp_server;
		mutexed_wrapper<con::vector<mem::sptr<net::Socket>>> _tcp_server_clients;
		mutexed_wrapper<con::vector<mem::sptr<net::Socket>>> _tcp_clients;

		mem::sptr<net::Socket> _http_socket;

		mem::sptr<net::Socket> _udp_server;
		mem::sptr<net::Socket> _udp_client;

		tim::SteadyTimestamp _clients_send_msg_timestamp;

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

		void AdjustForApplicationClose(mem::sptr<evnt::Event> e)
		{
			TcpServerCloseClients();
			_udp_server->Close();
			_http_socket->Close();
		}

		void HandleNetworkClient(mem::sptr<evnt::Event> e)
		{
			net::NetworkClientEvent& client_event = (net::NetworkClientEvent&)*e;
			net::NetworkClientEventData& client_data = client_event.GetData();

			str name = "UNKNOWN";
			if (client_data.host)
			{
				if (client_data.host->name != "")
					name = client_data.host->name;
				else if (!client_data.host->ipv4s.empty())
					name = to_str(client_data.host->ipv4s.begin()->first);
				else if (!client_data.host->ipv6s.empty())
					name = to_str(client_data.host->ipv6s.begin()->first);
			}

			if (client_data.socket && *client_data.socket)
			{
				NP_ENGINE_LOG_INFO("server accepted: " + name);

				_tcp_server_clients.get_access()->emplace_back(client_data.socket);
				client_data.socket->StartReceiving();
			}
			else
			{
				NP_ENGINE_LOG_INFO("server denied: " + name);
			}

			e->SetHandled();
		}

		void HandleEvent(mem::sptr<evnt::Event> e) override
		{
			switch (e->GetType())
			{
			case evnt::EventType::ApplicationClose:
				AdjustForApplicationClose(e);
				break;

			case evnt::EventType::WindowClosing:
				AdjustForWindowClosing(e);
				break;

			case evnt::EventType::NetworkClient:
				HandleNetworkClient(e);
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

			gpu::RenderPipeline::Properties render_pipeline_properties{framebuffers, vertex_shader, fragment_shader};
			mem::sptr<gpu::RenderPipeline> render_pipeline = gpu::RenderPipeline::Create(render_pipeline_properties);

			gpu::Scene::Properties scene_properties{render_pipeline, _camera};
			auto scene = _scene.get_access();
			*scene = gpu::Scene::Create(scene_properties);

			_model_handle = _services->GetUidSystem().CreateUid();
			uid::Uid model_id = _services->GetUidSystem().GetUid(_model_handle);
			mem::sptr<gpu::VisibleObject> model_visible = mem::create_sptr<gpu::VisibleObject>(_services->GetAllocator());

			(*scene)->Register(model_id, model_visible, _model);
			(*scene)->GetOnRenderDelegate().SetCallback(this, SceneOnRenderCallback);

			geom::FltObb3D model_obb = _model->GetObb(); // TODO: we need to update this
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

			// TODO: improve camera controls and get the camera/visible thing working

			const dbl seconds = time_delta.count() / 1000.f;

			if (keys[Key::A].IsActive())
			{
				const ::glm::vec3 look = _camera.GetLookDirection();
				const ::glm::vec3 direction = ::glm::cross(gpu::Camera::Up, _camera.GetLookDirection());
				::glm::vec3 rate = ::glm::normalize(direction);
				rate *= _rate * seconds;

				::glm::vec4 pos = {_camera.eye.x, _camera.eye.y, _camera.eye.z, 1};
				pos = ::glm::translate(::glm::mat4{1.f}, rate) * pos;
				_camera.eye = {pos.x, pos.y, pos.z};
				_camera.lookAt = _camera.eye + look;
				_camera.NormalizeLookAt();
			}

			if (keys[Key::D].IsActive())
			{
				const ::glm::vec3 look = _camera.GetLookDirection();
				const ::glm::vec3 direction = ::glm::cross(gpu::Camera::Up, _camera.GetLookDirection());
				::glm::vec3 rate = ::glm::normalize(direction);
				rate *= -_rate * seconds;

				::glm::vec4 pos = {_camera.eye.x, _camera.eye.y, _camera.eye.z, 1};
				pos = ::glm::translate(::glm::mat4{1.f}, rate) * pos;
				_camera.eye = {pos.x, pos.y, pos.z};
				_camera.lookAt = _camera.eye + look;
				_camera.NormalizeLookAt();
			}

			if (keys[Key::W].IsActive())
			{
				const ::glm::vec3 look = _camera.GetLookDirection();
				::glm::vec3 rate = ::glm::normalize(look);
				rate *= _rate * seconds;

				::glm::vec4 pos = {_camera.eye.x, _camera.eye.y, _camera.eye.z, 1};
				pos = ::glm::translate(::glm::mat4{1.f}, rate) * pos;
				_camera.eye = {pos.x, pos.y, pos.z};
				_camera.lookAt = _camera.eye + look;
				_camera.NormalizeLookAt();
			}

			if (keys[Key::S].IsActive())
			{
				const ::glm::vec3 look = _camera.GetLookDirection();
				::glm::vec3 rate = ::glm::normalize(look);
				rate *= -_rate * seconds;

				::glm::vec4 pos = {_camera.eye.x, _camera.eye.y, _camera.eye.z, 1};
				pos = ::glm::translate(::glm::mat4{1.f}, rate) * pos;
				_camera.eye = {pos.x, pos.y, pos.z};
				_camera.lookAt = _camera.eye + look;
				_camera.NormalizeLookAt();
			}

			if (keys[Key::Q].IsActive())
			{
				const ::glm::vec3 look = _camera.GetLookDirection();
				const ::glm::vec3 direction = gpu::Camera::Up;
				::glm::vec3 rate = ::glm::normalize(direction);
				rate *= -_rate * seconds;

				::glm::vec4 pos = {_camera.eye.x, _camera.eye.y, _camera.eye.z, 1};
				pos = ::glm::translate(::glm::mat4{1.f}, rate) * pos;
				_camera.eye = {pos.x, pos.y, pos.z};
				_camera.lookAt = _camera.eye + look;
				_camera.NormalizeLookAt();
			}

			if (keys[Key::E].IsActive())
			{
				const ::glm::vec3 look = _camera.GetLookDirection();
				const ::glm::vec3 direction = gpu::Camera::Up;
				::glm::vec3 rate = ::glm::normalize(direction);
				rate *= _rate * seconds;

				::glm::vec4 pos = {_camera.eye.x, _camera.eye.y, _camera.eye.z, 1};
				pos = ::glm::translate(::glm::mat4{1.f}, rate) * pos;
				_camera.eye = {pos.x, pos.y, pos.z};
				_camera.lookAt = _camera.eye + look;
				_camera.NormalizeLookAt();
			}

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
				// uid::Uid model_id = _services->GetUidSystem().GetUid(_model_handle);
				//(*_scene.get_access())->Register(model_id, mem::create_sptr<gpu::VisibleObject>(_services->GetAllocator()));
			}

			if (keys[Key::Z].IsActive())
			{
				// turn camera to origin
				_camera.lookAt = {0, 0, 0};
				_camera.NormalizeLookAt();

				// make the camera lookat a unit vector
			}

			if (keys[Key::H].IsActive())
			{
				// scale up
				geom::Transform& transform = _model->GetTransform();
				transform.scale.z = ::std::clamp(transform.scale.z + _rate, 1.f, 2.f);
			}

			if (keys[Key::B].IsActive())
			{
				// scale down
				geom::Transform& transform = _model->GetTransform();
				transform.scale.z = ::std::clamp(transform.scale.z - _rate, 1.f, 2.f);
			}

			if (mouse[Mouse::LeftButton].IsActive())
			{
				if (!_prev_mouse[Mouse::LeftButton].IsActive())
					_mouse_is_dragging = true;

				if (_mouse_is_dragging)
				{
					if (mouse_position.GetPosition() != _prev_mouse_position.GetPosition())
					{
						const flt scale = 0.04;
						::glm::vec2 diff = _prev_mouse_position.GetPosition() - mouse_position.GetPosition();
						diff *= _rate * scale;
						diff = ::glm::radians(diff);

						::glm::vec3 look = _camera.GetLookDirection();
						::glm::quat v{0.f, look};
						::glm::quat q = ::glm::angleAxis(diff.x, gpu::Camera::Up);
						v = q * v * ::glm::conjugate(q);
						look = {v.x, v.y, v.z}; // rotated around Up

						::glm::vec3 right = ::glm::cross(look, gpu::Camera::Up);
						q = ::glm::angleAxis(diff.y, right);
						v = q * v * ::glm::conjugate(q);
						look = {v.x, v.y, v.z}; // rotated around right

						_camera.lookAt = _camera.eye + look;
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

		static void TcpServerAcceptClientCallback(void* caller, mem::Delegate& d)
		{
			((GameLayer*)caller)->_tcp_server->Accept(true);
		}

		void SubmitTcpServerAcceptClientJob()
		{
			mem::sptr<jsys::Job> job = _services->GetJobSystem().CreateJob();
			job->GetDelegate().SetCallback(this, TcpServerAcceptClientCallback);
			_services->GetJobSystem().SubmitJob(jsys::JobPriority::Higher, job);
		}

		void TcpServerCloseClients()
		{
			auto clients = _tcp_server_clients.get_access();
			for (auto c = clients->begin(); c != clients->end(); c++)
				(*c)->Close();
		}

		static void ClientConnectToTcpServerCallback(void* caller, mem::Delegate& d)
		{
			((GameLayer*)caller)->ClientConnectToTcpServerProcedure();
		}

		void ClientConnectToTcpServerProcedure()
		{
			mem::sptr<net::Socket> client = net::Socket::Create(_network_context);
			if (client)
			{
				client->Open(net::Protocol::Tcp);
				client->Enable({net::SocketOptions::ReuseAddress, net::SocketOptions::ReusePort});
				client->ConnectTo(net::Ipv4{127, 0, 0, 1}, 55555);
			}

			if (client)
				_tcp_clients.get_access()->emplace_back(client);
		}

		void SubmitClientConnectToTcpServerJob()
		{
			mem::sptr<jsys::Job> job = _services->GetJobSystem().CreateJob();
			job->GetDelegate().SetCallback(this, ClientConnectToTcpServerCallback);
			_services->GetJobSystem().SubmitJob(jsys::JobPriority::Higher, job);
		}

	public:
		GameLayer(mem::sptr<srvc::Services> services, WindowLayer& window_layer):
			Layer(services),
			_window_layer(window_layer),
			_window_id_handle(_services->GetUidSystem().CreateUid()),
			_window(_window_layer.Create(win::DetailType::Glfw, _services->GetUidSystem().GetUid(_window_id_handle))),
			_scene(nullptr),
			_model_filename(
				fsys::Append(fsys::Append(fsys::Append(NP_ENGINE_WORKING_DIR, "test"), "assets"), "viking_room.obj")),
			_model_texture_filename(
				fsys::Append(fsys::Append(fsys::Append(NP_ENGINE_WORKING_DIR, "test"), "assets"), "viking_room.png")),
			_model(mem::create_sptr<gpu::Model>(_services->GetAllocator(), _model_filename, _model_texture_filename, true)),
			_model_handle(nullptr),
			_start_timestamp(tim::SteadyClock::now()),
			_network_context(net::Context::Create(net::DetailType::Native, _services)),
			_clients_send_msg_timestamp(tim::SteadyClock::now())
		{
			net::Init(net::DetailType::Native);

			geom::Transform& transform = _model->GetTransform();

			transform.position = {0.f, 0.f, 0.f};
			transform.orientation = {0.f, 0.f, 0.f, 1.f};
			transform.scale = {1.f, 1.f, 1.f};

			::glm::quat rot(::glm::vec3{-M_PI_2, M_PI_2, 0.f});
			transform.orientation *= rot;

			//_model->GetTexture().SetHotReloadable();
			//_renderable_model->GetUpdateMetaValuesOnFrameDelegate().SetCallback(this, UpdateMetaValuesOnFrameCallback);

			_camera.eye = {3.f, 3.f, 3.f};
			_camera.fovy = 70.f;
			_camera.nearPlane = 0.01f;
			_camera.farPlane = 100.0f;
			_camera.lookAt = {0, 0, 0};
			_camera.NormalizeLookAt();

			//-----------------------------------------------------------

			if (_window)
				SubmitCreateSceneJob();

			//-----------------------------------------------------------

			_tcp_server = net::Socket::Create(_network_context);
			_tcp_server->Open(net::Protocol::Tcp);
			_tcp_server->Enable({net::SocketOptions::ReuseAddress, net::SocketOptions::ReusePort});
			_tcp_server->BindTo(net::Ipv4{127, 0, 0, 1}, 55555);
			_tcp_server->Listen();
			SubmitTcpServerAcceptClientJob();

			//-----------------------------------------------------------

			_udp_server = net::Socket::Create(_network_context);
			_udp_server->Open(net::Protocol::Udp);
			_udp_server->Enable({net::SocketOptions::ReuseAddress, net::SocketOptions::ReusePort});
			_udp_server->BindTo(net::Ipv4{127, 0, 0, 1}, 54555);
			_udp_server->StartReceiving();

			_udp_client = net::Socket::Create(_network_context);
			_udp_client->Open(net::Protocol::Udp);
			_udp_client->Enable({net::SocketOptions::ReuseAddress, net::SocketOptions::ReusePort});

			//-----------------------------------------------------------

			mem::sptr<net::Resolver> resv = net::Resolver::Create(_network_context);

			net::Host host = resv->GetHost("example.com");

			_http_socket = net::Socket::Create(_network_context);
			_http_socket->Open(net::Protocol::Tcp);
			_http_socket->Enable({net::SocketOptions::ReuseAddress, net::SocketOptions::ReusePort, net::SocketOptions::Direct});
			_http_socket->ConnectTo(host.ipv4s.begin()->first, 80);
			str http_get = "GET /index.html HTTP/1.1\r\n"
						   "Host: example.com\r\n"
						   "Connection: close\r\n\r\n";
			_http_socket->Send(http_get.data(), http_get.size());
			_http_socket->StartReceiving();

			//-----------------------------------------------------------

			SubmitClientConnectToTcpServerJob();
		}

		~GameLayer()
		{
			TcpServerCloseClients();
			_udp_server->Close();
			_http_socket->Close();

			net::Terminate(net::DetailType::Native);
		}

		void Update(tim::DblMilliseconds time_delta) override
		{
			DigestInput(time_delta);

			// just in case window is not created in constructor
			if (!_window && _window_id_handle &&
				_services->GetUidSystem().Has(_services->GetUidSystem().GetUid(_window_id_handle)))
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
			{
				auto clients = _tcp_server_clients.get_access();
				for (auto c = clients->begin(); c != clients->end(); c++)
				{
					net::MessageQueue& inbox = (*c)->GetInbox();
					inbox.ToggleState();
					for (net::Message msg = inbox.Pop(); msg; msg = inbox.Pop())
					{
						switch (msg.header.type)
						{
						case net::MessageType::Text:
						{
							net::TextMessageBody& text = (net::TextMessageBody&)*msg.body;
							::std::stringstream ss;
							ss << thr::ThisThread::get_id();
							NP_ENGINE_LOG_INFO("Server received(" + str(ss.str()) + "):\n" + text.content);
							break;
						}
						default:
							break;
						}
					}
				}
			}
			{
				::std::stringstream ss;
				ss << thr::ThisThread::get_id();

				net::MessageQueue& inbox = _udp_server->GetInbox();
				inbox.ToggleState();
				for (net::Message msg = inbox.Pop(); msg; msg = inbox.Pop())
				{
					switch (msg.header.type)
					{
					case net::MessageType::Text:
					{
						net::TextMessageBody& text = (net::TextMessageBody&)*msg.body;
						NP_ENGINE_LOG_INFO("Server received(" + str(ss.str()) + ") text:\n" + text.content);
						break;
					}
					case net::MessageType::Blob:
					{
						net::BlobMessageBody& blob = (net::BlobMessageBody&)*msg.body;
						str blob_str((chr*)blob.GetData(), msg.header.bodySize);
						NP_ENGINE_LOG_INFO("Server received(" + str(ss.str()) + ") blob:\n" + blob_str);
						break;
					}
					default:
						break;
					}
				}
			}
			{
				net::MessageQueue& inbox = _http_socket->GetInbox();
				inbox.ToggleState();
				for (net::Message msg = inbox.Pop(); msg; msg = inbox.Pop())
				{
					switch (msg.header.type)
					{
					case net::MessageType::Blob:
					{
						net::BlobMessageBody& blob = (net::BlobMessageBody&)*msg.body;
						str blob_str((chr*)blob.GetData(), msg.header.bodySize);
						NP_ENGINE_LOG_INFO("_http_socket received:\n\n" + blob_str + "\n\n");
						break;
					}
					default:
						break;
					}
				}
			}
			{
				tim::SteadyTimestamp now = tim::SteadyClock::now();
				if (now - _clients_send_msg_timestamp > tim::DblMilliseconds(1000))
				{
					_clients_send_msg_timestamp = now;
					{
						auto clients = _tcp_clients.get_access();
						for (auto c = clients->begin(); c != clients->end(); c++)
						{
							net::Message msg;
							msg.header.type = net::MessageType::Text;
							msg.body = mem::create_sptr<net::TextMessageBody>(_services->GetAllocator());
							net::TextMessageBody& msg_body = (net::TextMessageBody&)*msg.body;
							msg_body.content = "Hello TCP Server!\n\t- tcp client: " + to_str((siz)mem::AddressOf(**c)) + " <3";
							msg.header.bodySize = msg_body.content.size();
							(*c)->Send(msg);
						}
					}
					{
						net::Message msg;
						msg.header.type = net::MessageType::Text;
						msg.body = mem::create_sptr<net::TextMessageBody>(_services->GetAllocator());
						net::TextMessageBody& msg_body = (net::TextMessageBody&)*msg.body;
						msg_body.content =
							"Hello UDP Server!\n\t- udp client: " + to_str((siz)mem::AddressOf(*_udp_client)) + " <3";
						msg.header.bodySize = msg_body.content.size();
						_udp_client->SendTo(msg, net::Ipv4{127, 0, 0, 1}, 54555);
					}
					{
						str blob = "hello from blob! >:D";
						_udp_client->SendTo(blob.data(), blob.size(), net::Ipv4{127, 0, 0, 1}, 54555);
					}
				}
			}
		}

		void Cleanup() override
		{
			{
				auto scene = _scene.get_access();
				if (scene && *scene)
					(*scene)->CleanupVisibles();
			}
			{
				// TODO: cleanup our _server_clients and _clients
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