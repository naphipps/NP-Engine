# //##===----------------------------------------------------------------------===##//
# //
# //  Author: Nathan Phipps 11/1/21
# //
# //##===----------------------------------------------------------------------===##//

cmake_minimum_required(VERSION 3.21.4 FATAL_ERROR)
project(NP-Engine VERSION 0.0.0.0 LANGUAGES CXX C)

set(BOX2D_BUILD_EXAMPLES false CACHE BOOL "" FORCE)
set(BOX2D_BUILD_UNITTESTS false CACHE BOOL "" FORCE)
set(BOX2D_BUILD_UNITTESTS false CACHE BOOL "" FORCE)
set(USE_MSVC_RUNTIME_LIBRARY_DLL false CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS false CACHE BOOL "" FORCE)
set(GLFW_INSTALL false CACHE BOOL "" FORCE)
set(GLFW_LIBRARY_TYPE "STATIC" CACHE STRING "" FORCE)
set(RAPIDJSON_BUILD_DOC false CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_EXAMPLES false CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_TESTS false CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_CXX11 false CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_CXX17 true CACHE BOOL "" FORCE)
set(RAPIDJSON_ENABLE_INSTRUMENTATION_OPT false CACHE BOOL "" FORCE)

add_subdirectory(vendor/naphipps_liquidfun/liquidfun/Box2D)
add_subdirectory(vendor/naphipps_randutils)
add_subdirectory(vendor/naphipps_pcg-cpp)
add_subdirectory(vendor/skypjack_entt)
add_subdirectory(vendor/gabime_spdlog)
add_subdirectory(vendor/g-truc_glm)
add_subdirectory(vendor/glfw_glfw)
add_subdirectory(vendor/Tencent_rapidjson)
add_subdirectory(vendor/tinyobjloader_tinyobjloader)
add_subdirectory(vendor/naphipps_lodepng)
add_subdirectory(vendor/mariusbancila_stduuid)
add_subdirectory(src)

option(NP_ENGINE_BUILD_TESTER "Build the tester for NP-Engine" true)

if (NP_ENGINE_BUILD_TESTER)
	set(NP_ENGINE_TESTER "${PROJECT_NAME}-Tester")
	add_subdirectory(test)
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${NP_ENGINE_TESTER})
endif()