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
project(LinaEditor CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)


#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------
set (LINAEDITOR_SOURCES
	

	#Input
	src/Core/EditorLayer.cpp
	
)

#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(LINAINPUT_HEADERS

	# include/linaeditor_export.h
	#Input
	include/Core/EditorLayer.hpp

)


#--------------------------------------------------------------------
# Define the library & create an alias
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} SHARED ${LINAEDITOR_SOURCES} ${LINAEDITOR_HEADERS})
add_library(Lina::Editor ALIAS ${PROJECT_NAME}) 
include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})

#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------


target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaEngine/include)

#--------------------------------------------------------------------
# Subdirectories & linking
#--------------------------------------------------------------------


target_link_libraries(${PROJECT_NAME} 

	PRIVATE Lina::Engine
)


#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	foreach(source IN LISTS LINAEDITOR_HEADERS LINAEDITOR_SOURCES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${LinaInput_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
endif()


