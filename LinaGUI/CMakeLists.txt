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
project(LinaGUI CXX)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------
set (LINAGUI_SOURCES
	
	# Core
	src/Core/GUINode.cpp

	# Primitives
	src/Primitives/Rectangle.cpp
	src/Primitives/Circle.cpp
	src/Primitives/Ngon.cpp
	src/Primitives/Convex.cpp
	src/Primitives/SimpleText.cpp
	src/Primitives/SDFText.cpp

	# Platform
	include/Platform/LinaVGIncl.hpp
	
	# Layout
	src/Layout/HorizontalLayout.cpp
)

#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(LINAGUI_HEADERS

	# Core
	include/Core/GUINode.hpp

	# Primitives
	include/Primitives/Rectangle.hpp
	include/Primitives/Circle.hpp
	include/Primitives/Ngon.hpp
	include/Primitives/Convex.hpp
	include/Primitives/SimpleText.hpp
	include/Primitives/SDFText.hpp

	# Layout
	include/Layout/HorizontalLayout.hpp
)

#--------------------------------------------------------------------
# Define the library & create an alias
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${LINAGUI_SOURCES} ${LINAGUI_HEADERS})
add_library(Lina::GUI ALIAS ${PROJECT_NAME}) 
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ${UNITY_BUILD_VALUE} UNITY_BUILD_UNIQUE_ID "LinaGUI")
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

target_link_libraries(${PROJECT_NAME} 
PRIVATE Lina::Common)

include(../_Dependencies/freetype/link.cmake)
include(../_Dependencies/linavg/link.cmake)
link_freetype()
link_linavg()


#--------------------------------------------------------------------
# Packages
#--------------------------------------------------------------------


#--------------------------------------------------------------------
# Generate export header for this dll project 
#--------------------------------------------------------------------
 #include (GenerateExportHeader)
#	GENERATE_EXPORT_HEADER (LinaGUI
#	BASE_NAME LinaGUI
#	EXPORT_MACRO_NAME LINA_API
#	EXPORT_FILE_NAME InputEngine.hpp
#)

#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	foreach(source IN LISTS LINAGUI_HEADERS LINAGUI_SOURCES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${LinaGUI_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
endif()


