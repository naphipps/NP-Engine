# //##===----------------------------------------------------------------------===##//
# //
# //  Author: Nathan Phipps 3/1/22
# //
# //##===----------------------------------------------------------------------===##//

#TODO: should this cmake start a new project? I think so
#TODO: I think we can put a lot of this into the test/src
#TODO: fix cmake setup so linux build can resume

set(NP_ENGINE_TESTER_HEADER_FILES
	${PROJECT_SOURCE_DIR}/test/include/NP-Engine-Tester.hpp
)

set(NP_ENGINE_TESTER_SOURCE_FILES
	src/NP-Engine-Tester.cpp
)

add_executable(${NP_ENGINE_TESTER} ${NP_ENGINE_TESTER_HEADER_FILES} ${NP_ENGINE_TESTER_SOURCE_FILES}) 

if (MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DDEBUG=1 /D_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS=1")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_USE_MATH_DEFINES=1 /D_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS=1")
else()
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG=1")
endif()

target_link_libraries(${NP_ENGINE_TESTER} PUBLIC ${PROJECT_NAME})

target_include_directories(
	${NP_ENGINE_TESTER} PUBLIC
	${PROJECT_SOURCE_DIR}/test/include
)

set_target_properties(
	${NP_ENGINE_TESTER} PROPERTIES
	CXX_STANDARD 17
	CXX_EXTENSIONS false
	CXX_STANDARD_REQUIRED true
)

#TODO: I do not like this duplicated code from repo/src/CMakeLists.txt, but this will work for now...
if (APPLE)
	file(TO_NATIVE_PATH "$ENV{HOME}/Library/NP-Engine" NP_ENGINE_WORKING_DIR)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	file(TO_NATIVE_PATH "$ENV{HOME}/.NP-Engine" NP_ENGINE_WORKING_DIR)
elseif(MSVC)
	file(TO_NATIVE_PATH "$ENV{ALLUSERSPROFILE}/NP-Engine" NP_ENGINE_WORKING_DIR)
endif()

if (EXISTS ${NP_ENGINE_WORKING_DIR})
#https://developer.blender.org/diffusion/B/browse/master/source/creator/CMakeLists.txt;9d5661c9e8b1ab383be4ae7b065f50d43310f6cb$175-211

	set(NP_ENGINE_TEST_ASSETS_FILES
		${PROJECT_SOURCE_DIR}/test/assets/statue-512x512.png
		${PROJECT_SOURCE_DIR}/test/assets/viking_room.obj
		${PROJECT_SOURCE_DIR}/test/assets/viking_room.png
	)

	file(TO_NATIVE_PATH "${NP_ENGINE_WORKING_DIR}/test/fake" NP_ENGINE_TEST_FAKE)
	file(TO_NATIVE_PATH "${NP_ENGINE_WORKING_DIR}/test/assets" NP_ENGINE_TEST_ASSETS_DST)

	if (EXISTS ${NP_ENGINE_TEST_FAKE})
		message(FATAL_ERROR "ERROR: \"${NP_ENGINE_TEST_FAKE}\" found, this should never be created, remove!")
	endif()

	add_custom_target("${PROJECT_NAME}-Copy-Test-Assets" ALL DEPENDS ${NP_ENGINE_TEST_FAKE})

	add_custom_command(OUTPUT ${NP_ENGINE_TEST_FAKE} # outputing the fake ensures we run our command(s)
		COMMAND ${CMAKE_COMMAND} -E make_directory ${NP_ENGINE_TEST_ASSETS_DST}
		COMMAND ${CMAKE_COMMAND} -E copy ${NP_ENGINE_TEST_ASSETS_FILES} ${NP_ENGINE_TEST_ASSETS_DST}
		COMMENT "Updating test assets in ${NP_ENGINE_WORKING_DIR}"
	)

	unset(NP_ENGINE_TEST_ASSETS_FILES)
	unset(NP_ENGINE_TEST_ASSETS_DST)
	unset(NP_ENGINE_TEST_FAKE)
endif()

source_group(include FILES ${NP_ENGINE_TESTER_HEADER_FILES})
source_group(src FILES ${NP_ENGINE_TESTER_SOURCE_FILES})