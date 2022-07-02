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
project(LinaGraphics)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------
set (LINAGRAPHICS_SOURCES

	src/Core/RenderEngine.cpp
	src/ECS/Systems/SkySystem.cpp
	src/ECS/Systems/SpriteSystem.cpp
	src/ECS/Systems/MeshSystem.cpp
	src/ECS/Systems/DecalSystem.cpp
	src/ECS/Systems/LightingSystem.cpp
	src/ECS/Systems/ParticleSystem.cpp
)

#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(LINAGRAPHICS_HEADERS

	include/Core/RenderEngine.hpp
	include/ECS/Components/SkyComponent.hpp
	include/ECS/Components/SpriteComponent.hpp
	include/ECS/Components/MeshComponent.hpp
	include/ECS/Components/DecalComponent.hpp
	include/ECS/Components/LightComponent.hpp
	include/ECS/Components/ParticleComponent.hpp
	include/ECS/Systems/SkySystem.hpp
	include/ECS/Systems/SpriteSystem.hpp
	include/ECS/Systems/MeshSystem.hpp
	include/ECS/Systems/DecalSystem.hpp
	include/ECS/Systems/LightingSystem.hpp
	include/ECS/Systems/ParticleSystem.hpp

	include/Utility/stb/stb_image.h
	include/Utility/stb/stb_image_write.h

)


#--------------------------------------------------------------------
# Define the library & create an alias
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${LINAGRAPHICS_SOURCES} ${LINAGRAPHICS_HEADERS})
add_library(Lina::Graphics ALIAS ${PROJECT_NAME}) 
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ${UNITY_BUILD_VALUE} UNITY_BUILD_UNIQUE_ID "LinaGraphics")
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
	
include(../_Dependencies/glad/link.cmake)
include(../_Dependencies/glfw/link.cmake)
include(../_Dependencies/assimp/link.cmake)
link_glad()
link_glfw()
link_assimp()

#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	foreach(source IN LISTS LINAGRAPHICS_HEADERS LINAGRAPHICS_SOURCES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${LinaGraphics_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
endif()


