cmake_minimum_required(VERSION 3.14)

project(Gogaman LANGUAGES C CXX)

set(WORKSPACE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(GOGAMAN_ROOT ${WORKSPACE_ROOT}/Gogaman)
set(DEPENDENCIES_ROOT ${WORKSPACE_ROOT}/Dependencies)

if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
	set(SYSTEM_ARCHITECTURE "x64")
else() 
	set(SYSTEM_ARCHITECTURE "x86")
endif()

set(OUTPUT_DIRECTORY ${WORKSPACE_ROOT}/build/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${SYSTEM_ARCHITECTURE})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories(${WORKSPACE_ROOT})

#Preprocessor definitions
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
	add_compile_definitions(GM_DEBUG=1)
elseif(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
	add_compile_definitions(GM_RELEASE=1)
elseif(${CMAKE_BUILD_TYPE} MATCHES "Release")
	add_compile_definitions(GM_RELEASE=1)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_compile_definitions(GM_PLATFORM_WINDOWS=1)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	add_compile_definitions(GM_PLATFORM_LINUX=1)
else()
	message(FATAL_ERROR "Unsupported platform!")
endif()

add_subdirectory(Gogaman)
add_subdirectory(Sandbox)