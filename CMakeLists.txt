cmake_minimum_required(VERSION 3.21.4 FATAL_ERROR)
project(NP-Engine VERSION 0.0.0.1 LANGUAGES CXX C)

set(BOX2D_BUILD_EXAMPLES false CACHE BOOL "" FORCE)
set(BOX2D_BUILD_UNITTESTS false CACHE BOOL "" FORCE)

add_subdirectory(vendor/naphipps_concurrentqueue)
add_subdirectory(vendor/naphipps_liquidfun/liquidfun/Box2D)
add_subdirectory(vendor/naphipps_randutils)
add_subdirectory(vendor/naphipps_pcg-cpp)
add_subdirectory(vendor/nlohmann_json)
add_subdirectory(vendor/skypjack_entt)
add_subdirectory(vendor/gabime_spdlog)
add_subdirectory(src)

option(NP_ENGINE_BUILD_TESTER "Build the tester for NP-Engine" true)

if (NP_ENGINE_BUILD_TESTER)
	add_subdirectory(test)
endif()