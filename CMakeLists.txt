cmake_minimum_required(VERSION "3.14.1")

project(Gogaman LANGUAGES CXX)

add_library(Gogaman src/"Application.cpp")

target_include_directories(Gogaman PUBLIC $<INSTALL_INTERFACE:include>
