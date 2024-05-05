
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Author: Inan Evin
# www.inanevin.com
# 
# Copyright (C) 2018 Inan Evin
# 
# Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
# and limitations under the License.
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------

cmake_minimum_required (VERSION 3.8)
project (Lina)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;DebugFAST;DebugASAN;" CACHE STRING "" FORCE)
set_property(GLOBAL PROPERTY DEBUG_CONFIGURATIONS "Debug;DebugFAST;DebugASAN")
set(LINA_GAME_PLUGIN_NAME "GamePlugin")
set(LINA_APP_NAME "LaunchGame")

#--------------------------------------------------------------------
# Options & Definitions
#--------------------------------------------------------------------

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_SUPPRESS_REGENERATION true)

set(TARGET_ARCHITECTURE "x64")

add_compile_definitions($<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=1>)
add_compile_definitions($<$<CONFIG:Debug>:LINA_DEBUG=1>)

add_compile_definitions($<$<CONFIG:DebugFAST>:_ITERATOR_DEBUG_LEVEL=0>)
add_compile_definitions($<$<CONFIG:DebugFAST>:LINA_DEBUG=1>)
add_compile_definitions($<$<CONFIG:DebugFAST>:_DEBUG=1>)
add_compile_definitions($<$<CONFIG:DebugFAST>:NDEBUG=0>)

add_compile_definitions($<$<CONFIG:DebugASAN>:_ITERATOR_DEBUG_LEVEL=2>)
add_compile_definitions($<$<CONFIG:DebugASAN>:LINA_DEBUG=1>)
add_compile_definitions($<$<CONFIG:DebugASAN>:_DEBUG=1>)
add_compile_definitions($<$<CONFIG:DebugASAN>:NDEBUG=0>)

if(MSVC)
  add_compile_options($<$<CONFIG:DebugFAST>:/O2>)
  add_compile_options($<$<CONFIG:DebugFAST>:/MDd>)
  add_compile_options($<$<CONFIG:DebugASAN>:/Od>)
  add_compile_options($<$<CONFIG:DebugASAN>:/Ob0>)
  add_compile_options($<$<CONFIG:DebugASAN>:/MDd>)
  add_compile_options($<$<CONFIG:DebugASAN>:/fsanitize=address>)
  add_compile_options($<$<CONFIG:DebugASAN>:/Zi>)
  add_link_options($<$<CONFIG:DebugASAN>:/DEBUG>)
else()
  add_compile_options($<$<CONFIG:DebugASAN>:-fsanitize=address>)
endif()



add_compile_definitions(LINA_APP_NAME="${LINA_APP_NAME}")
add_compile_definitions(LINA_VERSION_MAJOR=2)
add_compile_definitions(LINA_VERSION_MINOR=0)
add_compile_definitions(LINA_VERSION_PATCH=0)

#--------------------------------------------------------------------
# Platform
#--------------------------------------------------------------------

if (WIN32)
    add_compile_definitions(LINA_PLATFORM_WINDOWS=1)
endif()
if(APPLE)
    add_compile_definitions(LINA_PLATFORM_APPLE=1)
endif()
if(UNIX AND NOT APPLE)
    add_compile_definitions(LINA_PLATFORM_UNIX=1)
endif()
if(NOT WIN32 AND NOT APPLE AND NOT UNIX)
    add_compile_definitions(LINA_PLATFORM_UNKNOWN=1)
endif()


#--------------------------------------------------------------------
# Compiler
#--------------------------------------------------------------------
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_definitions(LINA_COMPILER_CLANG=1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_definitions(LINA_COMPILER_GNU=1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_definitions(LINA_COMPILER_MSVC=1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    add_compile_definitions(LINA_COMPILER_INTEL=1)
else()
    add_compile_definitions(LINA_COMPILER_OTHER=1)
endif()


#--------------------------------------------------------------------
# Sub directories
#--------------------------------------------------------------------

include(Dependencies/Dependencies.cmake)
add_subdirectory(LinaCommon)
add_subdirectory(LinaCore)
add_subdirectory(LinaEditor)
add_subdirectory(${LINA_GAME_PLUGIN_NAME})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CustomTargets")
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT LinaEditor)

