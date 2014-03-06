# -*- mode: cmake; -*-
# -*- coding: utf-8 -*-
# CMakeLists.txt
# main CMake script

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# "moth" yeah bitch
project(Moth)

# # include/
# include_directories()
# # lib/
# link_directories()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -O0 -g")

# Clang supports extensions
if(${CMAKE_CXX_COMPILER} MATCHES "clang")
  set_property(TARGET Main
    PROPERTY COMPILE_DEFINITIONS USE_STDCPP14)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++1y-extensions")
endif()
  
# here we add definitions for any package if requred.
# add_definitions()

# main executable
add_executable(Main
  main.cc
  entity.cc
  log.cc
  controlctx.cc
  mgen.cc
  component.cc
  util/demangle.cc
)

# -llibs
target_link_libraries(Main
  pthread
  )
