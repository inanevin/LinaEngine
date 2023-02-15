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

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;" CACHE STRING "" FORCE)

#--------------------------------------------------------------------
# Download dependencies
#--------------------------------------------------------------------

include(CMake/GitUtils.cmake)
git_clone(
		PROJECT_NAME    _Dependencies
		GIT_URL         https://github.com/inanevin/LinaDependencies.git
		GIT_BRANCH      master
		DIRECTORY 	    ${CMAKE_SOURCE_DIR})

#--------------------------------------------------------------------
# Options & Definitions
#--------------------------------------------------------------------

# Used by other CMake macros to determine library folders
if(WIN32)
	set(PLATFORM "Win64")
	
    set(LINA_GRAPHICS_D3D12 TRUE)
endif()
		
if(UNIX)
	set(PLATFORM "Unix")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_SUPPRESS_REGENERATION true)
set(LINA_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/_bin)

option(LINA_ENABLE_LOGGING "Enables logging." ON)
option(LINA_ENABLE_PROFILING "Enables profiling." ON)
option(LINA_PRODUCTION_BUILD "Strips all debugging functionality." OFF)

set(TARGET_ARCHITECTURE "x64")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")


#--------------------------------------------------------------------
# Sub directories
#--------------------------------------------------------------------

file(MAKE_DIRECTORY ${LINA_WORKING_DIRECTORY})

add_subdirectory(LinaCommon)
add_subdirectory(LinaCore)
add_subdirectory(LinaEditor)
add_subdirectory(GamePlugin)
add_subdirectory(GameLauncher)
# add_subdirectory(LinaHeaderTool)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CustomTargets")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT LinaEditor)

