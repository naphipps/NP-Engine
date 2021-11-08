##==--------------------------------------------------------==##
#NP_ENGINE Header Files
##==--------------------------------------------------------==##

set(NP_ENGINE_APPLICATION_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Application/Application.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Application/ApplicationEvent.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Application/ApplicationImpl.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Application/Layer.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Application/LayerContainer.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Application/Window.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Application/WindowCloseEvent.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Application/WindowEvents.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Application/WindowResizeEvent.hpp
)

set(NP_ENGINE_CONCURRENCY_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Concurrency/Concurrency.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Concurrency/ConcurrencyConstants.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Concurrency/MpmcQueue.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Concurrency/PadObject.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Concurrency/Thread.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Concurrency/ThreadPool.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Concurrency/ThreadToken.hpp
)

set(NP_ENGINE_CONTAINER_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Container/array.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Container/Container.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Container/container_traits.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Container/deque.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Container/deque_iterator.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Container/fixed_queue.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Container/init_list.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Container/iterator.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Container/oset.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Container/priority_queue.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Container/queue.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Container/uset.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Container/vector.hpp
)

set(NP_ENGINE_CORE_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Core/Color.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Core/Core.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Core/DmsImage.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Core/DmsLineSegment.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Core/DrawableImage.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Core/Image.hpp
)

set(NP_ENGINE_ECS_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/ECS/ECS.hpp
)

set(NP_ENGINE_EVENT_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Event/Event.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Event/EventImpl.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Event/EventQueue.hpp
)

set(NP_ENGINE_FILESYSTEM_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Filesystem/Filesystem.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Filesystem/filesystem_functions.hpp
)

set(NP_ENGINE_FOUNDATION_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Foundation/Constants.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Foundation/ConstantsSettings.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Foundation/Foundation.hpp
)

set(NP_ENGINE_INPUT_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Input/Input.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Input/KeyCodes.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Input/KeyEvent.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Input/MouseCodes.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Input/MouseEvent.hpp
)

set(NP_ENGINE_INSIGHT_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Insight/Assert.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Insight/Insight.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Insight/Instrumentor.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Insight/Log.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Insight/ScopedTimer.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Insight/Timer.hpp
)

set(NP_ENGINE_JOBSYSTEM_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/JobSystem/Job.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/JobSystem/JobFunction.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/JobSystem/JobPool.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/JobSystem/JobPriority.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/JobSystem/JobRecord.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/JobSystem/JobSystem.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/JobSystem/JobSystemImpl.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/JobSystem/JobToken.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/JobSystem/JobWorker.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/JobSystem/JobWorkerToken.hpp
)

set(NP_ENGINE_MATH_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Math/AABB.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Math/BresenhamLine.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Math/ClipLine.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Math/cmath_include.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Math/ease_functions.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Math/exp_functions.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Math/Math.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Math/math_functions.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Math/MidpointCircle.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Math/owen.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Math/pnpoly.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Math/Point.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Math/PointMath.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Math/Polygon.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Math/PolygonUtility.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Math/statistic_functions.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Math/trig_functions.hpp
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
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/SizedAllocator.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/sptr.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/TraitAllocator.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Memory/uptr.hpp
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
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Platform/PlatformConstants.hpp
)

set(NP_ENGINE_PLATFORM_APPLE_APPLICATION_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Platform/Apple/Application/AppleWindow.hpp
)

set(NP_ENGINE_PLATFORM_LINUX_APPLICATION_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Platform/Linux/Application/LinuxWindow.hpp
)

set(NP_ENGINE_PLATFORM_WINDOWS_APPLICATION_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Platform/Windows/Application/WindowsWindow.hpp
)

set(NP_ENGINE_PRIMITIVE_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Primitive/atomic_types.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Primitive/Primitive.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Primitive/primitive_types.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Primitive/primitive_values.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Primitive/stream_types.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Primitive/sync_types.hpp
)

set(NP_ENGINE_RANDOM_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Random/PcgRandutils.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Random/Random.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Random/RandomBase.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Random/RandomDistribution.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Random/RandomEngine.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Random/RandomSeed.hpp
)

set(NP_ENGINE_RENDERER_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Renderer/Renderer.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Renderer/RendererImpl.hpp
)

set(NP_ENGINE_SERIALIZATION_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Serialization/json.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Serialization/Serializable.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Serialization/Serialization.hpp
)

set(NP_ENGINE_SYSTEM_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/System/System.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/System/SystemImpl.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/System/Popup.hpp
)

set(NP_ENGINE_TIME_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Time/Time.hpp
)

set(NP_ENGINE_TYPETRAITS_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/TypeTraits/TypeTraits.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/TypeTraits/TypeTraitsImpl.hpp
)

set(NP_ENGINE_UTILITY_HPP
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Utility/Utility.hpp
	${PROJECT_SOURCE_DIR}/include/NP-Engine/Utility/UtilityImpl.hpp
)

set(NP_ENGINE_HEADER_FILES
	${NP_ENGINE_APPLICATION_HPP}
	${NP_ENGINE_CONCURRENCY_HPP}
	${NP_ENGINE_CONTAINER_HPP}
	${NP_ENGINE_CORE_HPP}
	${NP_ENGINE_ECS_HPP}
	${NP_ENGINE_EVENT_HPP}
	${NP_ENGINE_FILESYSTEM_HPP}
	${NP_ENGINE_FOUNDATION_HPP}
	${NP_ENGINE_INPUT_HPP}
	${NP_ENGINE_INSIGHT_HPP}
	${NP_ENGINE_JOBSYSTEM_HPP}
	${NP_ENGINE_MATH_HPP}
	${NP_ENGINE_MEMORY_HPP}
	${NP_ENGINE_NOISE_HPP}
	${NP_ENGINE_HPP}
	${NP_ENGINE_PHYSICS_HPP}
	${NP_ENGINE_PLATFORM_HPP}
	${NP_ENGINE_PLATFORM_APPLE_APPLICATION_HPP}
	${NP_ENGINE_PLATFORM_LINUX_APPLICATION_HPP}
	${NP_ENGINE_PLATFORM_WINDOWS_APPLICATION_HPP}
	${NP_ENGINE_PRIMITIVE_HPP}
	${NP_ENGINE_RANDOM_HPP}
	${NP_ENGINE_RENDERER_HPP}
	${NP_ENGINE_SERIALIZATION_HPP}
	${NP_ENGINE_SYSTEM_HPP}
	${NP_ENGINE_TIME_HPP}
	${NP_ENGINE_TYPETRAITS_HPP}
	${NP_ENGINE_UTILITY_HPP}
)

##==--------------------------------------------------------==##
#NP_ENGINE Source Files
##==--------------------------------------------------------==##

set(NP_ENGINE_APPLICATION_CPP
	Application/Window.cpp
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
	Memory/CAllocator.cpp
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

set(NP_ENGINE_PLATFORM_APPLE_APPLICATION_CPP
	Platform/Apple/Application/AppleWindow.cpp
)

set(NP_ENGINE_PLATFORM_LINUX_APPLICATION_CPP
	Platform/Linux/Application/LinuxWindow.cpp
)

set(NP_ENGINE_PLATFORM_WINDOWS_APPLICATION_CPP
	Platform/Windows/Application/WindowsWindow.cpp
)

set(NP_ENGINE_RENDERER_CPP
	Renderer/RendererImpl.cpp
)

set(NP_ENGINE_SYSTEM_CPP)

if (APPLE)
	set(NP_ENGINE_SYSTEM_CPP
		${NP_ENGINE_SYSTEM_CPP}
		System/Popup.mm
	)
else()
	set(NP_ENGINE_SYSTEM_CPP
		${NP_ENGINE_SYSTEM_CPP}
		System/Popup.cpp
	)
endif()

set(NP_ENGINE_SOURCE_FILES
	${NP_ENGINE_APPLICATION_CPP}
	${NP_ENGINE_INSIGHT_CPP}
	${NP_ENGINE_JOBSYSTEM_CPP}
	${NP_ENGINE_MATH_CPP}
	${NP_ENGINE_MEMORY_CPP}
	${NP_ENGINE_NP_ENGINE_MAIN_CPP}
	${NP_ENGINE_PHYSICS_CPP}
	${NP_ENGINE_PLATFORM_APPLE_APPLICATION_CPP}
	${NP_ENGINE_PLATFORM_LINUX_APPLICATION_CPP}
	${NP_ENGINE_PLATFORM_WINDOWS_APPLICATION_CPP}
	${NP_ENGINE_RENDERER_CPP}
	${NP_ENGINE_SYSTEM_CPP}
)

##==--------------------------------------------------------==##

add_library(${PROJECT_NAME} STATIC ${NP_ENGINE_SOURCE_FILES} ${NP_ENGINE_HEADER_FILES})

if (MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DDEBUG=1")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_USE_MATH_DEFINES=1")
else()
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG=1")
endif()

target_link_libraries(
	${PROJECT_NAME} PUBLIC
	spdlog::spdlog
	concurrentqueue
	Box2D
	pcg-cpp
	randutils
	nlohmann_json::nlohmann_json
	EnTT::EnTT
)

target_include_directories(
	${PROJECT_NAME} PUBLIC
	${PROJECT_SOURCE_DIR}/include
	${Box2D_SOURCE_DIR}
)

set_target_properties(
	${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 17
	CXX_EXTENSIONS false
	CXX_STANDARD_REQUIRED true
)

if (APPLE)
   find_library(COCOA_LIBRARY Cocoa)
   target_link_libraries(${PROJECT_NAME} PUBLIC ${COCOA_LIBRARY})
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   find_package(PkgConfig REQUIRED)
   pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
   target_link_libraries(${PROJECT_NAME} PUBLIC ${GTK3_LIBRARIES})
   target_include_directories(${PROJECT_NAME} PRIVATE ${GTK3_INCLUDE_DIRS})
endif()

##==--------------------------------------------------------==##

source_group("" FILES ${NP_ENGINE_HPP})
source_group(Application FILES ${NP_ENGINE_APPLICATION_HPP} ${NP_ENGINE_APPLICATION_CPP})
source_group(Concurrency FILES ${NP_ENGINE_CONCURRENCY_HPP})
source_group(Container FILES ${NP_ENGINE_CONTAINER_HPP})
source_group(Core FILES ${NP_ENGINE_CORE_HPP})
source_group(ECS FILES ${NP_ENGINE_ECS_HPP})
source_group(Event FILES ${NP_ENGINE_EVENT_HPP})
source_group(Filesystem FILES ${NP_ENGINE_FILESYSTEM_HPP})
source_group(Foundation FILES ${NP_ENGINE_FOUNDATION_HPP})
source_group(Input FILES ${NP_ENGINE_INPUT_HPP})
source_group(Insight FILES ${NP_ENGINE_INSIGHT_HPP} ${NP_ENGINE_INSIGHT_CPP})
source_group(JobSystem FILES ${NP_ENGINE_JOBSYSTEM_HPP} ${NP_ENGINE_JOBSYSTEM_CPP})
source_group(Math FILES ${NP_ENGINE_MATH_HPP} ${NP_ENGINE_MATH_CPP})
source_group(Memory FILES ${NP_ENGINE_MEMORY_HPP} ${NP_ENGINE_MEMORY_CPP})
source_group(Noise FILES ${NP_ENGINE_NOISE_HPP})
source_group(NP-Engine-main FILES ${NP_ENGINE_NP_ENGINE_MAIN_CPP})
source_group(Physics FILES ${NP_ENGINE_PHYSICS_HPP} ${NP_ENGINE_PHYSICS_CPP})
source_group(Platform FILES ${NP_ENGINE_PLATFORM_HPP})
source_group(Platform/Apple/Application FILES ${NP_ENGINE_PLATFORM_APPLE_APPLICATION_HPP} ${NP_ENGINE_PLATFORM_APPLE_APPLICATION_CPP})
source_group(Platform/Linux/Application FILES ${NP_ENGINE_PLATFORM_LINUX_APPLICATION_HPP} ${NP_ENGINE_PLATFORM_LINUX_APPLICATION_CPP})
source_group(Platform/Windows/Application FILES ${NP_ENGINE_PLATFORM_WINDOWS_APPLICATION_HPP} ${NP_ENGINE_PLATFORM_WINDOWS_APPLICATION_CPP})
source_group(Primitive FILES ${NP_ENGINE_PRIMITIVE_HPP})
source_group(Random FILES ${NP_ENGINE_RANDOM_HPP})
source_group(Renderer FILES ${NP_ENGINE_RENDERER_HPP} ${NP_ENGINE_RENDERER_CPP})
source_group(Serialization FILES ${NP_ENGINE_SERIALIZATION_HPP})
source_group(System FILES ${NP_ENGINE_SYSTEM_HPP} ${NP_ENGINE_SYSTEM_CPP})
source_group(Time FILES ${NP_ENGINE_TIME_HPP})
source_group(TypeTraits FILES ${NP_ENGINE_TYPETRAITS_HPP})
source_group(Utility FILES ${NP_ENGINE_UTILITY_HPP})