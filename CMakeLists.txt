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

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} --std=c++11 -O0 -g")
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
)

# -llibs
target_link_libraries(Main
  pthread
  )
