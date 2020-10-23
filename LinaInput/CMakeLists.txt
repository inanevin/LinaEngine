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
project(LinaInput CXX)
set(CMAKE_CXX_STANDARD 17)


#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------
set (LINAINPUT_SOURCES

	#PAM
	src/PackageManager/OpenGL/GLInputDevice.cpp
	
	#ECS
	src/ECS/Systems/FreeLookSystem.cpp
	
	#Input
	src/Input/InputEngine.cpp
	
	#Interfaces
	src/Interfaces/IInputSubscriber.cpp
)

#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(LINAINPUT_HEADERS

	#Input
	include/Input/InputAxisBinder.hpp
	include/Input/InputEngine.hpp
	include/Input/InputMappings.hpp
	include/Input/InputCommon.hpp
	include/Input/InputDevice.hpp
	
	#ECS
	include/ECS/Components/FreeLookComponent.hpp
	include/ECS/Systems/FreeLookSystem.hpp


	#Interfaces
	include/Interfaces/IInputSubscriber.hpp
	
	#Package Manager
	include/PackageManager/OpenGL/GLInputDevice.hpp
	include/PackageManager/PAMInputDevice.hpp
)


#--------------------------------------------------------------------
# Define the library & create an alias
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${LINAINPUT_SOURCES} ${LINAINPUT_HEADERS})
add_library(Lina::Input ALIAS ${PROJECT_NAME}) 

#--------------------------------------------------------------------
# Config & Options & Compile Definitions
#--------------------------------------------------------------------
include(../CMake/ProjectSettings.cmake)


#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------


target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glfw/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaCommon/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaAction/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaECS/include)

# Language standard
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_OPENGL=1)

#--------------------------------------------------------------------
# Subdirectories & linking
#--------------------------------------------------------------------


target_link_libraries(${PROJECT_NAME} 
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glfw/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/glfw3.lib
	PRIVATE Lina::Action
	PRIVATE Lina::Common
	PRIVATE Lina::ECS
)


#--------------------------------------------------------------------
# Packages
#--------------------------------------------------------------------


#--------------------------------------------------------------------
# Generate export header for this dll project 
#--------------------------------------------------------------------
 #include (GenerateExportHeader)
#	GENERATE_EXPORT_HEADER (LinaInput
#	BASE_NAME LinaInput
#	EXPORT_MACRO_NAME LINA_API
#	EXPORT_FILE_NAME InputEngine.hpp
#)

#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	foreach(source IN LISTS LINAINPUT_HEADERS LINAINPUT_SOURCES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${LinaInput_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
endif()


