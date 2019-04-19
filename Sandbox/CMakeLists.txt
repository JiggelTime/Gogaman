cmake_minimum_required(VERSION 3.14.1)

project(Gogaman C CXX)

set(source_root ${CMAKE_CURRENT_SOURCE_DIR})
set(Gogaman_root ${source_root}/Gogaman)
set(Dependencies_root ${source_root}/Dependencies)
set(CMake_include $(source_root}/cmake)

include(${CMake_include}/listChildren.cmake)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

ListDirectories(Gogaman_subdirectories ${Gogaman_root})

include_directories(${source_root})

include_directories(${Dependencies_root}/AntTweakBar/include)
include_directories(${Dependencies_root}/assimp/include)
include_directories(${Dependencies_root}/glm/include)
include_directories(${Dependencies_root}/glad/include)
include_directories(${Dependencies_root}/KHR/include)
include_directories(${Dependencies_root}/stb_image/include)
include_directories(${Dependencies_root}/GLFW/include)

add_subdirectory(Gogaman)
add_subdirectory(Sandbox)