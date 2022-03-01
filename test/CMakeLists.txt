# //##===----------------------------------------------------------------------===##//
# //
# //  Author: Nathan Phipps 3/1/22
# //
# //##===----------------------------------------------------------------------===##//

#TODO: should this cmake start a new project? I think so
#TODO: I think we can put a lot of this into the test/src

set(NP_ENGINE_TESTER_HEADER_FILES
	${PROJECT_SOURCE_DIR}/test/include/NP-Engine-Tester.hpp
)

set(NP_ENGINE_TESTER_SOURCE_FILES
	src/NP-Engine-Tester.cpp
)

add_executable(${NP_ENGINE_TESTER} ${NP_ENGINE_TESTER_HEADER_FILES} ${NP_ENGINE_TESTER_SOURCE_FILES}) 

if (MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DDEBUG=1")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_USE_MATH_DEFINES=1")
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

source_group(include FILES ${NP_ENGINE_TESTER_HEADER_FILES})
source_group(src FILES ${NP_ENGINE_TESTER_SOURCE_FILES})