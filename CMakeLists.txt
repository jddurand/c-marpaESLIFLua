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
# PROJECT_VERSION: see below. we will inherit marpaESLIF's PROJECT_VESION, and increment
# only PROJECT_VERSION_TWEAK
#
set(PROJECT_VERSION_TWEAK 1)
#
# Get library helper
#
include(FetchContent)
if("x$ENV{CMAKE_HELPERS_DEPEND_CMAKE_HELPERS_FILE}" STREQUAL "x")
  FetchContent_Declare(cmake-helpers GIT_REPOSITORY https://github.com/jddurand/cmake-helpers.git GIT_SHALLOW TRUE)
else()
  FetchContent_Declare(cmake-helpers URL $ENV{CMAKE_HELPERS_DEPEND_CMAKE_HELPERS_FILE})
endif()
FetchContent_MakeAvailable(cmake-helpers)
#
# The use of LUAXX option is STRONGLY DISCOURAGED when doing production package. This will create a marpaESLIFLua
# dependant on a local version of Lua, that may very well NOT fulfill your production system.
# Setting no option, will call the standard find_package(Lua REQUIRED) that should match production systems.
#
option(LUA52 "Compile explicitly against lua 5.2" OFF)
option(LUA53 "Compile explicitly against lua 5.3" OFF)
option(LUA54 "Compile explicitly against lua 5.4" OFF)
#
# LUA52, LUA53, LUA54 options are mutually exclusive
#
set(LUAXX 0)
if(LUA52)
  set(lua_package lua52)
  set(lua_full_version 5.2.4)
  set(lua_version 5.2)
  set(lua_tarball https://www.lua.org/ftp/lua-${lua_full_version}.tar.gz)
  set(lua_CMakeLists ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/tar/lua-5.2/CMakeLists.txt)
  math(EXPR LUAXX "${LUAXX} + 1")
endif()
if(LUA53)
  set(lua_package lua53)
  set(lua_full_version 5.3.6)
  set(lua_version 5.3)
  set(lua_tarball https://www.lua.org/ftp/lua-${lua_full_version}.tar.gz)
  set(lua_CMakeLists ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/tar/lua-5.3/CMakeLists.txt)
  math(EXPR LUAXX "${LUAXX} + 1")
endif()
if(LUA54)
  set(lua_package lua54)
  set(lua_full_version 5.4.6)
  set(lua_version 5.4)
  set(lua_tarball https://www.lua.org/ftp/lua-${lua_full_version}.tar.gz)
  set(lua_CMakeLists ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/tar/lua-5.4/CMakeLists.txt)
  math(EXPR LUAXX "${LUAXX} + 1")
endif()
if(LUAXX GREATER 1)
  message(FATAL_ERROR "LUA52, LUA53, LUA54 options are mutually exclusive")
elseif(LUAXX EQUAL 1)
  set(ENV{LUA_FULL_VERSION} ${lua_full_version})
  cmake_helpers_depend(${lua_package}
    FIND TRUE
    FIND_PACKAGE_NAME lua
    FIND_PACKAGE_ARGS ${lua_version} REQUIRED CONFIG NO_DEFAULT_PATH PATHS ${CMAKE_HELPERS_INSTALL_PATH}/${CMAKE_HELPERS_INSTALL_CMAKEDIR}
    EXTERNALPROJECT_ADD_ARGS
      URL ${lua_tarball}
      PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${lua_CMakeLists} CMakeLists.txt
  )
  #
  # We know exactly where is the executable
  #
  set(LUA_EXECUTABLE ${CMAKE_HELPERS_INSTALL_PATH}/${CMAKE_INSTALL_BINDIR}/lua${CMAKE_EXECUTABLE_SUFFIX_C})
else()
  find_package(Lua REQUIRED)
  find_program(LUA_EXECUTABLE NAMES lua${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR} lua)
  message(STATUS "")
  message(STATUS "Lua settings:")
  message(STATUS "-------------")
  message(STATUS "LUA_LIBRARIES     : ${LUA_LIBRARIES}")
  message(STATUS "LUA_INCLUDE_DIR   : ${LUA_INCLUDE_DIR}")
  message(STATUS "LUA_EXECUTABLE    : ${LUA_EXECUTABLE}")
  message(STATUS "LUA_VERSION_STRING: ${LUA_VERSION_STRING}")
  message(STATUS "LUA_VERSION_MAJOR : ${LUA_VERSION_MAJOR}")
  message(STATUS "LUA_VERSION_MINOR : ${LUA_VERSION_MINOR}")
  message(STATUS "LUA_VERSION_PATCH : ${LUA_VERSION_PATCH}")
  message(STATUS "")
endif()
#
# Dependencies
#
set(marpaESLIFLua_depends)
set(marpaESLIFLua_depends_ext)
set(marpaESLIFLua_find_dependencies)
if(LUAXX EQUAL 1)
  list(APPEND marpaESLIFLua_find_dependencies "Lua REQUIRED")
  list(APPEND marpaESLIFLua_depends PUBLIC lua::lua)
else()
  list(APPEND marpaESLIFLua_find_dependencies "Lua REQUIRED")
  list(APPEND marpaESLIFLua_depends PUBLIC ${LUA_LIBRARIES})
endif()
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
  EXTERNALPROJECT_ADD_ARGS GIT_REPOSITORY ${marpaESLIF_git} GIT_SHALLOW TRUE
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
  CONFIG_ARGS        include/marpaESLIFLua/internal/config.h.in include/marpaESLIFLua/internal/config.h
  FIND_DEPENDENCIES  ${marpaESLIFLua_find_dependencies}
  DEPENDS            ${marpaESLIFLua_depends}
  DEPENDS_EXT        ${marpaESLIFLua_depends_ext}
  TARGETS_OUTVAR     targets
)
#
# Get lua include directories when using standard build
#
if(NOT LUAXX EQUAL 1)
  foreach(target IN LISTS targets)
    target_include_directories(${target} PUBLIC ${LUA_INCLUDE_DIR})
  endforeach()
endif()
#
# Lua recommended specifics on the shared library: no prefix, no versioning
#
set_target_properties (marpaESLIFLua PROPERTIES PREFIX "" NO_SONAME TRUE)
#
# We declare tests if LUA_EXECUTABLE is set
#
if(LUA_EXECUTABLE)
  include(CTest)
  #
  # Tests. cmake_helpers_exe() has the logic to do path modifications. So we build
  # a fake binary and run test with a custom COMMAND
  #
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
    COMMAND ${CMAKE_COMMAND} -E env ${LUA_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/marpaESLIFLuaTest.lua
    ENVIRONMENTS
      LUA_PATH=unset:
      LUA_CPATH=path_list_prepend:$<TARGET_FILE:marpaESLIFLua>
  )
endif()
#
# Package
#
cmake_helpers_package()
