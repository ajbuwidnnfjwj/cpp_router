cmake_minimum_required(VERSION 3.20)

project(cpp_router
  VERSION 0.1.0
  DESCRIPTION "Header-only C++ router"
  LANGUAGES CXX)

add_library(cpp_router INTERFACE)

target_include_directories(cpp_router
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

target_compile_features(cpp_router INTERFACE cxx_std_20)

include(GNUInstallDirs)
install(TARGETS cpp_router EXPORT cpp_routerTargets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

option(CPP_ROUTER_BUILD_EXAMPLES "Build examples" OFF)
if(CPP_ROUTER_BUILD_EXAMPLES)
  file(GLOB EXAMPLE_SOURCES CONFIGURE_DEPENDS examples/*.cpp)
  foreach(src IN LISTS EXAMPLE_SOURCES)
    get_filename_component(name "${src}" NAME_WE)
    add_executable(${name} "${src}")
    target_link_libraries(${name} PRIVATE cpp_router)
  endforeach()
endif()
