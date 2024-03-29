cmake_minimum_required(VERSION 3.26.0 FATAL_ERROR)
project(lua VERSION $ENV{LUA_FULL_VERSION} LANGUAGES C)
#
# Get library helper
#
include(FetchContent)
FetchContent_Declare(cmake-helpers GIT_REPOSITORY https://github.com/jddurand/cmake-helpers.git GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(cmake-helpers)
#
# OS tweaks
# Note that we do not use readline nor apply compability flag with previous major version
#
set(lua_depends)
#
# Math lib
#
if(CMAKE_MATH_LIBS)
  list(APPEND lua_depends PUBLIC ${CMAKE_MATH_LIBS})
endif()
#
# dl lib
# On Apple dlopen/dlcose are available without -ldl
#
if(NOT WIN32)
  if(CMAKE_DL_LIBS OR APPLE)
    add_definitions(-DLUA_USE_DLOPEN)
    if(CMAKE_DL_LIBS)
      list(APPEND lua_depends PUBLIC ${CMAKE_DL_LIBS})
    endif()
  endif()
endif()
#
# POSIX
#
if(LINUX OR APPLE)
  add_definitions(-DLUA_USE_POSIX)
endif()
#
# Long long
#
if(SIZEOF_LONG_LONG)
  add_definitions(-DLUA_USE_LONGLONG)
endif()
#
# Threads
#
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads)
if(THREADS_FOUND)
  if(CMAKE_THREAD_LIBS_INIT)
    list(APPEND lua_depends PUBLIC ${CMAKE_THREAD_LIBS_INIT})
  endif()
endif()
#
# Headers are in src
#
include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR} src)
#
# Library sources
#
set(lib_srcs
  src/lapi.c
  src/lcode.c
  src/lctype.c
  src/ldebug.c
  src/ldo.c
  src/ldump.c
  src/lfunc.c
  src/lgc.c
  src/llex.c
  src/lmem.c
  src/lobject.c
  src/lopcodes.c
  src/lparser.c
  src/lstate.c
  src/lstring.c
  src/ltable.c
  src/ltm.c
  src/lundump.c
  src/lvm.c
  src/lzio.c
  src/lauxlib.c
  src/lbaselib.c
  src/lbitlib.c
  src/lcorolib.c
  src/ldblib.c
  src/liolib.c
  src/lmathlib.c
  src/loslib.c
  src/lstrlib.c
  src/ltablib.c
  src/loadlib.c
  src/linit.c
)
set(lib_hdrs
  src/lauxlib.h
  src/lua.h
  src/luaconf.h
  src/lualib.h
)
#
# Library creation
#
cmake_helpers_library(lua
  SOURCES                        ${lib_srcs}
  HEADERS                        ${lib_hdrs}
  HEADERS_BASE_DIRS              src
  WITH_VISIBILITY_PRESET_HIDDEN  FALSE
  WITH_VISIBILITY_INLINES_HIDDEN FALSE
  ADD_VERSION_DEFINES            FALSE
  EXPORT_HEADER                  FALSE
  DEPENDS                        ${lua_depends}
  TARGETS_OUTVAR                 targets
)
#
# General compatibility mode
#
add_definitions(-DLUA_COMPAT_ALL)
#
# Specific to Win32
#
if(WIN32)
  foreach(target IN LISTS targets)
    target_compile_definitions(${target} PRIVATE -DLUA_BUILD_AS_DLL)
  endforeach()
endif()
#
# Executables: only lua is provided
#
cmake_helpers_exe(lua
  SOURCES                  src/lua.c
  DEPENDS                  ${lua_depends}
  EXE_USING_STATIC_LIBRARY FALSE
  INSTALL                  TRUE
  TEST                     FALSE
)
cmake_helpers_call(set_target_properties lua_shared_exe PROPERTIES OUTPUT_NAME lua)
