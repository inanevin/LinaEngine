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
project(Sandbox)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------

set(SANDBOX_SOURCES 

src/Core/SandboxApplication.cpp
src/Levels/Example1Level.cpp
src/FPSDemo/FPSDemoLevel.cpp
)

set(SANDBOX_HEADERS

include/Core/test.hpp
include/Levels/Example1Level.hpp
include/FPSDemo/FPSDemoLevel.hpp
)

#--------------------------------------------------------------------
# Create executable project
#--------------------------------------------------------------------
add_executable(${PROJECT_NAME} ${SANDBOX_SOURCES} ${SANDBOX_HEADERS})
add_executable(Lina::Sandbox ALIAS ${PROJECT_NAME}) 

#--------------------------------------------------------------------
# Options & Definitions
#--------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaEditor/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/glfw/include)

include(../CMake/ProjectSettings.cmake)

#--------------------------------------------------------------------
# Links
#--------------------------------------------------------------------
target_link_libraries(${PROJECT_NAME} 
PRIVATE Lina::Engine
PRIVATE Lina::Editor
PRIVATE ${CMAKE_SOURCE_DIR}/vendor/glfw/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/glfw3.lib

)

# Copy engine resources to project solution directory to run within IDE.
add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/LinaEngine/resources" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/resources/")

# Copy engine resources to binary directory
add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/LinaEngine/resources" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/resources")

# Copy editor resources to project solution directory to run within IDE.
add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/LinaEditor/resources" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/resources/")

# Copy editor resources to binary directory
add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/LinaEditor/resources" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/resources")


# Copy project resources to binary directory
add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/resources" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/resources")

# Copy project resources to project solution directory to run within IDE.
add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/resources" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/resources/")

# Copy assimp dll to project binary directory
add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/vendor/assimp/bin/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/${ASSIMP_DLL}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>")


# Copy assimp dll to project solution directory to run within IDE.
add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/vendor/assimp/bin/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/${ASSIMP_DLL}" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}")

#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	foreach(source IN LISTS SANDBOX_HEADERS SANDBOX_SOURCES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${SANDBOX_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
endif()