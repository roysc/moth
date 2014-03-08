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

# Clang supports extensions
if(${CMAKE_CXX_COMPILER} MATCHES "clang")
  set_property(TARGET main
    PROPERTY COMPILE_DEFINITIONS 
    USE_STDCPP14
  )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++1y-extensions")
endif()

# if blah
set(CFLAG_STD_ "--std=c++11")
# endif
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CFLAG_STD_}")

# error on non-returning fn., should be default
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=return-type")

set(Moth_BUILD_KIND "debug")
# or "release"?

# opt flags
if (${Moth_BUILD_KIND} MATCHES "debug")
  set(CFLAG_OPT_ "-O0 -g")
elseif(${Moth_BUILD_KIND} MATCHES "release")
  set(CFLAG_OPT_ "-O2")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CFLAG_OPT_}")

# main executable
add_executable(main
  main.cc
  log.cc
  entity.cc
  component.cc
  expression.cc
  controlctx.cc
  mgen.cc

  # should be in sep. library
  util/demangle.cc
)
add_dependencies(main Moth)

# here we add definitions for any package if requred.
# add_definitions()

# -llibs
target_link_libraries(main
  pthread
)
