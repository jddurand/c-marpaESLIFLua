#
# Multi-config generator, e.g. Visual Studio on Windows:
#
# cmake -S c-marpaESLIFLua -B c-marpaESLIFLua-build
# cmake --build c-marpaESLIFLua-build --config RelWithDebInfo
# ctest --test-dir c-marpaESLIFLua-build -C RelWithDebInfo
# Windows:
#   cmake --install c-marpaESLIFLua-build --config RelWithDebInfo --prefix %cd%/c-marpaESLIFLua-install
# Others:
#   cmake --install c-marpaESLIFLua-build --config RelWithDebInfo --prefix `pwd`/c-marpaESLIFLua-install
# cmake --build c-marpaESLIFLua-build --config RelWithDebInfo --target package
#
# Single-config generator, e.g. NMake Makefiles on Windows, Unix Makefiles on Linxu:
#
# cmake -S c-marpaESLIFLua -B c-marpaESLIFLua-build -DCMAKE_BUILD_TYPE=RelWithDebInfo
# cmake --build c-marpaESLIFLua-build
# ctest --test-dir c-marpaESLIFLua-build
# Windows:
#   cmake --install c-marpaESLIFLua-build --prefix %cd%/c-marpaESLIFLua-install
# Others:
#   cmake --install c-marpaESLIFLua-build --prefix `pwd`/c-marpaESLIFLua-install
# cmake --build c-marpaESLIFLua-build --target package
#
# Local tests done with: cmake -S c-marpaESLIFLua -B c-marpaESLIFLua-build -DCMAKE_HELPERS_DEBUG=OFF -DICU_ROOT=C:\icu4c-74_2-Win64-MSVC2019
#
cmake_minimum_required(VERSION 3.26.0 FATAL_ERROR)
project(marpaESLIFLua LANGUAGES C) # We will set version later
#
# Lua dependency
#
find_package(Lua REQUIRED)
message(STATUS "")
message(STATUS "LUA Setup:")
message(STATUS "----------")
message(STATUS "LUA_LIBRARIES     : ${LUA_LIBRARIES}")
message(STATUS "LUA_INCLUDE_DIR   : ${LUA_INCLUDE_DIR}")
message(STATUS "LUA_VERSION_STRING: ${LUA_VERSION_STRING}")
message(STATUS "LUA_VERSION_MAJOR : ${LUA_VERSION_MAJOR}")
message(STATUS "LUA_VERSION_MINOR : ${LUA_VERSION_MINOR}")
message(STATUS "LUA_VERSION_PATCH : ${LUA_VERSION_PATCH}")
message(STATUS "")
#
# Get library helper
#
include(FetchContent)
FetchContent_Declare(cmake-helpers GIT_REPOSITORY https://github.com/jddurand/cmake-helpers.git GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(cmake-helpers)
#
# Dependencies
#
set(marpaESLIFLua_depends)
set(marpaESLIFLua_depends_ext)
set(marpaESLIFLua_find_dependencies)
#
# genericLogger dependency
#
set(genericLogger_git https://github.com/jddurand/c-genericLogger.git)
cmake_helpers_depend(genericLogger
  EXTERNALPROJECT_ADD_ARGS GIT_REPOSITORY ${genericLogger_git} GIT_SHALLOW TRUE
  FIND_PACKAGE_ARGS REQUIRED CONFIG
)
list(APPEND marpaESLIFLua_find_dependencies "genericLogger REQUIRED CONFIG")
list(APPEND marpaESLIFLua_depends PUBLIC genericLogger::genericLogger)
#
# genericStack dependency
#
set(genericStack_git https://github.com/jddurand/c-genericStack.git)
cmake_helpers_depend(genericStack
  EXTERNALPROJECT_ADD_ARGS GIT_REPOSITORY ${genericStack_git} GIT_SHALLOW TRUE
  FIND_PACKAGE_ARGS REQUIRED CONFIG
)
list(APPEND marpaESLIFLua_find_dependencies "genericStack REQUIRED CONFIG")
list(APPEND marpaESLIFLua_depends PUBLIC genericStack::genericStack)
#
# marpaESLIF dependency (DEV: How to get it to work with a branch ? Does not for me if I use origin/<branch name>
#
set(marpaESLIF_git https://github.com/jddurand/c-marpaESLIF.git)
cmake_helpers_depend(marpaESLIF
  EXTERNALPROJECT_ADD_ARGS DOWNLOAD_COMMAND ${GIT_EXECUTABLE} clone --single-branch --branch cmake_helpers ${marpaESLIF_git} ${CMAKE_CURRENT_BINARY_DIR}/_deps/marpaeslif-src
  FIND_PACKAGE_ARGS REQUIRED CONFIG
)
list(APPEND marpaESLIFLua_find_dependencies "marpaESLIF REQUIRED CONFIG")
list(APPEND marpaESLIFLua_depends PUBLIC marpaESLIF::marpaESLIF)
#
# We inherit marpaESLIF's version
#
find_package(marpaESLIF REQUIRED CONFIG)
message(STATUS "Using marpaESLIF's version: ${marpaESLIF_VERSION}")
set(PROJECT_VERSION ${marpaESLIF_VERSION})
set(PROJECT_VERSION_MAJOR ${marpaESLIF_VERSION_MAJOR})
set(PROJECT_VERSION_MINOR ${marpaESLIF_VERSION_MINOR})
set(PROJECT_VERSION_PATCH ${marpaESLIF_VERSION_PATCH})
#
# Create library
#
cmake_helpers_library(marpaESLIFLua
  FIND_DEPENDENCIES  ${marpaESLIFLua_find_dependencies}
  DEPENDS            ${marpaESLIFLua_depends}
  DEPENDS_EXT        ${marpaESLIFLua_depends_ext}
  TARGETS_OUTVAR     targets
)
#
# Lua recommended specifics on the shared library: no prefix, no versioning
#
set_target_properties (marpaESLIFLua PROPERTIES PREFIX "" NO_SONAME TRUE)
#
# Tests. cmake_helpers_exe() has the logic to do path modifications. So we build
# a fake binary and run test with a custom COMMAND
#
include(CTest)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/marpaESLIFLuaTesterDummy.c [[
#include <stdlib.h>

int main(int argc, char **argv) {
  exit(0);
}
]])
cmake_helpers_exe(marpaESLIFLuaTester
  INSTALL FALSE
  TEST TRUE
  SOURCES marpaESLIFLuaTesterDummy.c
  COMMAND ${CMAKE_COMMAND} -E env ${LUA_EXECUTABLE} test/marpaESLIFLuaTest.lua
)
#
# Package
#
cmake_helpers_package()
