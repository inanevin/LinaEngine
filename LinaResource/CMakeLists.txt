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
project(LinaResource)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------
set (LINARESOURCE_SOURCES
	
	src/Core/ResourceManager.cpp
	src/Core/Resource.cpp
	src/Core/ResourceCommon.cpp
	src/Core/ResourceLoader.cpp
	src/Core/ResourceCache.cpp
	src/Core/ResourcePackager.cpp
	src/Utility/ResourceUtility.cpp
)

#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(LINARESOURCE_HEADERS

	include/Core/ResourceCommon.hpp
	include/Core/ResourceManager.hpp
	include/Core/Resource.hpp
	include/Core/ResourceLoader.hpp
	include/Core/ResourceHandle.hpp
	include/Core/ResourceCache.hpp
	include/Core/ResourcePackager.hpp
	include/Utility/ResourceUtility.hpp
)


#--------------------------------------------------------------------
# Define the library & create an alias
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${LINARESOURCE_SOURCES} ${LINARESOURCE_HEADERS})
add_library(Lina::Resource ALIAS ${PROJECT_NAME}) 
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ${UNITY_BUILD_VALUE} UNITY_BUILD_UNIQUE_ID "LinaResource")
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD_MODE BATCH UNITY_BUILD_BATCH_SIZE 16)

#--------------------------------------------------------------------
# Config & Options & Compile Definitions
#--------------------------------------------------------------------
include(../CMake/ProjectSettings.cmake)

#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaCommon/include)

#--------------------------------------------------------------------
# Subdirectories & linking
#--------------------------------------------------------------------

target_link_libraries(${PROJECT_NAME} PUBLIC user32.lib PRIVATE Lina::Common)

include(../_Dependencies/bit7z/link.cmake)
link_bit7z()

#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	foreach(source IN LISTS LINARESOURCE_HEADERS LINARESOURCE_SOURCES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${LinaResource_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
endif()


