set(NP_ENGINE_TESTER "${PROJECT_NAME}-Tester")

add_executable(${NP_ENGINE_TESTER} NP-Engine-Tester.cpp)

if (MSVC)
	target_compile_definitions(
		${NP_ENGINE_TESTER} PRIVATE
		_USE_MATH_DEFINES=1
	)
endif()

target_link_libraries(${NP_ENGINE_TESTER} PUBLIC ${PROJECT_NAME})

set_target_properties(
	${NP_ENGINE_TESTER} PROPERTIES
	CXX_STANDARD 17
	CXX_EXTENSIONS false
	CXX_STANDARD_REQUIRED true
)