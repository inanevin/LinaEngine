#linaenginebuildlauncherentrypointv100

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

cmake_minimum_required (VERSION 3.6)
project (Lina)
set(CMAKE_CXX_STANDARD 20)

#--------------------------------------------------------------------
# Download dependencies
#--------------------------------------------------------------------

include(CMake/GitUtils.cmake)
git_clone(
		PROJECT_NAME    _Dependencies
		GIT_URL         https://github.com/inanevin/LinaDependencies.git
		GIT_BRANCH      master
		DIRECTORY 	    ${CMAKE_SOURCE_DIR})

		
if(WIN32)
	set(PLATFORM "Win64")
endif()
		
if(UNIX)
	set(PLATFORM "Unix")
endif()

#--------------------------------------------------------------------
# Options & Definitions
#--------------------------------------------------------------------
#set(CMAKE_C_COMPILER /c/MinGW/bin/gcc.exe)
#set(CMAKE_CXX_COMPILER /c/MinGW/bin/g++.exe)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_SUPPRESS_REGENERATION true)

option(LINA_ENABLE_LOGGING "Enables console logging" ON)
option(LINA_ENABLE_PROFILING "Enables profiling" ON)
option(LINA_PRODUCTION_BUILD "Enable distribution ready build." OFF)
option(LINA_UNITY_BUILDS "Faster build times with unity builds." ON)

if(LINA_ENABLE_LOGGING)
	add_compile_definitions(LINA_ENABLE_LOGGING)
endif()

if(LINA_ENABLE_PROFILING)
	add_compile_definitions(LINA_ENABLE_PROFILING)
endif()

if(LINA_PRODUCTION_BUILD)
	add_compile_definitions(LINA_PRODUCTION_BUILD)
endif()

if(LINA_UNITY_BUILDS)
	set(UNITY_BUILD_VALUE ON)
else()
	set(UNITY_BUILD_VALUE OFF)
endif()

set(TARGET_ARCHITECTURE "x64")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")

#--------------------------------------------------------------------
# Set source & header dirs
#--------------------------------------------------------------------
set(LINAENGINE_SOURCE_DIR LinaEngine/src)
set(LINAENGINE_HEADER_DIR LinaEngine/include)
set(LINAENGINE_VENDOR_DIR LinaEngine/vendor)
set(LINASANDBOX_SOURCE_DIR Sandbox/src)
set(LINASANDBOX_HEADER_DIR Sandbox/include)

#--------------------------------------------------------------------
# Sub directories
#--------------------------------------------------------------------

add_subdirectory(LinaAudio)
add_subdirectory(LinaCommon)
add_subdirectory(LinaGraphics)
add_subdirectory(LinaInput)
add_subdirectory(LinaPhysics)
add_subdirectory(LinaEngine)
add_subdirectory(LinaEditor)
add_subdirectory(LinaResource)
add_subdirectory(Sandbox)


set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Sandbox)

