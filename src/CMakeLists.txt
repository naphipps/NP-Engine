set(
	NP_ENGINE_SOURCE_FILES
	Application/Window.cpp
	# Math/PolygonUtility.cpp
	# Math/Polygon.cpp
	Memory/CAllocator.cpp
	Renderer/RendererImpl.cpp
	NP-Engine-main/NP-Engine-main.cpp
	Physics/ParticleSystem.cpp
	Physics/Body.cpp
	Physics/ContactListener.cpp
	Physics/Physics.cpp
	JobSystem/JobWorker.cpp
	Insight/Instrumentor.cpp
	Insight/Log.cpp
	Platform/Apple/Application/AppleWindow.cpp
)

add_library(${PROJECT_NAME} STATIC ${NP_ENGINE_SOURCE_FILES})

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