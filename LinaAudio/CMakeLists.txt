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
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)


#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------
set (LINAAUDIO_SOURCES

	#AUDIO
	src/Core/Backend/OpenAL/AudioEngineOpenAL.cpp

)

#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(LINAAUDIO_HEADERS
	
	#AUDIO
	include/Core/Backend/OpenAL/AudioEngineOpenAL.hpp
	include/Core/AudioBackend.hpp
	include/Core/AudioBackendFwd.hpp
	
	include/Components/ECAudioSource.hpp
	include/Components/ECAudioListener.hpp
	
	include/linaaudio_export.h
)


#--------------------------------------------------------------------
# Define the library & create an alias
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} SHARED ${LINAAUDIO_SOURCES} ${LINAAUDIO_HEADERS})
add_library(Lina::Audio ALIAS ${PROJECT_NAME}) 
include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})

#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/openal/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/alut/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaCommon/include)

#--------------------------------------------------------------------
# Subdirectories & linking
#--------------------------------------------------------------------

target_link_libraries(${PROJECT_NAME} 
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/openal/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/OpenAL32.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/alut/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/alut.lib
	PRIVATE Lina::Common
)
	

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


