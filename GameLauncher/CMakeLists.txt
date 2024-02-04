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
project(GameLauncher)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------

set(SOURCES 
)

set(HEADERS
src/Main.cpp
)

#--------------------------------------------------------------------
# Create executable project
#--------------------------------------------------------------------

## WIN32 Console Subsystem
if(WIN32)
	add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})
else()
	add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
endif()

#--------------------------------------------------------------------
# Config & Options & Compile Definitions
#--------------------------------------------------------------------
include(../CMake/ProjectSettings.cmake)
include(../CMake/ExecutableSettings.cmake)

#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaCore/include)
target_compile_definitions(${PROJECT_NAME} PUBLIC _ITERATOR_DEBUG_LEVEL=0)
#--------------------------------------------------------------------
# Links
#--------------------------------------------------------------------
target_link_libraries(${PROJECT_NAME} 
PUBLIC Lina::Core
)


if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  target_compile_options(${PROJECT_NAME} PRIVATE "/ZI")
endif()

include(../CMake/EditorSandboxCommands.cmake)

#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	foreach(source IN LISTS LINAGAMELAUNCHER_HEADERS LINAGAMELAUNCHER_SOURCES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${LINAGAMELAUNCHER_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
endif()