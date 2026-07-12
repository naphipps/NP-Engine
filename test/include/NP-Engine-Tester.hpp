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
		struct Vertex
		{
			glm::vec4 position{};
			glm::vec4 color{};
			glm::vec2 uv{};

			static con::vector<gpu::Format> GetInputFormatting()
			{
				return
				{ 
					gpu::Format{gpu::Format::Floating, 4, sizeof(flt)},
					gpu::Format{gpu::Format::Floating, 4, sizeof(flt)},
					gpu::Format{gpu::Format::Floating, 2, sizeof(flt)}
				};
			}
		};

		struct Ubo
		{
			alignas(BIT(4)) ::glm::mat4 model{};
			alignas(BIT(4)) ::glm::mat4 view{};
			alignas(BIT(4)) ::glm::mat4 projection{};
		};

		struct Scene
		{
			tim::steady_timestamp startTimestamp = tim::steady_clock::now();

			atm_bl isRendering = false;
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
			mem::sptr<gpu::ResourceGroupPool> resourceGroupPool = nullptr;

			siz frameCounter = 0;
			siz frameCount = SIZ_MAX;
			con::vector<mem::sptr<gpu::CommandBuffer>> commandBuffers{};
			con::vector<mem::sptr<gpu::Fence>> submitCompleteFences{};
			con::vector<mem::sptr<gpu::Semaphore>> frameReadySemaphores{};
			con::vector<mem::sptr<gpu::Semaphore>> submitCompleteSemaphores{};
			con::vector<mem::sptr<gpu::BufferResource>> uboBuffers{};
			mem::sptr<gpu::ImageResourceView> statueImageResourceView = nullptr;
			mem::sptr<gpu::SamplerResource> statueSamplerResource = nullptr;
			con::vector<con::vector<mem::sptr<gpu::ResourceGroup>>> resourceGroups{};

			con::vector<Vertex> vertices{};
			con::vector<ui16> indices{};
			mem::sptr<gpu::BufferResource> vertexBuffer = nullptr;
			mem::sptr<gpu::BufferResource> indexBuffer = nullptr;

			Scene() : isRendering(false) {}

			~Scene()
			{
				if (device)
					device->WaitUntilIdle();
			}

			void EnsureFramebuffers(const con::vector<mem::sptr<gpu::Frame>>& frames)
			{
				framebuffers.resize(frameCount);
				for (siz i = 0; i < framebuffers.size(); i++)
					framebuffers[i] =
					gpu::Framebuffer::Create(renderpass, frameContext->GetFrameWidth(),
						frameContext->GetFrameHeight(), 1, { frames[i]->GetImageResourceView() });
			}

			void EnsureFrameCommandBuffers()
			{
				commandBuffers.resize(frameCount);
				for (auto it = commandBuffers.begin(); it != commandBuffers.end(); it++)
					if (!*it)
						*it = commandBufferPool->CreateCommandBuffer();
			}

			void EnsureFrameSyncronizations()
			{
				submitCompleteFences.resize(frameCount);
				for (auto it = submitCompleteFences.begin(); it != submitCompleteFences.end(); it++)
					if (!*it)
						*it = device->CreateFence();

				submitCompleteSemaphores.resize(frameCount);
				for (auto it = submitCompleteSemaphores.begin(); it != submitCompleteSemaphores.end(); it++)
					if (!*it)
						*it = device->CreateSemaphore();

				frameReadySemaphores.resize(frameCount);
				for (auto it = frameReadySemaphores.begin(); it != frameReadySemaphores.end(); it++)
					if (!*it)
						*it = device->CreateSemaphore();
			}

			void EnsureFrameResources()
			{
				if (!statueImageResourceView)
				{
					gpu::Image statue_image{ fsys::append("test", "assets", "statue-512x512.png") };

					mem::sptr<gpu::BufferResource> statue_buffer_resource = gpu::BufferResource::Create(device,
						gpu::BufferResourceUsage::HostAccessible | gpu::BufferResourceUsage::Transfer | gpu::BufferResourceUsage::Read,
						mem::calc_aligned_size(statue_image.Size(), mem::DEFAULT_ALIGNMENT), { queue->GetDeviceQueueFamily() });

					con::vector<ui8> statue_bytes(statue_image.Size());
					mem::copy_bytes(statue_bytes.data(), statue_image.Data(), statue_bytes.size());
					statue_buffer_resource->SetBytes(0, statue_bytes);
					statue_buffer_resource->ClearCacheForDevice(0, statue_buffer_resource->GetSize());

					mem::sptr<gpu::ImageResource> statue_image_resource = gpu::ImageResource::Create(device,
						gpu::ImageResourceUsage::DeviceLocal | 
						gpu::ImageResourceUsage::Transfer | gpu::ImageResourceUsage::Write |
						gpu::ImageResourceUsage::Sampled,
						gpu::Format{ gpu::Format::Unsigned | gpu::Format::Integer | gpu::Format::GammaCorrection, 4, sizeof(ui8) },
						1, 1, 1, statue_image.GetWidth(), statue_image.GetHeight(), 1,
						statue_buffer_resource->GetDeviceQueueFamilies());

					mem::sptr<srvc::Services> services = detailInstance->GetServices();

					gpu::ImageBarrier image_barrier_0{};
					image_barrier_0.dstAccess = gpu::Access::Transfer | gpu::Access::Write;
					image_barrier_0.srcAccess = gpu::Access::None;
					image_barrier_0.image = statue_image_resource;
					image_barrier_0.dstImageResourceUsage = gpu::ImageResourceUsage::Transfer | gpu::ImageResourceUsage::Write;
					image_barrier_0.srcImageResourceUsage = gpu::ImageResourceUsage::None;
					image_barrier_0.dstQueueFamily = queue->GetDeviceQueueFamily();
					image_barrier_0.srcQueueFamily = queue->GetDeviceQueueFamily();
					image_barrier_0.range.usage = gpu::ImageResourceUsage::Color;

					gpu::ImageLayers image_layers{};
					image_layers.usage = gpu::ImageResourceUsage::Color;

					gpu::CopyBufferToImageRange copy_buffer_to_image_range{};
					copy_buffer_to_image_range.imageSize = { statue_image_resource->GetWidth(), statue_image_resource->GetHeight(), 1 };
					copy_buffer_to_image_range.bufferRowCount = copy_buffer_to_image_range.imageSize.y;
					copy_buffer_to_image_range.bufferRowLength = copy_buffer_to_image_range.imageSize.x;
					copy_buffer_to_image_range.imageLayers = { image_layers };

					gpu::ImageBarrier image_barrier_1{};
					image_barrier_1.dstAccess = gpu::Access::Shader | gpu::Access::Read;
					image_barrier_1.srcAccess = image_barrier_0.dstAccess;
					image_barrier_1.image = image_barrier_0.image;
					image_barrier_1.dstImageResourceUsage = gpu::ImageResourceUsage::Shader | gpu::ImageResourceUsage::Read;
					image_barrier_1.srcImageResourceUsage = image_barrier_0.dstImageResourceUsage;
					image_barrier_1.dstQueueFamily = image_barrier_0.dstQueueFamily;
					image_barrier_1.srcQueueFamily = image_barrier_0.srcQueueFamily;
					image_barrier_1.range.usage = image_barrier_0.range.usage;

					mem::sptr<gpu::BarrierCommand> barrier_cmd_0 = 
						gpu::Command::Create(gpu::DetailType::Vulkan, services, gpu::CommandType::Barrier);
					barrier_cmd_0->dstStage = gpu::Stage::Transfer;
					barrier_cmd_0->srcStage = gpu::Stage::Top;
					barrier_cmd_0->imageBarriers = { image_barrier_0 };

					mem::sptr<gpu::CopyBufferToImageCommand> copy_buffer_to_image_cmd = 
						gpu::Command::Create(gpu::DetailType::Vulkan, services, gpu::CommandType::CopyBufferToImage);
					copy_buffer_to_image_cmd->buffer = statue_buffer_resource;
					copy_buffer_to_image_cmd->image = statue_image_resource;
					copy_buffer_to_image_cmd->imageUsage = gpu::ImageResourceUsage::Transfer | gpu::ImageResourceUsage::Write;
					copy_buffer_to_image_cmd->ranges = { copy_buffer_to_image_range };

					mem::sptr<gpu::BarrierCommand> barrier_cmd_1 = 
						gpu::Command::Create(gpu::DetailType::Vulkan, services, gpu::CommandType::Barrier);
					barrier_cmd_1->dstStage = gpu::Stage::VertexInput;
					barrier_cmd_1->srcStage = gpu::Stage::Transfer;
					barrier_cmd_1->imageBarriers = { image_barrier_1 };

					mem::sptr<gpu::CommandBuffer> command_buffer_0 = commandBufferPool->CreateCommandBuffer();
					mem::sptr<gpu::CommandBuffer> command_buffer_1 = commandBufferPool->CreateCommandBuffer();
					mem::sptr<gpu::CommandBuffer> command_buffer_2 = commandBufferPool->CreateCommandBuffer();

					commandBufferPool->Begin(command_buffer_0, gpu::CommandBufferUsage::SingleUse);
					command_buffer_0->Add(barrier_cmd_0);
					commandBufferPool->End(command_buffer_0, gpu::CommandBufferUsage::None);

					commandBufferPool->Begin(command_buffer_1, gpu::CommandBufferUsage::SingleUse);
					command_buffer_1->Add(copy_buffer_to_image_cmd);
					commandBufferPool->End(command_buffer_1, gpu::CommandBufferUsage::None);

					commandBufferPool->Begin(command_buffer_2, gpu::CommandBufferUsage::SingleUse);
					command_buffer_2->Add(barrier_cmd_1);
					commandBufferPool->End(command_buffer_2, gpu::CommandBufferUsage::None);

					mem::sptr<gpu::Semaphore> semaphore_0 = device->CreateSemaphore();
					mem::sptr<gpu::Semaphore> semaphore_1 = device->CreateSemaphore();
					mem::sptr<gpu::Fence> fence = device->CreateFence();

					gpu::Submit submit_0{};
					submit_0.commandBuffers = { command_buffer_0 };
					submit_0.signalSemaphores = { semaphore_0 };

					gpu::Submit submit_1{};
					submit_1.commandBuffers = { command_buffer_1 };
					submit_1.waitStageSemaphores = { {gpu::Stage::Transfer, semaphore_0} };
					submit_1.signalSemaphores = { semaphore_1 };
					
					gpu::Submit submit_2{};
					submit_2.commandBuffers = { command_buffer_2};
					submit_2.waitStageSemaphores = { {gpu::Stage::VertexInput, semaphore_1} };

					fence->Reset();
					queue->Submit({ submit_0, submit_1, submit_2 }, fence);
					fence->Wait();

					statueImageResourceView = gpu::ImageResourceView::Create(device, statue_image_resource, gpu::ImageResourceUsage::Color);
				}

				if (!statueSamplerResource)
				{
					gpu::SamplerAddressModes address_modes{};
					address_modes.u = gpu::SamplerAddressMode::Repeat;
					address_modes.v = gpu::SamplerAddressMode::Repeat;
					address_modes.w = gpu::SamplerAddressMode::Repeat;

					statueSamplerResource = gpu::SamplerResource::Create(device, 
						gpu::SamplerResourceUsage::MinimizeLinear | gpu::SamplerResourceUsage::MagnifyLinear | gpu::SamplerResourceUsage::MipmapLinear, 
						DBL_MAX, //TODO: I'd like to be able to query the max value here
						gpu::CompareOperation::None, 
						gpu::LodBounds{}, 
						gpu::SamplerBorder::Opaque | gpu::SamplerBorder::Black, 
						address_modes);
				}

				uboBuffers.resize(frameCount);
				for (auto it = uboBuffers.begin(); it != uboBuffers.end(); it++)
					if (!*it)
						*it = gpu::BufferResource::Create(device,
							gpu::BufferResourceUsage::Uniform | gpu::BufferResourceUsage::HostAccessible | gpu::BufferResourceUsage::AutoClearCache,
							mem::calc_aligned_size(sizeof(Ubo), mem::DEFAULT_ALIGNMENT), {queue->GetDeviceQueueFamily()});

				mem::sptr<gpu::PipelineResourceLayout> pipeline_layout = graphicsPipeline->GetPipelineResourceLayout();

				bl is_resource_group_pool_reset = false;
				if (!resourceGroupPool || frameCount != resourceGroupPool->GetSize())
				{
					resourceGroups.clear();
					resourceGroupPool = gpu::ResourceGroupPool::Create(device, frameCount, pipeline_layout->GetResourceDesciptions());
					is_resource_group_pool_reset = true;
				}

				con::vector<mem::sptr<gpu::ResourceLayout>> resource_layouts = pipeline_layout->GetResourceLayouts();

				resourceGroups.resize(frameCount);
				for (auto it = resourceGroups.begin(); it != resourceGroups.end(); it++)
					if (it->size() != resource_layouts.size() || is_resource_group_pool_reset)
						*it = resourceGroupPool->CreateResourceGroups(resource_layouts);
			}

			void EnsureFrames()
			{
				con::vector<mem::sptr<gpu::Frame>> frames = frameContext->GetFrames();
				frameCounter = 0;
				frameCount = frames.size();

				EnsureFramebuffers(frames);
				EnsureFrameCommandBuffers();
				EnsureFrameSyncronizations();
				EnsureFrameResources();
			}

			void RebuildFrames()
			{
				device->WaitUntilIdle();
				frameContext->Rebuild();
				EnsureFrames();
			}

			void Render()
			{
				bl expected = false;
				if (isRendering.compare_exchange_strong(expected, true, mo_release, mo_relaxed))
				{
					mem::sptr<gpu::Semaphore> frame_ready_semaphore = frameReadySemaphores[frameCounter];
					gpu::Result acquire_result = frameContext->TryAcquireFrame(frame_ready_semaphore, nullptr);
					if (acquire_result.Contains(gpu::Result::OutOfDate))
					{
						RebuildFrames();
					}
					else if (acquire_result.ContainsAny(gpu::Result::Success | gpu::Result::NotReady))
					{
						mem::sptr<gpu::Fence> submit_complete_fence = submitCompleteFences[frameCounter];
						submit_complete_fence->Wait();
						submit_complete_fence->Reset();

						tim::seconds s = startTimestamp - tim::steady_clock::now();

						Ubo ubo{};
						ubo.model = ::glm::rotate(::glm::mat4{ 1 }, ::glm::radians(90.f) * (flt)s.count(), ::glm::vec3{0, 0, 1});
						ubo.view = ::glm::lookAt(::glm::vec3{ 2, 2, 2 }, ::glm::vec3{ 0, 0, 0 }, ::glm::vec3{ 0, 0, 1 });
						ubo.projection = ::glm::perspective(::glm::radians(45.f), (flt)frameContext->GetFrameAspectRatio(), 0.1f, 10.f);
						ubo.projection[1][1] *= -1; //compensate for glm legacy where y axis was inverted
						con::vector<ui8> ubo_bytes(sizeof(Ubo));
						mem::copy_bytes(ubo_bytes.data(), &ubo, ubo_bytes.size());
						uboBuffers[frameCounter]->SetBytes(0, ubo_bytes);
						uboBuffers[frameCounter]->ClearCacheForDevice(0, ubo_bytes.size());

						con::vector<mem::sptr<gpu::ResourceGroup>>& resource_group = resourceGroups[frameCounter];
						gpu::Result assignment_result = resource_group[0]->ApplyResourceAssignments({ 
							{
								{1, {{statueImageResourceView, statueSamplerResource, gpu::ImageResourceUsage::Shader}}}
							},
							{
								{0, {{uboBuffers[frameCounter], 0, uboBuffers[frameCounter]->GetSize()}}}
							},
						{} });

						mem::sptr<srvc::Services> services = detailInstance->GetServices();
						mem::sptr<gpu::Frame> frame = frameContext->GetAcquiredFrame();

						mem::sptr<gpu::BeginRenderPassCommand> begin_renderpass_cmd =
							gpu::Command::Create(gpu::DetailType::Vulkan, services, gpu::CommandType::BeginRenderPass);
						begin_renderpass_cmd->framebuffer = framebuffers[frameContext->GetAcquiredFrameIndex()];
						begin_renderpass_cmd->renderArea = {
							{/*no offset*/}, frameContext->GetFrameWidth(), frameContext->GetFrameHeight() };
						begin_renderpass_cmd->clearColors = { gpu::ClearColor{} };

						mem::sptr<gpu::BindPipelineCommand> bind_pipeline_cmd =
							gpu::Command::Create(gpu::DetailType::Vulkan, services, gpu::CommandType::BindPipeline);
						bind_pipeline_cmd->pipeline = graphicsPipeline;
						bind_pipeline_cmd->usage = gpu::PipelineUsage::Graphics;

						mem::sptr<gpu::SetViewportsCommand> set_viewports_cmd =
							gpu::Command::Create(gpu::DetailType::Vulkan, services, gpu::CommandType::SetViewports);
						set_viewports_cmd->viewports = { {{}, (dbl)frameContext->GetFrameWidth(), (dbl)frameContext->GetFrameHeight(), {0, 1}} };

						mem::sptr<gpu::SetScissorsCommand> set_scissors_cmd =
							gpu::Command::Create(gpu::DetailType::Vulkan, services, gpu::CommandType::SetScissors);
						set_scissors_cmd->scissors = { {{}, frameContext->GetFrameWidth(), frameContext->GetFrameHeight()} };

						mem::sptr<gpu::BindVertexBuffersCommand> bind_vertex_buffers_cmd =
							gpu::Command::Create(gpu::DetailType::Vulkan, services, gpu::CommandType::BindVertexBuffers);
						bind_vertex_buffers_cmd->contexts = { {vertexBuffer, 0} };

						mem::sptr<gpu::BindIndexBufferCommand> bind_index_buffer_cmd =
							gpu::Command::Create(gpu::DetailType::Vulkan, services, gpu::CommandType::BindIndexBuffer);
						bind_index_buffer_cmd->buffer = indexBuffer;
						bind_index_buffer_cmd->stride = sizeof(ui16);

						mem::sptr<gpu::BindResourceGroupsCommand> bind_resource_groups_cmd =
							gpu::Command::Create(gpu::DetailType::Vulkan, services, gpu::CommandType::BindResourceGroups);
						bind_resource_groups_cmd->pipeline = graphicsPipeline;
						bind_resource_groups_cmd->resourceGroups = { resourceGroups[frameCounter] };

						mem::sptr<gpu::DrawIndexedCommand> draw_indexed_cmd =
							gpu::Command::Create(gpu::DetailType::Vulkan, services, gpu::CommandType::DrawIndexed);
						draw_indexed_cmd->indexCount = indices.size();

						mem::sptr<gpu::EndRenderPassCommand> end_renderpass_cmd =
							gpu::Command::Create(gpu::DetailType::Vulkan, services, gpu::CommandType::EndRenderPass);
						end_renderpass_cmd->framebuffer = begin_renderpass_cmd->framebuffer;

						mem::sptr<gpu::CommandBuffer> command_buffer = commandBuffers[frameCounter];
						mem::sptr<gpu::Semaphore> submit_complete_semaphore = submitCompleteSemaphores[frameCounter];

						commandBufferPool->Reset(command_buffer, gpu::CommandBufferUsage::None);
						commandBufferPool->Begin(command_buffer, gpu::CommandBufferUsage::SingleUse);
						command_buffer->Add(begin_renderpass_cmd);
						command_buffer->Add(bind_pipeline_cmd);
						command_buffer->Add(set_viewports_cmd);
						command_buffer->Add(set_scissors_cmd);
						command_buffer->Add(bind_vertex_buffers_cmd);
						command_buffer->Add(bind_index_buffer_cmd);
						command_buffer->Add(bind_resource_groups_cmd);
						command_buffer->Add(draw_indexed_cmd);
						command_buffer->Add(end_renderpass_cmd);
						commandBufferPool->End(command_buffer, gpu::CommandBufferUsage::None);

						gpu::Submit submit{};
						submit.commandBuffers = { command_buffer };
						submit.waitStageSemaphores = { {gpu::Stage::PresentComplete, frame_ready_semaphore} };
						submit.signalSemaphores = { submit_complete_semaphore };
						bl submit_success = queue->Submit({ submit }, submit_complete_fence);

						gpu::Present present{};
						present.frameContexts = { frameContext };
						present.waitSemaphores = { submit_complete_semaphore };
						gpu::PresentResults present_results = queue->Present(present);

						bl should_rebuild_frames = present_results.overallResult.ContainsAny(gpu::Result::OutOfDate | gpu::Result::Suboptimal);
						for (siz i = 0; !should_rebuild_frames && i < present_results.individualResults.size(); i++)
							should_rebuild_frames |= present_results.individualResults[i].ContainsAny(gpu::Result::OutOfDate | gpu::Result::Suboptimal);

						frameCounter = (frameCounter + 1) % frameCount;

						if (should_rebuild_frames)
							RebuildFrames();
					}

					isRendering.store(false, mo_release);
				}
			}
		};

		WindowLayer& _window_layer;
		mem::sptr<uid::UidHandle> _window_id_handle;
		mem::sptr<win::Window> _window;
		mutexed_wrapper<mem::sptr<Scene>> _scene; //mutexed wrapper helps smooth rendering on window resize
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

		static void LogFramebufferSize(void*, ::glm::uvec2)
		{
			NP_ENGINE_LOG_INFO("framebuffer callback");
		}

		static void LogPosition(void*, ::glm::ivec2)
		{
			NP_ENGINE_LOG_INFO("position callback");
		}

		static void LogSize(void*, ::glm::uvec2)
		{
			NP_ENGINE_LOG_INFO("size callback");
		}

		static void RenderOnFramebufferSize(void* caller, ::glm::uvec2 framebuffer_size)
		{
			if (framebuffer_size.x != 0 && framebuffer_size.y != 0)
				(*static_cast<GameLayer*>(caller)->_scene.get_access())->Render();
		}

		static void RenderOnSize(void* caller, ::glm::uvec2 size)
		{
			if (size.x != 0 && size.y != 0)
				(*static_cast<GameLayer*>(caller)->_scene.get_access())->Render();
		}

		static bl PreventWindowClose(void* caller)
		{
			GameLayer& self = *static_cast<GameLayer*>(caller);
			win::PopupSelection selection = win::Popup::Show(self._window, "Close Window?", "Testing: do we want to close this window?",
				win::PopupStyle::Question, win::PopupButtons::Yes | win::PopupButtons::No | win::PopupButtons::Cancel);
			return !selection.Equals(win::PopupSelection::Yes);
		}

		/*
			aka: close our application when our last window is destroyed
		*/
		void HandleWindowDestroyEvent(mem::sptr<evnt::Event> e)
		{
			if (e->GetEventType().Contains(evnt::EventType::Did))
			{
				mem::sptr<win::WindowDestroyEvent> event = e;
				win::WindowEventData& data = event->GetData();

				if (_window && _window->GetUid() == data.windowId)
				{
					//we could reset the scene sptr here
					_window_id_handle.reset();
					_window.reset();

					mem::sptr<evnt::Event> app_close_event =
						mem::create_sptr<ApplicationCloseEvent>(_services->GetAllocator(), evnt::EventType::Will);
					_services->GetEventSubmitter().Submit(app_close_event);
				}
			}
		}

		void HandleNetworkClientEvent(mem::sptr<evnt::Event> e)
		{
			mem::sptr<net::NetworkClientEvent> event = e;
			net::NetworkClientEventData& data = event->GetData();

			str name = "UNKNOWN";
			if (data.host)
			{
				if (data.host->name != "")
					name = data.host->name;
				else if (!data.host->ipv4s.empty())
					name = to_str(data.host->ipv4s.begin()->first);
				else if (!data.host->ipv6s.empty())
					name = to_str(data.host->ipv6s.begin()->first);
			}

			if (data.socket && *data.socket)
			{
				NP_ENGINE_LOG_INFO("server accepted: " + name);

				_tcp_server_clients.get_access()->emplace_back(data.socket);
				data.socket->StartReceiving();
			}
			else
			{
				NP_ENGINE_LOG_INFO("server denied: " + name);
			}
		}

		void HandleApplicationEvent(mem::sptr<evnt::Event> e)
		{
			/*TcpServerCloseClients();
			_udp_server->Close();
			_http_socket->Close();*/
		}

		void HandleWindowEvent(mem::sptr<evnt::Event> e)
		{
			switch (e->GetEventType().GetTopic())
			{
			case evnt::EventType::Destroy:
				HandleWindowDestroyEvent(e);
				break;
			}
		}

		void HandleNetworkEvent(mem::sptr<evnt::Event> e)
		{

		}

		void HandleEvent(mem::sptr<evnt::Event> e) override
		{
			switch (e->GetEventType().GetCategory())
			{
			case evnt::EventType::Application:
				HandleApplicationEvent(e);
				break;

			case evnt::EventType::Window:
				HandleWindowEvent(e);
				break;

			case evnt::EventType::Network:
				HandleNetworkEvent(e);
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
			//*
			//self._window->SetFramebufferSizeCallback(mem::address_of(self), RenderOnFramebufferSize);
			self._window->SetSizeCallback(mem::address_of(self), RenderOnSize);
			//*/
			/*
			self._window->SetPreventCloseCallback(mem::address_of(self), PreventWindowClose);
			//*/
			/*
			self._window->SetKeyCallback(mem::address_of(self), LogSubmitKeyState);
			self._window->SetMouseCallback(mem::address_of(self), LogSubmitMouseState);
			//self._window->SetMousePositionCallback(mem::address_of(self), LogSubmitMousePosition);
			self._window->SetControllerCallback(mem::address_of(self), LogSubmitControllerState);
			//self._window->SetFocusCallback(mem::address_of(self), LogFocus);
			//self._window->SetFramebufferSizeCallback(mem::address_of(self), LogFramebufferSize);
			self._window->SetMaximizeCallback(mem::address_of(self), LogMaximize);
			self._window->SetMinimizeCallback(mem::address_of(self), LogMinimize);
			//self._window->SetPositionCallback(mem::address_of(self), LogPosition);
			//self._window->SetSizeCallback(mem::address_of(self), LogSize);
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
			gpu::PipelineUsage graphics_pipeline_usage = gpu::PipelineUsage::Graphics | gpu::PipelineUsage::ColorBlend | gpu::PipelineUsage::Dynamic;

			gpu::ResourceDescription ubo_description
			{
				gpu::ResourceType::Buffer, gpu::BufferResourceUsage::Uniform, 1, gpu::Stage::VertexInput
			};

			gpu::ResourceDescription image_sampler_description
			{
				gpu::ResourceType::Image | gpu::ResourceType::Sampler, gpu::ImageResourceUsage::None, 1, gpu::Stage::FragmentInput
			};

			mem::sptr<gpu::ResourceLayout> graphics_resource_layout = gpu::ResourceLayout::Create(scene->device, { ubo_description, image_sampler_description });

			mem::sptr<gpu::PipelineResourceLayout> graphics_pipeline_layout =
				gpu::PipelineResourceLayout::Create(scene->device, { graphics_resource_layout }, {});

			con::vector<mem::sptr<gpu::Shader>> graphics_shaders{scene->vertexShader, scene->fragmentShader};
			con::vector<gpu::Format> input_vertex_formatting = Vertex::GetInputFormatting();
			con::vector<gpu::Format> input_instance_formatting{};
			gpu::PrimitiveTopology graphics_topology = gpu::PrimitiveTopology::Triangle | gpu::PrimitiveTopology::List;
			con::vector<gpu::Viewport> graphics_viewports{
				{{}, (dbl)scene->frameContext->GetFrameWidth(), (dbl)scene->frameContext->GetFrameHeight(), {0, 1}}};
			con::vector<gpu::Scissor> graphics_scissors{
				{{}, scene->frameContext->GetFrameWidth(), scene->frameContext->GetFrameHeight()}};
			gpu::Rasterization graphics_rasterization{gpu::RasterizationUsage::PolygonFill | gpu::RasterizationUsage::CullBack, {}};
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
			gpu::DynamicUsage graphics_dynamic_usage = gpu::DynamicUsage::Scissor | gpu::DynamicUsage::Viewport;

			scene->graphicsPipeline = gpu::GraphicsPipeline::Create(
				scene->renderpass, graphics_pipeline_usage, graphics_pipeline_layout, graphics_shaders, input_vertex_formatting,
				input_instance_formatting, graphics_topology, 0, graphics_viewports, graphics_scissors, graphics_rasterization,
				graphics_multisample, {}, graphics_blend, graphics_dynamic_usage, nullptr);

			scene->commandBufferPool = scene->queue->CreateCommandBufferPool(gpu::CommandBufferPoolUsage::Resettable);
			
			scene->EnsureFrames();

			//<https://vulkan-tutorial.com/en/Drawing_a_triangle/Drawing/Command_buffers>

			scene->vertices = {
				{{-0.5, -0.5, 0, 1}, {1, 0, 1, 1}, {1.f, 0.f}},
				{{0.5, -0.5, 0, 1}, {1, 1, 0, 1}, {0.f, 0.f}},
				{{0.5, 0.5, 0, 1}, {0, 1, 1, 1}, {0.f, 1.f}},
				{{-0.5, 0.5, 0, 1}, {0, 1, 0, 1}, {1.f, 1.f}}
			};

			scene->indices = {0, 1, 2, 2, 3, 0};

			con::vector<ui8> vertex_buffer_bytes(sizeof(Vertex) * scene->vertices.size());
			mem::copy_bytes(vertex_buffer_bytes.data(), scene->vertices.data(), vertex_buffer_bytes.size());

			con::vector<ui8> index_buffer_bytes(sizeof(ui32) * scene->indices.size());
			mem::copy_bytes(index_buffer_bytes.data(), scene->indices.data(), index_buffer_bytes.size());

			const gpu::BufferResourceUsage vertex_buffer_usage = gpu::BufferResourceUsage::Vertex | gpu::BufferResourceUsage::DeviceLocal |
				gpu::BufferResourceUsage::Transfer | gpu::BufferResourceUsage::Write;
			scene->vertexBuffer = gpu::BufferResource::Create(scene->device, vertex_buffer_usage, vertex_buffer_bytes.size(), {scene->queue->GetDeviceQueueFamily()});

			const gpu::BufferResourceUsage index_buffer_usage = gpu::BufferResourceUsage::Index | gpu::BufferResourceUsage::DeviceLocal |
				gpu::BufferResourceUsage::Transfer | gpu::BufferResourceUsage::Write;
			scene->indexBuffer = gpu::BufferResource::Create(scene->device, index_buffer_usage, index_buffer_bytes.size(), { scene->queue->GetDeviceQueueFamily() });

			const gpu::BufferResourceUsage staging_vertex_buffer_usage = gpu::BufferResourceUsage::Vertex | gpu::BufferResourceUsage::HostAccessible |
				gpu::BufferResourceUsage::Transfer | gpu::BufferResourceUsage::Read;
			mem::sptr<gpu::BufferResource> staging_vertex_buffer =
				gpu::BufferResource::Create(scene->device, staging_vertex_buffer_usage, vertex_buffer_bytes.size(), { scene->queue->GetDeviceQueueFamily() });

			const gpu::BufferResourceUsage staging_index_buffer_usage = gpu::BufferResourceUsage::Index | gpu::BufferResourceUsage::HostAccessible |
				gpu::BufferResourceUsage::Transfer | gpu::BufferResourceUsage::Read;
			mem::sptr<gpu::BufferResource> staging_index_buffer =
				gpu::BufferResource::Create(scene->device, staging_index_buffer_usage, index_buffer_bytes.size(), { scene->queue->GetDeviceQueueFamily() });

			staging_vertex_buffer->SetBytes(0, vertex_buffer_bytes);
			staging_vertex_buffer->ClearCacheForDevice(0, vertex_buffer_bytes.size());
			staging_index_buffer->SetBytes(0, index_buffer_bytes);
			staging_index_buffer->ClearCacheForDevice(0, index_buffer_bytes.size());

			mem::sptr<gpu::CopyBufferCommand> copy_vertex_buffer_cmd = gpu::Command::Create(gpu::DetailType::Vulkan, services, gpu::CommandType::CopyBuffer);
			copy_vertex_buffer_cmd->dst = scene->vertexBuffer;
			copy_vertex_buffer_cmd->src = staging_vertex_buffer;
			copy_vertex_buffer_cmd->ranges = { {0, 0, vertex_buffer_bytes.size()} };

			mem::sptr<gpu::CopyBufferCommand> copy_index_buffer_cmd = gpu::Command::Create(gpu::DetailType::Vulkan, services, gpu::CommandType::CopyBuffer);
			copy_index_buffer_cmd->dst = scene->indexBuffer;
			copy_index_buffer_cmd->src = staging_index_buffer;
			copy_index_buffer_cmd->ranges = { {0, 0, index_buffer_bytes.size()} };
			
			mem::sptr<gpu::CommandBufferPool> command_buffer_pool = scene->queue->CreateCommandBufferPool(gpu::CommandBufferPoolUsage::None);
			mem::sptr<gpu::CommandBuffer> command_buffer = command_buffer_pool->CreateCommandBuffer();
			command_buffer_pool->Begin(command_buffer, gpu::CommandBufferUsage::SingleUse);
			command_buffer->Add(copy_vertex_buffer_cmd);
			command_buffer->Add(copy_index_buffer_cmd);
			command_buffer_pool->End(command_buffer, gpu::CommandBufferUsage::None);

			gpu::Submit submit{};
			submit.commandBuffers = { command_buffer };
			mem::sptr<gpu::Fence> fence = scene->device->CreateFence();
			fence->Reset();
			scene->queue->Submit({ submit }, fence);
			fence->Wait();

			self._scene = scene;
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
			_window(_window_layer.CreateWindow(win::DetailType::Glfw, _services->GetUidSystem().GetUid(_window_id_handle))),
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

			/*
			_tcp_server = net::Socket::Create(_network_context);
			_tcp_server->Open(net::Protocol::Tcp);
			_tcp_server->Enable({net::SocketOptions::ReuseAddress, net::SocketOptions::ReusePort});
			_tcp_server->BindTo(net::Ipv4{127, 0, 0, 1}, 55555);
			_tcp_server->Listen();
			SubmitTcpServerAcceptClientJob();
			//*/

			//-----------------------------------------------------------

			/*
			_udp_server = net::Socket::Create(_network_context);
			_udp_server->Open(net::Protocol::Udp);
			_udp_server->Enable({net::SocketOptions::ReuseAddress, net::SocketOptions::ReusePort});
			_udp_server->BindTo(net::Ipv4{127, 0, 0, 1}, 54555);
			_udp_server->StartReceiving();

			_udp_client = net::Socket::Create(_network_context);
			_udp_client->Open(net::Protocol::Udp);
			_udp_client->Enable({net::SocketOptions::ReuseAddress, net::SocketOptions::ReusePort});
			//*/

			//-----------------------------------------------------------

			/*
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
			//*/

			//-----------------------------------------------------------

			//SubmitClientConnectToTcpServerJob();
		}

		~GameLayer()
		{
			//TcpServerCloseClients();
			//_udp_server->Close();
			//_http_socket->Close();

			net::Terminate(net::DetailType::Native);
		}

		void Render()
		{
			auto scene = _scene.get_access();
			if (scene && *scene && !(*scene)->presentTarget->GetWindow()->IsMinimized())
				(*scene)->Render();
		}

		bl IsWindowMaximized() const
		{
			return _window && _window->IsMaximized();
		}

		virtual void Update(tim::milliseconds time_delta) override
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
			if (false)
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
			if (false)
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
			if (false)
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
			if (false)
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

		virtual bl CanHandle(evnt::EventType type) const override
		{
			return true;
		}
	};

	class GameApp : public Application
	{
	private:
		GameLayer _game_layer;
		atm_bl _keep_running;

		static void AppLoopCallback(mem::delegate& d)
		{
			NP_ENGINE_PROFILE_FUNCTION();
			NP_ENGINE_LOG_INFO("App Loop Start");

			GameApp& self = *((GameApp*)d.GetPayload());
			evnt::EventQueue& event_queue = self._services->GetEventQueue();
			nput::InputQueue& input_queue = self._services->GetInputQueue();

			thr::thread_duration_sleeper sleeper{ self.GetPlatformDefaultApplicationLoopDuration() / 2 };
			tim::steady_timestamp next = tim::steady_clock::now();
			tim::steady_timestamp prev = next;
			tim::milliseconds delta{};

			mem::sptr<evnt::Event> e = nullptr;
			con::vector<Layer*>::iterator it{};

			while (self._keep_running.load(mo_acquire))
			{
				NP_ENGINE_PROFILE_SCOPE("app loop");

				self.PublishEvents();

				if (!self._keep_running.load(mo_acquire))
					break;

				next = tim::steady_clock::now();
				delta = next - prev;
				prev = next;

				input_queue.ApplySubmissions();

				for (it = self._layers.begin(); it != self._layers.end(); it++)
					(*it)->BeforeUpdate();

				for (it = self._layers.begin(); it != self._layers.end(); it++)
					(*it)->Update(delta);

				for (it = self._layers.begin(); it != self._layers.end(); it++)
					(*it)->AfterUpdate();

				for (it = self._layers.begin(); it != self._layers.end(); it++)
					(*it)->CleanupUpdate();

				sleeper.sleep();
			}

			event_queue.Clear();

			NP_ENGINE_LOG_INFO("App Loop End");
		}

		static void RenderCallback(mem::delegate& d)
		{
			NP_ENGINE_PROFILE_FUNCTION();
			NP_ENGINE_LOG_INFO("Rendering Job Start");

			GameApp& self = *((GameApp*)d.GetPayload());
			thr::thread_duration_sleeper sleeper{ self.GetPlatformDefaultApplicationLoopDuration() };

			while (self._keep_running.load(mo_acquire))
			{
				self._game_layer.Render();

				//TODO: if our window is maximized or full screen, we can render like crazy

				if (self._game_layer.IsWindowMaximized())
					sleeper.reset();
				else
					sleeper.sleep();
			}

			NP_ENGINE_LOG_INFO("Rendering Job End");
		}

		void CustomizeJobSystem()
		{
			const ui32 core_count = thr::thread::hardware_concurrency();
			NP_ENGINE_ASSERT(core_count >= 4, "NP Engine Test requires at least four cores");
			const siz worker_count = ::std::max(core_count, 8u);

			/*
				Thread-to-Core Layout:
					- core 0: main thread (polling loop) and worker 0 (app loop)
						- I'm fine with core crowding here since the polling loop should not be intensive enough to consistently throttle app loop
						- TODO: ^ investigate
					- core 1: worker 1 (rendering)
					- core 2+: worker 2+ for normal job executing
			*/

			jsys::JobSystem& job_system = _services->GetJobSystem();
			job_system.SetIsOffsettingWorkerThreadAffinity(false);
			job_system.SetJobWorkerCount(worker_count);

			con::vector<jsys::JobWorker>& job_workers = job_system.GetJobWorkers();
			jsys::JobWorker& app_loop_worker = job_workers[0];
			jsys::JobWorker& render_worker = job_workers[1];

			app_loop_worker.ClearCoworkers();
			render_worker.ClearCoworkers();
			for (auto it = job_workers.begin(); it != job_workers.end(); it++)
			{
				it->RemoveCoworker(app_loop_worker);
				it->RemoveCoworker(render_worker);
			}

			mem::sptr<jsys::Job> app_loop_job = job_system.CreateJob();
			app_loop_job->SetCanBeStolen(false);
			app_loop_job->SetPayload(this);
			app_loop_job->SetCallback(AppLoopCallback);
			app_loop_worker.SubmitImmediateJob(app_loop_job);

			mem::sptr<jsys::Job> render_job = job_system.CreateJob();
			render_job->SetCanBeStolen(false);
			render_job->SetPayload(this);
			render_job->SetCallback(RenderCallback);
			render_worker.SubmitImmediateJob(render_job);
		}

		void PollingLoop()
		{
			NP_ENGINE_PROFILE_FUNCTION();
			NP_ENGINE_LOG_INFO("Polling Loop Start");

			thr::thread_duration_sleeper sleeper{ GetPlatformDefaultApplicationLoopDuration() };

			tim::steady_timestamp next = tim::steady_clock::now();
			tim::steady_timestamp prev = next;
			tim::milliseconds delta{};

			mem::sptr<evnt::Event> e = nullptr;
			con::vector<Layer*>::iterator it{};

			while (_keep_running.load(mo_acquire))
			{
				NP_ENGINE_PROFILE_SCOPE("poll loop");

				next = tim::steady_clock::now();
				delta = next - prev;
				prev = next;

				for (it = _layers.begin(); it != _layers.end(); it++)
					(*it)->BeforePoll();

				for (it = _layers.begin(); it != _layers.end(); it++)
					(*it)->Poll(delta);

				for (it = _layers.begin(); it != _layers.end(); it++)
					(*it)->AfterPoll();

				for (it = _layers.begin(); it != _layers.end(); it++)
					(*it)->CleanupPoll();

				sleeper.sleep(thr::sleep_type::std);
			}

			NP_ENGINE_LOG_INFO("Polling Loop End");
		}

	public:
		GameApp(mem::sptr<srvc::Services> services) :
			Application("My Game App", services),
			_game_layer(services, _window_layer),
			_keep_running(false)
		{
			PushLayer(_game_layer);
		}

		virtual ~GameApp() = default;

		void Run(i32 argc, chr** argv) override
		{
			NP_ENGINE_PROFILE_FUNCTION();
			NP_ENGINE_LOG_INFO("Hello world from my game app! My title is '" + GetTitle() + "'");
			CustomizeJobSystem();

			_keep_running.store(true, mo_release);

			jsys::JobSystem& job_system = _services->GetJobSystem();
			job_system.Start();
			PollingLoop();
			job_system.Stop();
			job_system.Clear();
		}

		virtual void StopRunning() override
		{
			_keep_running.store(false, mo_release);
		}

		virtual bl IsRunning() const override
		{
			return _keep_running.load(mo_acquire);
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_TESTER_HPP */