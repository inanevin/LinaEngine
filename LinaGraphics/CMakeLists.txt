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
set(CMAKE_CXX_STANDARD 17)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------
set (LINAGRAPHICS_SOURCES

	#Rendering
	src/Rendering/ArrayBitmap.cpp
	src/Rendering/Texture.cpp
	src/Rendering/IndexedModel.cpp
	src/Rendering/ModelLoader.cpp
	src/Rendering/Material.cpp
	src/Rendering/RenderEngine.cpp
	src/Rendering/Mesh.cpp
	src/Rendering/RenderingCommon.cpp
	
	src/PackageManager/OpenGL/GLRenderDevice.cpp
	src/PackageManager/OpenGL/GLWindow.cpp
	
	src/ECS/Systems/MeshRendererSystem.cpp
	src/ECS/Systems/SpriteRendererSystem.cpp
	src/ECS/Systems/CameraSystem.cpp
	src/ECS/Systems/LightingSystem.cpp
)

#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(LINAGRAPHICS_HEADERS
	#Helpers
	include/Helpers/DrawParameterHelper.hpp
	
	#Rendering
	include/Rendering/ArrayBitmap.hpp
	include/Rendering/RenderEngine.hpp
	include/Rendering/Shader.hpp
	include/Rendering/Texture.hpp
	include/Rendering/Window.hpp
	include/Rendering/IndexedModel.hpp
	include/Rendering/ModelLoader.hpp
	include/Rendering/VertexArray.hpp
	include/Rendering/Material.hpp
	include/Rendering/Shader.hpp
	include/Rendering/Sampler.hpp
	include/Rendering/UniformBuffer.hpp
	include/Rendering/RenderTarget.hpp
	include/Rendering/Mesh.hpp
	include/Rendering/RenderingCommon.hpp
	include/Rendering/RenderConstants.hpp
	include/Rendering/RenderBuffer.hpp
	
	include/PackageManager/PAMRenderDevice.hpp	
	include/PackageManager/PAMWindow.hpp
	include/PackageManager/OpenGL/GLRenderDevice.hpp
	include/PackageManager/OpenGL/GLWindow.hpp
	
	
	include/ECS/Systems/CameraSystem.hpp
	include/ECS/Systems/MeshRendererSystem.hpp
	include/ECS/Systems/LightingSystem.hpp
	include/ECS/Systems/SpriteRendererSystem.hpp
	include/ECS/Components/MeshRendererComponent.hpp
	include/ECS/Components/CameraComponent.hpp
	include/ECS/Components/LightComponent.hpp
	include/ECS/Components/SpriteRendererComponent.hpp
	
	include/Utility/stb/stb.h
	include/Utility/stb/stb_c_lexer.h
	include/Utility/stb/stb_connected_components.h
	include/Utility/stb/stb_divide.h
	include/Utility/stb/stb_ds.h
	include/Utility/stb/stb_dxt.h
	include/Utility/stb/stb_easy_font.h
	include/Utility/stb/stb_herringbone_wang_tile.h
	include/Utility/stb/stb_image.h
	include/Utility/stb/stb_image_resize.h
	include/Utility/stb/stb_image_write.h
	include/Utility/stb/stb_leakcheck.h
	include/Utility/stb/stb_perlin.h
	include/Utility/stb/stb_rect_pack.h
	include/Utility/stb/stb_sprintf.h
	include/Utility/stb/stb_textedit.h
	include/Utility/stb/stb_tilemap_editor.h
	include/Utility/stb/stb_truetype.h
	include/Utility/stb/stb_vorbis.c
	include/Utility/stb/stb_voxel_render.h
	include/Utility/stb/stretchy_buffer.h
)


#--------------------------------------------------------------------
# Define the library & create an alias
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${LINAGRAPHICS_SOURCES} ${LINAGRAPHICS_HEADERS})
add_library(Lina::Graphics ALIAS ${PROJECT_NAME}) 

#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glfw/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glad/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/assimp/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaCommon/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaECS/include)


#--------------------------------------------------------------------
# Subdirectories & linking
#--------------------------------------------------------------------

target_link_libraries(${PROJECT_NAME} 
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glad/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/glad.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/assimp/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/${ASSIMP_LIB}
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glfw/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/glfw3.lib
	PRIVATE Lina::Common
	PRIVATE Lina::ECS
)
	

#--------------------------------------------------------------------
# Options & Packages
#--------------------------------------------------------------------
include(../CMake/GLGraphicsDefinitions.cmake)
find_package(OpenGL REQUIRED)


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


