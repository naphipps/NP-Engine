//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/1/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_TESTER_HPP
#define NP_ENGINE_TESTER_HPP

//#define NP_ENGINE_MEM_ACCUMULATING_ALLOCATOR_BLOCK_SIZE (GIGABYTE_SIZE * 2) //TODO: test

#include <iostream>

#include <NP-Engine/NP-Engine.hpp>

namespace np::app
{
	class GameLayer : public Layer
	{
	private:
		struct Scene
		{
			gpu::DetailType detailType = gpu::DetailType::None;
			mem::sptr<gpu::DetailInstance> detailInstance = nullptr;
			mem::sptr<gpu::PresentTarget> presentTarget = nullptr;
			mem::sptr<gpu::Device> device = nullptr;
			mem::sptr<gpu::Queue> queue = nullptr;
			mem::sptr<gpu::FrameContext> frameContext = nullptr;
			mem::sptr<gpu::Shader> vertexShader = nullptr;
			mem::sptr<gpu::Shader> fragmentShader = nullptr;
			con::vector<mem::sptr<gpu::Framebuffer>> framebuffers{};
			mem::sptr<gpu::RenderPass> renderpass = nullptr;
			mem::sptr<gpu::GraphicsPipeline> graphicsPipeline = nullptr;
			mem::sptr<gpu::CommandBufferPool> commandBufferPool = nullptr;
			con::vector<mem::sptr<gpu::CommandBuffer>> commandBuffers{};

			bl once = false;

			void Render()
			{
				if (once)
				{
					//return;
				}
				once = true;

				mem::sptr<srvc::Services> services = detailInstance->GetServices();

				mem::sptr<gpu::Frame> frame = frameContext->TryAcquireFrame() ? frameContext->GetAcquiredFrame() : nullptr;
				frame->GetReadyFence()->Wait();

				mem::sptr<gpu::BeginRenderPassCommand> begin_renderpass_cmd =
					gpu::Command::Create(gpu::DetailType::Vulkan, services, gpu::CommandType::BeginRenderPass);
				begin_renderpass_cmd->framebuffer = framebuffers[frameContext->GetAcquiredFrameIndex()];
				begin_renderpass_cmd->renderArea = {
					{/*no offset*/}, frameContext->GetFrameWidth(), frameContext->GetFrameHeight()};
				begin_renderpass_cmd->clearColors = {gpu::ClearColor{}};
				//begin_renderpass_cmd->clearColors.front().color.r = UI8_MAX;

				mem::sptr<gpu::BindPipelineCommand> bind_pipeline_cmd =
					gpu::Command::Create(gpu::DetailType::Vulkan, services, gpu::CommandType::BindPipeline);
				bind_pipeline_cmd->pipeline = graphicsPipeline;
				bind_pipeline_cmd->usage = gpu::PipelineUsage::Graphics;

				mem::sptr<gpu::DrawCommand> draw_cmd =
					gpu::Command::Create(gpu::DetailType::Vulkan, services, gpu::CommandType::Draw);
				draw_cmd->vertexCount = 3;

				mem::sptr<gpu::EndRenderPassCommand> end_renderpass_cmd =
					gpu::Command::Create(gpu::DetailType::Vulkan, services, gpu::CommandType::EndRenderPass);
				end_renderpass_cmd->framebuffer = begin_renderpass_cmd->framebuffer;

				if (!commandBufferPool)
					commandBufferPool = queue->CreateCommandBufferPool(gpu::CommandBufferPoolUsage::Resettable);

				if (commandBuffers.empty())
				{
					commandBuffers.resize(frameContext->GetFrames().size());
					for (siz i = 0; i < commandBuffers.size(); i++)
						commandBuffers[i] = commandBufferPool->CreateCommandBuffer();
				}

				mem::sptr<gpu::CommandBuffer> command_buffer = commandBuffers[frameContext->GetAcquiredFrameIndex()];

				commandBufferPool->Reset(command_buffer, gpu::CommandBufferUsage::None);
				commandBufferPool->Begin(command_buffer, gpu::CommandBufferUsage::None);
				command_buffer->Add(begin_renderpass_cmd);
				command_buffer->Add(bind_pipeline_cmd);
				command_buffer->Add(draw_cmd);
				command_buffer->Add(end_renderpass_cmd);
				commandBufferPool->End(command_buffer, gpu::CommandBufferUsage::None);

				gpu::Submit submit{};
				submit.stages = {gpu::Stage::PresentComplete};
				submit.commandBuffers = {command_buffer};
				submit.waitSemaphores = {frame->GetReadySemaphore()};
				submit.signalSemaphores = {frame->GetCompletedSemaphore()};

				mem::sptr<gpu::Fence> submit_fence = device->CreateFence();
				bl submit_success = queue->Submit(submit, submit_fence);
				submit_fence->Wait();

				gpu::Present present{};
				present.frameContexts = {frameContext};
				present.waitSemaphores = {frame->GetCompletedSemaphore()};
				con::vector<bl> present_successes = queue->Present(present);
			}
		};

		WindowLayer& _window_layer;
		mem::sptr<uid::UidHandle> _window_id_handle;
		mem::sptr<win::Window> _window;
		mutexed_wrapper<mem::sptr<Scene>> _scene;
		gpu::Camera _camera;
		str _model_filename;
		str _model_texture_filename;
		//mem::sptr<gpu::Model> _model;
		mem::sptr<uid::UidHandle> _model_handle;
		tim::steady_timestamp _start_timestamp;
		flt _rate = 5.f; // units per second

		mem::sptr<net::Context> _network_context;
		mem::sptr<net::Socket> _tcp_server;
		mutexed_wrapper<con::vector<mem::sptr<net::Socket>>> _tcp_server_clients;
		mutexed_wrapper<con::vector<mem::sptr<net::Socket>>> _tcp_clients;

		mem::sptr<net::Socket> _http_socket;

		mem::sptr<net::Socket> _udp_server;
		mem::sptr<net::Socket> _udp_client;

		tim::steady_timestamp _clients_send_msg_timestamp;

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

		struct AdjustForWindowClosingPayload
		{
			GameLayer* caller = nullptr;
			uid::Uid windowId{};
		};

		static void AdjustForWindowClosingCallback(mem::delegate& d)
		{
			AdjustForWindowClosingPayload* payload = (AdjustForWindowClosingPayload*)d.GetPayload();

			if (payload->caller->_window->GetUid() == payload->windowId)
			{
				payload->caller->_window_id_handle.reset();
				payload->caller->_window.reset();
			}

			//{
			//	auto scene = payload->caller->_scene.get_access();
			//	if ((*scene)->GetRenderTarget()->GetWindow()->GetUid() == payload->windowId)
			//		scene->reset(); // TODO: can we job-ify this?
			//}

			mem::destroy<AdjustForWindowClosingPayload>(payload->caller->_services->GetAllocator(), payload);
		}

		void AdjustForWindowClosing(mem::sptr<evnt::Event> e)
		{
			/*win::WindowClosingEvent& closing_event = (win::WindowClosingEvent&)(*e);
			win::WindowClosingEventData& closing_data = closing_event.GetData();

			mem::sptr<jsys::Job> adjust_job = _services->GetJobSystem().CreateJob();
			adjust_job->SetPayload(
				mem::create<AdjustForWindowClosingPayload>(_services->GetAllocator(), this, closing_data.windowId));
			adjust_job->SetCallback(AdjustForWindowClosingCallback);
			jsys::Job::AddDependency(closing_data.job, adjust_job);
			_services->GetJobSystem().SubmitJob(jsys::JobPriority::Higher, adjust_job);*/
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

		/*static void SceneOnRenderCallback(mem::delegate& d)
		{
			gpu::Scene::OnRenderPayload* payload = (gpu::Scene::OnRenderPayload*)d.GetPayload();
			GameLayer& self = *((GameLayer*)payload->caller);

			if (payload->scene)
				payload->scene->SetCamera(self._camera);


			self._model_handle = self._services->GetUidSystem().CreateUid();
			uid::Uid model_id = self._services->GetUidSystem().GetUid(self._model_handle);

			mem::sptr<gpu::Resource> resource = payload->scene->GetResource(model_id);
		}*/

		static void CreateSceneCallback(mem::delegate& d)
		{
			GameLayer& self = *((GameLayer*)d.GetPayload());

			self._window->SetTitle("My Game Window >:D");

			void* input_queue = mem::address_of(self._services->GetInputQueue());
			self._window->SetKeyCallback(input_queue, nput::InputListener::SubmitKeyState);
			self._window->SetMouseCallback(input_queue, nput::InputListener::SubmitMouseState);
			self._window->SetMousePositionCallback(input_queue, nput::InputListener::SubmitMousePosition);
			self._window->SetControllerCallback(input_queue, nput::InputListener::SubmitControllerState);
			/*
			self._window->SetKeyCallback(mem::address_of(self), LogSubmitKeyState);
			self._window->SetMouseCallback(mem::address_of(self), LogSubmitMouseState);
			self._window->SetMousePositionCallback(mem::address_of(self), LogSubmitMousePosition);
			self._window->SetControllerCallback(mem::address_of(self), LogSubmitControllerState);
			self._window->SetFocusCallback(mem::address_of(self), LogFocus);
			self._window->SetFramebufferCallback(mem::address_of(self), LogFramebuffer);
			self._window->SetMaximizeCallback(mem::address_of(self), LogMaximize);
			self._window->SetMinimizeCallback(mem::address_of(self), LogMinimize);
			self._window->SetPositionCallback(mem::address_of(self), LogPosition);
			self._window->SetSizeCallback(mem::address_of(self), LogSize);
			//*/

			mem::sptr<srvc::Services> services = self._services;
			mem::sptr<Scene> scene = mem::create_sptr<Scene>(services->GetAllocator());

			scene->detailInstance = gpu::DetailInstance::Create(gpu::DetailType::Vulkan, self._services);
			scene->presentTarget = gpu::PresentTarget::Create(scene->detailInstance, self._window);
			scene->device = gpu::Device::Create(scene->detailInstance, gpu::DeviceUsage::Graphics | gpu::DeviceUsage::Present,
												scene->presentTarget);

			for (const gpu::DeviceQueueFamily& family : scene->device->GetDeviceQueueFamilies())
				if (family.usage.Contains(gpu::DeviceQueueUsage::Present | gpu::DeviceQueueUsage::Graphics))
				{
					scene->queue = gpu::Queue::Create(scene->device, family, 0);
					break;
				}

			scene->frameContext = gpu::FrameContext::Create(scene->device, {scene->queue->GetDeviceQueueFamily()});

			scene->vertexShader = gpu::Shader::Create(scene->device, gpu::Stage::Vertex,
													  fsys::append("Vulkan", "shaders", "vertex.glsl"), "main");
			scene->fragmentShader = gpu::Shader::Create(scene->device, gpu::Stage::Fragment,
														fsys::append("Vulkan", "shaders", "fragment.glsl"), "main");

			//this is where we would consolodate our descriptions

			con::vector<gpu::ImageResourceDescription> image_descriptions{
				{scene->frameContext->GetFrameFormat(), 1, gpu::ResourceOperation::Load | gpu::ResourceOperation::Clear,
				 gpu::ResourceOperation::Store, gpu::ResourceOperation::Load | gpu::ResourceOperation::DontCare,
				 gpu::ResourceOperation::Store | gpu::ResourceOperation::DontCare, gpu::ImageResourceUsage::None,
				 gpu::ImageResourceUsage::Present}};

			con::vector<gpu::SubpassDescription> render_subpasses{{{}, {{0, gpu::ImageResourceUsage::Color}}, {}, {}}};

			con::vector<gpu::SubpassDependency> render_dependencies{{SIZ_MAX, 0, gpu::Stage::PresentComplete,
																	 gpu::Stage::PresentComplete, gpu::Access::None,
																	 gpu::Access::Image | gpu::Access::Write}};

			scene->renderpass =
				gpu::RenderPass::Create(scene->device, image_descriptions, render_subpasses, render_dependencies);
			gpu::PipelineUsage graphics_pipeline_usage = gpu::PipelineUsage::Graphics | gpu::PipelineUsage::ColorBlend;

			mem::sptr<gpu::PipelineResourceLayout> graphics_pipeline_layout =
				gpu::PipelineResourceLayout::Create(scene->device, {}, {});

			con::vector<mem::sptr<gpu::Shader>> graphics_shaders{scene->vertexShader, scene->fragmentShader};
			con::vector<gpu::Format> input_vertex_formatting{};
			con::vector<gpu::Format> input_instance_formatting{};
			gpu::PrimitiveTopology graphics_topology = gpu::PrimitiveTopology::Triangle | gpu::PrimitiveTopology::List;
			con::vector<gpu::Viewport> graphics_viewports{
				{{}, (dbl)scene->frameContext->GetFrameWidth(), (dbl)scene->frameContext->GetFrameHeight(), {0, 1}}};
			con::vector<gpu::Scissor> graphics_scissors{
				{{}, scene->frameContext->GetFrameWidth(), scene->frameContext->GetFrameHeight()}};
			gpu::Rasterization graphics_rasterization{gpu::RasterizationUsage::PolygonFill | gpu::RasterizationUsage::CullBack |
														  gpu::RasterizationUsage::FrontFaceClockwise,
													  {}};
			gpu::Multisample graphics_multisample{{gpu::MultisampleUsage::None, 1}, 0.0, {}};
			//gpu::DepthStencil graphics_depth_stencil{};
			gpu::Blend graphics_blend{
				gpu::LogicOperation::None,
				{{false,
				  {gpu::BlendScalar::Src | gpu::BlendScalar::Alpha,
				   gpu::BlendScalar::Src | gpu::BlendScalar::Alpha | gpu::BlendScalar::OneMinus, gpu::BlendOperation::Add},
				  {gpu::BlendScalar::Oned, gpu::BlendScalar::Zeroed, gpu::BlendOperation::Add},
				  gpu::ColorChannel::All}},
				{}};
			gpu::DynamicUsage graphics_dynamic_usage = gpu::DynamicUsage::None;

			scene->graphicsPipeline = gpu::GraphicsPipeline::Create(
				scene->renderpass, graphics_pipeline_usage, graphics_pipeline_layout, graphics_shaders, input_vertex_formatting,
				input_instance_formatting, graphics_topology, 0, graphics_viewports, graphics_scissors, graphics_rasterization,
				graphics_multisample, {}, graphics_blend, graphics_dynamic_usage, nullptr);

			con::vector<mem::sptr<gpu::ImageResourceView>> frame_image_views = scene->frameContext->GetImageViews();

			scene->framebuffers.resize(frame_image_views.size());
			for (siz i = 0; i < scene->framebuffers.size(); i++)
				scene->framebuffers[i] =
					gpu::Framebuffer::Create(scene->renderpass, scene->frameContext->GetFrameWidth(),
											 scene->frameContext->GetFrameHeight(), 1, {frame_image_views[i]});

			//<https://vulkan-tutorial.com/en/Drawing_a_triangle/Drawing/Command_buffers>

			*self._scene.get_access() = scene;
		}

		void SubmitCreateSceneJob()
		{
			NP_ENGINE_ASSERT(_window, "require valid window");
			mem::sptr<jsys::Job> create_scene_job = _services->GetJobSystem().CreateJob();
			create_scene_job->SetPayload(this);
			create_scene_job->SetCallback(CreateSceneCallback);
			_services->GetJobSystem().SubmitJob(jsys::JobPriority::Higher, create_scene_job);
		}

		nput::KeyCodeStates _prev_keys;
		nput::MouseCodeStates _prev_mouse;
		nput::MousePosition _prev_mouse_position;

		bl _mouse_is_dragging = false;

		void DigestInput(tim::milliseconds time_delta)
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
				_camera.projectionType = gpu::ProjectionType::Orthographic;

			if (keys[Key::P].IsActive())
				_camera.projectionType = gpu::ProjectionType::Perspective;

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
				/*geom::Transform& transform = _model->GetTransform();
				transform.scale.z = ::std::clamp(transform.scale.z + _rate, 1.f, 2.f);*/
			}

			if (keys[Key::B].IsActive())
			{
				// scale down
				/*geom::Transform& transform = _model->GetTransform();
				transform.scale.z = ::std::clamp(transform.scale.z - _rate, 1.f, 2.f);*/
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

		static void TcpServerAcceptClientCallback(mem::delegate& d)
		{
			GameLayer& self = *((GameLayer*)d.GetPayload());
			self._tcp_server->Accept(true);
		}

		void SubmitTcpServerAcceptClientJob()
		{
			mem::sptr<jsys::Job> job = _services->GetJobSystem().CreateJob();
			job->SetPayload(this);
			job->SetCallback(TcpServerAcceptClientCallback);
			_services->GetJobSystem().SubmitJob(jsys::JobPriority::Higher, job);
		}

		void TcpServerCloseClients()
		{
			auto clients = _tcp_server_clients.get_access();
			for (auto c = clients->begin(); c != clients->end(); c++)
				(*c)->Close();
		}

		static void ClientConnectToTcpServerCallback(mem::delegate& d)
		{
			GameLayer& self = *((GameLayer*)d.GetPayload());
			mem::sptr<net::Socket> client = net::Socket::Create(self._network_context);
			if (client)
			{
				client->Open(net::Protocol::Tcp);
				client->Enable({net::SocketOptions::ReuseAddress, net::SocketOptions::ReusePort});
				client->ConnectTo(net::Ipv4{127, 0, 0, 1}, 55555);
			}

			if (client)
				self._tcp_clients.get_access()->emplace_back(client);
		}

		void SubmitClientConnectToTcpServerJob()
		{
			mem::sptr<jsys::Job> job = _services->GetJobSystem().CreateJob();
			job->SetPayload(this);
			job->SetCallback(ClientConnectToTcpServerCallback);
			_services->GetJobSystem().SubmitJob(jsys::JobPriority::Higher, job);
		}

	public:
		GameLayer(mem::sptr<srvc::Services> services, WindowLayer& window_layer):
			Layer(services),
			_window_layer(window_layer),
			_window_id_handle(_services->GetUidSystem().CreateUid()),
			_window(_window_layer.Create(win::DetailType::Glfw, _services->GetUidSystem().GetUid(_window_id_handle))),
			_scene(nullptr),
			_model_filename(fsys::append(NP_ENGINE_WORKING_DIR, "test", "assets", "viking_room.obj")),
			_model_texture_filename(fsys::append(NP_ENGINE_WORKING_DIR, "test", "assets", "viking_room.png")),
			//_model(mem::create_sptr<gpu::Model>(_services->GetAllocator(), _model_filename, _model_texture_filename, true)),
			_model_handle(nullptr),
			_start_timestamp(tim::steady_clock::now()),
			_network_context(net::Context::Create(net::DetailType::Native, _services)),
			_clients_send_msg_timestamp(tim::steady_clock::now())
		{
			net::Init(net::DetailType::Native);

			//geom::Transform& transform = _model->GetTransform();

			/*transform.position = {0.f, 0.f, 0.f};
			transform.orientation = {0.f, 0.f, 0.f, 1.f};
			transform.scale = {1.f, 1.f, 1.f};*/

			::glm::quat rot(::glm::vec3{-M_PI_2, M_PI_2, 0.f});
			//transform.orientation *= rot;

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

		void Update(tim::milliseconds time_delta) override
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
							ss << thr::this_thread::get_id();
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
				ss << thr::this_thread::get_id();

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
				tim::steady_timestamp now = tim::steady_clock::now();
				if (now - _clients_send_msg_timestamp > tim::milliseconds(1000))
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
							msg_body.content = "Hello TCP Server!\n\t- tcp client: " + to_str((siz)mem::address_of(**c)) + " <3";
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
							"Hello UDP Server!\n\t- udp client: " + to_str((siz)mem::address_of(*_udp_client)) + " <3";
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
				/*auto scene = _scene.get_access();
				if (scene && *scene)
					(*scene)->CleanupVisibles();*/
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

			NP_ENGINE_ASSERT(thr::thread::hardware_concurrency() >= 4, "NP Engine Test requires at least four cores");
		}

	public:
		GameApp(mem::sptr<srvc::Services> services):
			Application(Application::Properties{"My Game App"}, services),
			_game_layer(services, _window_layer)
		{
			PushLayer(mem::address_of(_game_layer));
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