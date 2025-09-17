cmake_minimum_required(VERSION 3.20)

project(cpp_router
  VERSION 0.2.0
  DESCRIPTION "Header-only C++ router"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_library(cpp_router INTERFACE)

target_include_directories(cpp_router INTERFACE 
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(examples example/basic.cpp)
target_link_libraries(examples PRIVATE cpp_router)