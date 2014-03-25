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

# error on non-returning fn., should be default
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=return-type")

# opt flags
# maybe not necessary with cmake
set(${CMAKE_CXX_FLAGS_DEBUG} "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
set(${CMAKE_CXX_FLAGS_RELEASE} "${CMAKE_CXX_FLAGS_RELEASE} -O2")

# if blah
set(CFLAG_STD_ "--std=c++11")
# endif
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CFLAG_STD_}")

# main executable
add_executable(main
  main.cc

  controlctx.cc
  mgen.cc
  statement.cc
  expression.cc
  operation.cc
  
  entity.cc
  cptctx.cc
  data.cc
  component.cc
  
  log.cc

  # should be in sep. library
  util/demangle.cc
)
add_dependencies(main Moth)

# Clang supports extensions
if(${CMAKE_CXX_COMPILER} MATCHES "clang")
  set_property(TARGET main
    PROPERTY COMPILE_DEFINITIONS 
    USE_STDCPP14
  )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++1y-extensions")
endif()

# here we add definitions for any package if requred.
# add_definitions()

# -llibs
target_link_libraries(main
  pthread
)
