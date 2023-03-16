##===----------------------------------------------------------------------===##
#
# Author: Nathan Phipps 11/1/21
#
##===----------------------------------------------------------------------===##

##==--------------------------------------------------------==##
#NP_ENGINE Header Files
##==--------------------------------------------------------==##

set(NP_ENGINE_ALGORITHMS_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Algorithms/Algorithms.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Algorithms/BresenhamLine.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Algorithms/ClipLine.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Algorithms/MidpointCircle.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Algorithms/pnpoly.hpp
)

set(NP_ENGINE_APPLICATION_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Application/Application.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Application/ApplicationCloseEvent.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Application/Layer.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Application/WindowLayer.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Application/GraphicsLayer.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Application/Popup.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Application/ApplicationPopupEvent.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Application/ApplicationEvents.hpp
)

set(NP_ENGINE_CONTAINER_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Container/Container.hpp
)

set(NP_ENGINE_ECS_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/ECS/ECS.hpp
)

set(NP_ENGINE_EVENT_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Event/Event.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Event/EventImpl.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Event/EventQueue.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Event/EventHandler.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Event/EventSubmitter.hpp
)

set(NP_ENGINE_FILESYSTEM_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/FileSystem/FileSystem.hpp
)

set(NP_ENGINE_FOUNDATION_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Foundation/Constants.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Foundation/ConstantsSettings.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Foundation/Foundation.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Foundation/cmake_include.hpp.in
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Foundation/cmake_include.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Foundation/Assert.hpp
)

set(NP_ENGINE_GEOMETRY_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Geometry/Geometry.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Geometry/Shape.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Geometry/Line.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Geometry/Polygon.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Geometry/Circular.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Geometry/Aabb.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Geometry/Obb.hpp
)

set(NP_ENGINE_GRAPHICS_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Graphics.hpp
)

set(NP_ENGINE_GRAPHICS_INTERFACE_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Interface/Interface.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Interface/Renderer.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Interface/Camera.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Interface/Command.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Interface/Buffer.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Interface/Pipeline.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Interface/Scene.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Interface/Shader.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Interface/Texture.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Interface/GraphicsDetailType.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Interface/Vertex.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Interface/Color.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Interface/Image.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Interface/Model.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Interface/Frame.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Interface/Light.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Interface/RenderableObject.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Interface/RenderableLightObject.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Interface/RenderableImage.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Interface/RenderableModel.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Interface/RenderableLight.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Interface/RenderableType.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Interface/RenderableMetaValues.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Interface/PipelineMetaValues.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Interface/VisibleObject.hpp
)

set(NP_ENGINE_GRAPHICS_OPENGL_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/OpenGL/OpenGLRenderer.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/OpenGL/OpenGLGraphics.hpp
)

set(NP_ENGINE_GRAPHICS_VULKAN_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanGraphics.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanRenderer.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanRenderPass.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanDevice.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanInstance.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanSurface.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanPipeline.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanShader.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanSwapchain.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanVertex.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanScene.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanBuffer.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanImage.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanImageView.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanSampler.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanCommandPool.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanCommandBuffer.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanDescriptorSets.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanDescriptorSetLayout.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanTexture.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanFrame.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanFramebuffers.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanCamera.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanRenderableModel.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanRenderableImage.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanRenderableLight.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanCommand.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanCommands.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanCommandBeginRenderPass.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanCommandBindPipeline.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanCommandBindVertexBuffers.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanCommandBindIndexBuffer.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanCommandBindDescriptorSets.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanCommandDrawIndexed.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanCommandEndRenderPass.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanCommandCopyBuffers.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanCommandPipelineBarrier.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanCommandCopyBufferToImage.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanCommandPushConstants.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanQueue.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanFence.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics/Detail/Vulkan/VulkanSemaphore.hpp
)

set(NP_ENGINE_GRAPHICS_ALGORITHMS_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics-Algorithms/Graphics-Algorithms.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics-Algorithms/DmsImage.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics-Algorithms/DmsLineSegment.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics-Algorithms/DrawableImage.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics-Algorithms/ImageSubview.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics-Algorithms/Subview.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Graphics-Algorithms/FloodFillImage.hpp
)

set(NP_ENGINE_INPUT_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Input/Input.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Input/KeyCode.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Input/MouseCode.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Input/MousePosition.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Input/ControllerCode.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Input/InputQueue.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Input/InputState.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Input/InputSource.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Input/InputListener.hpp
)

set(NP_ENGINE_INSIGHT_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Insight/Insight.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Insight/Instrumentor.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Insight/InstrumentorTimer.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Insight/Log.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Insight/ScopedTimer.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Insight/Timer.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Insight/TraceEvent.hpp
)

set(NP_ENGINE_JOBSYSTEM_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/JobSystem/Job.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/JobSystem/JobPool.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/JobSystem/JobPriority.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/JobSystem/JobRecord.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/JobSystem/JobSystem.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/JobSystem/JobSystemImpl.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/JobSystem/JobWorker.hpp
)

set(NP_ENGINE_MATH_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Math/CMathInclude.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Math/EaseImpl.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Math/Math.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Math/MathImpl.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Math/owen.hpp
	# ${PROJECT_SOURCE_DIR}/include/NP-Engine/Math/Polygon.hpp
	# ${PROJECT_SOURCE_DIR}/include/NP-Engine/Math/PolygonUtility.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Math/StatisticImpl.hpp
)

set(NP_ENGINE_MEMORY_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/Allocator.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/Block.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/CAllocator.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/ExplicitListAllocator.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/ExplicitSegListAllocator.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/FallbackAllocator.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/ImplicitListAllocator.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/LinearAllocator.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/LockingPoolAllocator.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/Margin.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/Memory.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/MemoryFunctions.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/ObjectPool.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/PoolAllocator.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/RedBlackTreeAllocator.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/SegregatedAllocator.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/BlockedAllocator.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/StdAllocator.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/TraitAllocator.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/SmartPtr.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/PadObject.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/Delegate.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/AccumulatingAllocator.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/AccumulatingPool.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/BookkeepingAllocator.hpp
)

set(NP_ENGINE_NOISE_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Noise/Noise.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Noise/Perlin.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Noise/Simplex.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Noise/Turbulence.hpp
)

set(NP_ENGINE_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/NP-Engine.hpp
)

set(NP_ENGINE_PHYSICS_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Physics/Body.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Physics/ContactListener.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Physics/ParticleSystem.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Physics/Physics.hpp
)

set(NP_ENGINE_PLATFORM_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Platform/Platform.hpp
)

set(NP_ENGINE_PRIMITIVE_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Primitive/Primitive.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Primitive/PrimitiveTypes.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Primitive/PrimitiveValues.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Primitive/SyncTypes.hpp
)

set(NP_ENGINE_RANDOM_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Random/PcgRandutils.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Random/Random.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Random/RandomBase.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Random/SampleDistribution.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Random/RandomEngine.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Random/RandomSeed.hpp
)

set(NP_ENGINE_SERIALIZATION_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Serialization/Serializable.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Serialization/Serialization.hpp
)

set(NP_ENGINE_SERVICES_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Services/Services.hpp
)

set(NP_ENGINE_STRING_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/String/String.hpp
)

set(NP_ENGINE_SYSTEM_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/System/System.hpp
)

set(NP_ENGINE_THREAD_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Thread/Thread.hpp
)

set(NP_ENGINE_TIME_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Time/Time.hpp
)

set(NP_ENGINE_UID_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Uid/Uid.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Uid/UidImpl.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Uid/UidGenerator.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Uid/UidHandle.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Uid/UidSystem.hpp
)

set(NP_ENGINE_VENDOR_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Vendor/Box2dInclude.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Vendor/ConcurrentqueueInclude.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Vendor/EnttInclude.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Vendor/GlfwInclude.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Vendor/GlmInclude.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Vendor/PcgCppInclude.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Vendor/RandutilsInclude.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Vendor/SpdlogInclude.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Vendor/VulkanInclude.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Vendor/LodePngInclude.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Vendor/RapidJsonInclude.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Vendor/StduuidInclude.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Vendor/TinyObjLoaderInclude.hpp
)

set(NP_ENGINE_WINDOW_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Window/Window.hpp
)

set(NP_ENGINE_WINDOW_INTERFACE_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Window/Interface/WindowDetailType.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Window/Interface/WindowImpl.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Window/Interface/WindowClosedEvent.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Window/Interface/WindowClosingEvent.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Window/Interface/WindowEvents.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Window/Interface/WindowResizeEvent.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Window/Interface/WindowInterface.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Window/Interface/WindowPositionEvent.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Window/Interface/WindowFramebufferEvent.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Window/Interface/WindowMinimizeEvent.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Window/Interface/WindowMaximizeEvent.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Window/Interface/WindowFocusEvent.hpp
)

set(NP_ENGINE_WINDOW_GLFW_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Window/Detail/Glfw/GlfwWindow.hpp
)

set(NP_ENGINE_HEADER_FILES
	${NP_ENGINE_ALGORITHMS_HPP}
	${NP_ENGINE_APPLICATION_HPP}
	${NP_ENGINE_CONTAINER_HPP}
	${NP_ENGINE_ECS_HPP}
	${NP_ENGINE_EVENT_HPP}
	${NP_ENGINE_FILESYSTEM_HPP}
	${NP_ENGINE_FOUNDATION_HPP}
	${NP_ENGINE_GEOMETRY_HPP}
	${NP_ENGINE_GRAPHICS_HPP}
	${NP_ENGINE_GRAPHICS_INTERFACE_HPP}
	${NP_ENGINE_GRAPHICS_OPENGL_HPP}
	${NP_ENGINE_GRAPHICS_VULKAN_HPP}
	${NP_ENGINE_GRAPHICS_ALGORITHMS_HPP}
	${NP_ENGINE_INPUT_HPP}
	${NP_ENGINE_INSIGHT_HPP}
	${NP_ENGINE_JOBSYSTEM_HPP}
	${NP_ENGINE_MATH_HPP}
	${NP_ENGINE_MEMORY_HPP}
	${NP_ENGINE_NOISE_HPP}
	${NP_ENGINE_HPP}
	${NP_ENGINE_PHYSICS_HPP}
	${NP_ENGINE_PLATFORM_HPP}
	${NP_ENGINE_PRIMITIVE_HPP}
	${NP_ENGINE_RANDOM_HPP}
	${NP_ENGINE_SERIALIZATION_HPP}
	${NP_ENGINE_SERVICES_HPP}
	${NP_ENGINE_STRING_HPP}
	${NP_ENGINE_SYSTEM_HPP}
	${NP_ENGINE_THREAD_HPP}
	${NP_ENGINE_TIME_HPP}
	${NP_ENGINE_UID_HPP}
	${NP_ENGINE_VENDOR_HPP}
	${NP_ENGINE_WINDOW_HPP}
	${NP_ENGINE_WINDOW_INTERFACE_HPP}
	${NP_ENGINE_WINDOW_GLFW_HPP}
)

##==--------------------------------------------------------==##
#NP_ENGINE Source Files
##==--------------------------------------------------------==##

set(NP_ENGINE_APPPLICATION_CPP)

if (APPLE)
	set(NP_ENGINE_APPPLICATION_CPP
		${NP_ENGINE_APPPLICATION_CPP}
		Application/Popup.mm
	)
else()
	set(NP_ENGINE_APPPLICATION_CPP
		${NP_ENGINE_APPPLICATION_CPP}
		Application/Popup.cpp
	)
endif()

# TODO: we can remove these
set(NP_ENGINE_GRAPHICS_CPP
	#Graphics/Graphics.cpp
)

set(NP_ENGINE_GRAPHICS_INTERFACE_CPP
	Graphics/Interface/GraphicsDetailType.cpp
	#Graphics/Interface/Camera.cpp
	#Graphics/Interface/Command.cpp
	#Graphics/Interface/Buffer.cpp
	Graphics/Interface/RenderableImage.cpp
	Graphics/Interface/RenderableModel.cpp
	Graphics/Interface/RenderableLight.cpp
	#Graphics/Interface/Pipeline.cpp
	Graphics/Interface/Scene.cpp
	#Graphics/Interface/Shader.cpp
	#Graphics/Interface/Texture.cpp
	Graphics/Interface/Renderer.cpp
)

# TODO: we can remove these
set(NP_ENGINE_GRAPHICS_OPENGL_CPP
	#Graphics/Detail/OpenGL/OpenGLRenderer.cpp
)

# TODO: we can remove these
set(NP_ENGINE_GRAPHICS_VULKAN_CPP
	#Graphics/Detail/Vulkan/VulkanDevice.cpp
	#Graphics/Detail/Vulkan/VulkanRenderer.cpp
	#Graphics/Detail/Vulkan/VulkanPipeline.cpp
)

set(NP_ENGINE_GRAPHICS_VULKAN_SHADERS_GLSL
	${PROJECT_SOURCE_DIR}/src/Graphics/Detail/Vulkan/shaders/object_vertex.glsl
	${PROJECT_SOURCE_DIR}/src/Graphics/Detail/Vulkan/shaders/object_fragment.glsl
	${PROJECT_SOURCE_DIR}/src/Graphics/Detail/Vulkan/shaders/light_vertex.glsl
	${PROJECT_SOURCE_DIR}/src/Graphics/Detail/Vulkan/shaders/light_fragment.glsl
)

set(NP_ENGINE_INSIGHT_CPP
	Insight/Instrumentor.cpp
	Insight/Log.cpp
)

set(NP_ENGINE_JOBSYSTEM_CPP
	JobSystem/JobWorker.cpp
)

set(NP_ENGINE_MATH_CPP
	# Math/PolygonUtility.cpp
	# Math/Polygon.cpp
)

set(NP_ENGINE_MEMORY_CPP
	Memory/TraitAllocator.cpp
)

set(NP_ENGINE_NP_ENGINE_MAIN_CPP
	NP-Engine-main/NP-Engine-main.cpp
)

set(NP_ENGINE_PHYSICS_CPP
	Physics/ParticleSystem.cpp
	Physics/Body.cpp
	Physics/ContactListener.cpp
	Physics/Physics.cpp
)

set(NP_ENGINE_SYSTEM_CPP
	System/System.cpp
)

set(NP_ENGINE_THREAD_CPP
	Thread/Thread.cpp
)

set(NP_ENGINE_WINDOW_INTERFACE_CPP
	Window/Interface/WindowImpl.cpp
)

set(NP_ENGINE_SOURCE_FILES
	${NP_ENGINE_THREAD_CPP}
	${NP_ENGINE_INSIGHT_CPP}
	${NP_ENGINE_JOBSYSTEM_CPP}
	${NP_ENGINE_MATH_CPP}
	${NP_ENGINE_MEMORY_CPP}
	${NP_ENGINE_NP_ENGINE_MAIN_CPP}
	${NP_ENGINE_PHYSICS_CPP}
	${NP_ENGINE_WINDOW_INTERFACE_CPP}
	${NP_ENGINE_GRAPHICS_INTERFACE_CPP}
	${NP_ENGINE_GRAPHICS_OPENGL_CPP}
	${NP_ENGINE_GRAPHICS_VULKAN_CPP}
	${NP_ENGINE_GRAPHICS_VULKAN_SHADERS_GLSL}
	${NP_ENGINE_GRAPHICS_CPP}
	${NP_ENGINE_APPPLICATION_CPP}
	${NP_ENGINE_SYSTEM_CPP}
)

##==--------------------------------------------------------==##

add_library(${PROJECT_NAME} STATIC ${NP_ENGINE_SOURCE_FILES} ${NP_ENGINE_HEADER_FILES})

find_package(RapidJSON REQUIRED) # TODO: cleanup this file, it's getting a little messy

if (MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DDEBUG=1")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_USE_MATH_DEFINES=1")
else()
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG=1")
	set(CMAKE_BUILD_TYPE "Debug")
endif()

target_link_libraries(
	${PROJECT_NAME} PUBLIC
	spdlog::spdlog
	concurrentqueue
	Box2D
	pcg-cpp
	randutils
	EnTT::EnTT
	glm
	glfw
	${RapidJSON_LIBRARY}
	tinyobjloader
	lodepng
	stduuid
)

target_include_directories(
	${PROJECT_NAME} PUBLIC
	${PROJECT_SOURCE_DIR}/include
	${Box2D_SOURCE_DIR}
	${RapidJSON_INCLUDE_DIR}
	${stduuid_INCLUDE_DIR}
)

set_target_properties(
	${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 17
	CXX_EXTENSIONS false
	CXX_STANDARD_REQUIRED true
)

if (APPLE)
	file(TO_NATIVE_PATH "$ENV{HOME}/Library/NP-Engine" NP_ENGINE_WORKING_DIR)
	find_library(COCOA_LIBRARY Cocoa)
	target_link_libraries(${PROJECT_NAME} PUBLIC ${COCOA_LIBRARY})
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	file(TO_NATIVE_PATH "$ENV{HOME}/.NP-Engine" NP_ENGINE_WORKING_DIR)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
	if (NOT DEFINED GTK3_gtk+-3.0_VERSION)
		message(FATAL_ERROR "Could not find gtk+-3.0, so install libgtk-3-dev.\nExample: sudo apt install libgtk-3-dev")
	else()
	   target_link_libraries(${PROJECT_NAME} PUBLIC ${GTK3_LIBRARIES})
	   target_include_directories(${PROJECT_NAME} PUBLIC ${GTK3_INCLUDE_DIRS})
	endif()
elseif(MSVC)
	file(TO_NATIVE_PATH "$ENV{ALLUSERSPROFILE}/NP-Engine" NP_ENGINE_WORKING_DIR)
	target_link_libraries(${PROJECT_NAME} PUBLIC "dwmapi.lib")
endif()

find_package(Vulkan REQUIRED FATAL_ERROR)
if(NOT Vulkan_FOUND OR NOT DEFINED Vulkan_FOUND)
	message(FATAL_ERROR "Could not find Vulkan. Install the Vulkan SDK")
else()
	target_link_libraries(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARY})
	target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIR})
	file(TO_NATIVE_PATH ${Vulkan_GLSLC_EXECUTABLE} Vulkan_GLSLC_EXECUTABLE_NATIVE)
	set(Vulkan_GLSLC_EXECUTABLE ${Vulkan_GLSLC_EXECUTABLE_NATIVE})
	unset(Vulkan_GLSLC_EXECUTABLE_NATIVE)
endif()

if (NP_ENGINE_WORKING_DIR)
#https://developer.blender.org/diffusion/B/browse/master/source/creator/CMakeLists.txt;9d5661c9e8b1ab383be4ae7b065f50d43310f6cb$175-211

	file(TO_NATIVE_PATH "${NP_ENGINE_WORKING_DIR}/fake" NP_ENGINE_FAKE)
	file(TO_NATIVE_PATH "${NP_ENGINE_GRAPHICS_VULKAN_SHADERS_GLSL}" NP_ENGINE_GRAPHICS_VULKAN_SHADERS_GLSL)
	file(TO_NATIVE_PATH "${NP_ENGINE_WORKING_DIR}/Vulkan/shaders" NP_ENGINE_VULKAN_SHADERS_DIR)

	if (EXISTS ${NP_ENGINE_FAKE})
		message(FATAL_ERROR "ERROR: \"${NP_ENGINE_FAKE}\" found, this should never be created, remove!")
	endif()

	add_custom_target("${PROJECT_NAME}-Copy-Shaders" ALL DEPENDS ${NP_ENGINE_FAKE})

	add_custom_command(OUTPUT ${NP_ENGINE_FAKE} # outputing the fake ensures we run our command(s)
		COMMAND ${CMAKE_COMMAND} -E make_directory ${NP_ENGINE_VULKAN_SHADERS_DIR}
		COMMAND ${CMAKE_COMMAND} -E copy ${NP_ENGINE_GRAPHICS_VULKAN_SHADERS_GLSL} ${NP_ENGINE_VULKAN_SHADERS_DIR}
		COMMENT "Updating shaders in ${NP_ENGINE_WORKING_DIR}"
	)

	unset(NP_ENGINE_VULKAN_SHADERS_DIR)
	unset(NP_ENGINE_FAKE)
endif()

configure_file(
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Foundation/cmake_include.hpp.in
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Foundation/cmake_include.hpp
)

##==--------------------------------------------------------==##
#NP_ENGINE Source Groups
##==--------------------------------------------------------==##

source_group("" FILES ${NP_ENGINE_HPP})
source_group(Algorithms FILES ${NP_ENGINE_ALGORITHMS_HPP})
source_group(Application FILES ${NP_ENGINE_APPLICATION_HPP} ${NP_ENGINE_APPPLICATION_CPP})
source_group(Container FILES ${NP_ENGINE_CONTAINER_HPP})
source_group(ECS FILES ${NP_ENGINE_ECS_HPP})
source_group(Event FILES ${NP_ENGINE_EVENT_HPP})
source_group(FileSystem FILES ${NP_ENGINE_FILESYSTEM_HPP})
source_group(Foundation FILES ${NP_ENGINE_FOUNDATION_HPP})
source_group(Geometry FILES ${NP_ENGINE_GEOMETRY_HPP})
source_group(Graphics FILES ${NP_ENGINE_GRAPHICS_HPP} ${NP_ENGINE_GRAPHICS_CPP})
source_group(Graphics/Interface FILES ${NP_ENGINE_GRAPHICS_INTERFACE_HPP} ${NP_ENGINE_GRAPHICS_INTERFACE_CPP})
source_group(Graphics/Detail/OpenGL FILES ${NP_ENGINE_GRAPHICS_OPENGL_HPP} ${NP_ENGINE_GRAPHICS_OPENGL_CPP})
source_group(Graphics/Detail/Vulkan FILES ${NP_ENGINE_GRAPHICS_VULKAN_HPP} ${NP_ENGINE_GRAPHICS_VULKAN_CPP})
source_group(Graphics/Detail/Vulkan/shaders FILES ${NP_ENGINE_GRAPHICS_VULKAN_SHADERS_GLSL})
source_group(Graphics-Algorithms FILES ${NP_ENGINE_GRAPHICS_ALGORITHMS_HPP})
source_group(Input FILES ${NP_ENGINE_INPUT_HPP})
source_group(Insight FILES ${NP_ENGINE_INSIGHT_HPP} ${NP_ENGINE_INSIGHT_CPP})
source_group(JobSystem FILES ${NP_ENGINE_JOBSYSTEM_HPP} ${NP_ENGINE_JOBSYSTEM_CPP})
source_group(Math FILES ${NP_ENGINE_MATH_HPP} ${NP_ENGINE_MATH_CPP})
source_group(Memory FILES ${NP_ENGINE_MEMORY_HPP} ${NP_ENGINE_MEMORY_CPP})
source_group(Noise FILES ${NP_ENGINE_NOISE_HPP})
source_group(NP-Engine-main FILES ${NP_ENGINE_NP_ENGINE_MAIN_CPP})
source_group(Physics FILES ${NP_ENGINE_PHYSICS_HPP} ${NP_ENGINE_PHYSICS_CPP})
source_group(Platform FILES ${NP_ENGINE_PLATFORM_HPP})
source_group(Primitive FILES ${NP_ENGINE_PRIMITIVE_HPP})
source_group(Random FILES ${NP_ENGINE_RANDOM_HPP})
source_group(Serialization FILES ${NP_ENGINE_SERIALIZATION_HPP})
source_group(Services FILES ${NP_ENGINE_SERVICES_HPP})
source_group(String FILES ${NP_ENGINE_STRING_HPP})
source_group(System FILES ${NP_ENGINE_SYSTEM_HPP} ${NP_ENGINE_SYSTEM_CPP})
source_group(Thread FILES ${NP_ENGINE_THREAD_HPP} ${NP_ENGINE_THREAD_CPP})
source_group(Time FILES ${NP_ENGINE_TIME_HPP})
source_group(Uid FILES ${NP_ENGINE_UID_HPP})
source_group(Vendor FILES ${NP_ENGINE_VENDOR_HPP})
source_group(Window FILES ${NP_ENGINE_WINDOW_HPP})
source_group(Window/Interface FILES ${NP_ENGINE_WINDOW_INTERFACE_HPP} ${NP_ENGINE_WINDOW_INTERFACE_CPP})
source_group(Window/Detail/Glfw FILES ${NP_ENGINE_WINDOW_GLFW_HPP})