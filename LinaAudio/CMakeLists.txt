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
project(LinaAudio)


#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------
set (LINAAUDIO_SOURCES

	# Core
	src/Core/AudioEngine.cpp

	# Audio
	src/Audio/Audio.cpp
	src/Audio/AudioAssetData.cpp

)

#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(LINAAUDIO_HEADERS
	
	# Core
	include/Core/AudioEngine.hpp
	include/Core/AudioEngineFwd.hpp
	
	# Audio
	include/Audio/Audio.hpp
	include/Audio/AudioAssetData.hpp
)


#--------------------------------------------------------------------
# Define the library & create an alias
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${LINAAUDIO_SOURCES} ${LINAAUDIO_HEADERS})
add_library(Lina::Audio ALIAS ${PROJECT_NAME}) 
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ${UNITY_BUILD_VALUE} UNITY_BUILD_UNIQUE_ID "LinaAudio")
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

target_link_libraries(${PROJECT_NAME} PRIVATE Lina::Common)
	
include(../_Dependencies/alut/link.cmake)
include(../_Dependencies/openal/link.cmake)
link_alut()
link_openal()

#--------------------------------------------------------------------
# Packages
#--------------------------------------------------------------------


#--------------------------------------------------------------------
# Generate export header for this dll project 
#--------------------------------------------------------------------
# include (GenerateExportHeader)
# GENERATE_EXPORT_HEADER (LinaEngine
#    BASE_NAME LinaEngine
#    EXPORT_MACRO_NAME LINA_API
#    EXPORT_FILE_NAME LinaCore.hpp
#)





#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	foreach(source IN LISTS LINAAUDIO_HEADERS LINAAUDIO_SOURCES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${LinaAUDIO_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
endif()


