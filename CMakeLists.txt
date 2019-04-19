cmake_minimum_required(VERSION 3.14.1)

project(Gogaman C CXX)

set(workspace_root ${CMAKE_CURRENT_SOURCE_DIR})
set(Gogaman_root ${workspace_root}/Gogaman)
set(Dependencies_root ${workspace_root}/Dependencies)

if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
	set(system_architecture "x64")
else() 
	set(system_architecture "x86")
endif()

set(output_directory ${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${system_architecture})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories(${workspace_root})

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