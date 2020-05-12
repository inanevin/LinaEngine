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
	src/Rendering/DDSTexture.cpp
	src/Rendering/Texture.cpp
	src/Rendering/IndexedModel.cpp
	src/Rendering/ModelLoader.cpp
	src/Rendering/RenderEngine.cpp
	src/Rendering/RenderContext.cpp
	src/Rendering/ShaderConstants.cpp
	src/Rendering/Mesh.cpp
	
	src/PackageManager/OpenGL/GLRenderDevice.cpp
	src/PackageManager/OpenGL/GLWindow.cpp
	
	src/ECS/Systems/MeshRendererSystem.cpp
	src/ECS/Systems/CameraSystem.cpp
	src/ECS/Systems/LightingSystem.cpp
)

#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(LINAGRAPHICS_HEADERS

	#Rendering
	include/Rendering/ArrayBitmap.hpp
	include/Rendering/DDSTexture.hpp
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
	include/Rendering/RenderContext.hpp
	include/Rendering/RenderTarget.hpp
	include/Rendering/Mesh.hpp
	include/Rendering/RenderingCommon.hpp
	include/Rendering/ShaderConstants.hpp
	
	include/PackageManager/PAMRenderDevice.hpp	
	include/PackageManager/PAMWindow.hpp
	include/PackageManager/OpenGL/GLRenderDevice.hpp
	include/PackageManager/OpenGL/GLWindow.hpp
	
	
	include/ECS/Systems/CameraSystem.hpp
	include/ECS/Systems/MeshRendererSystem.hpp
	include/ECS/Systems/LightingSystem.hpp
	include/ECS/Components/MeshRendererComponent.hpp
	include/ECS/Components/CameraComponent.hpp
	include/ECS/Components/LightComponent.hpp
	
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
add_library(${PROJECT_NAME} STATIC ${LINAGRAPHICS_SOURCES} ${LINAGRAPHICS_HEADERS})
add_library(Lina::Graphics ALIAS ${PROJECT_NAME}) 

#--------------------------------------------------------------------
# Config & Options & Compile Definitions
#--------------------------------------------------------------------

set_target_properties(
    ${PROJECT_NAME}
      PROPERTIES 
        CXX_STANDARD 17 
        CXX_STANDARD_REQUIRED YES 
        CXX_EXTENSIONS NO
)


if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
   target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_COMPILER_CLANG=1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
   target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_COMPILER_GNU=1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
   target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_COMPILER_MSVC=1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Intel")
   target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_COMPILER_INTEL=1)
else()
   target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_COMPILER_OTHER=1)
endif()

if(LINA_CORE_ENABLE_LOGGING)
	target_compile_definitions(${PROJECT_NAME} PRIVATE LINA_CORE_ENABLE_LOGGING=1)
endif()
#--------------------------------------------------------------------
# Build Type Config
#--------------------------------------------------------------------

if ( CMAKE_BUILD_TYPE STREQUAL "" )
	# CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
	# differentiation between debug and release builds.
	set(
		CMAKE_BUILD_TYPE
			"Debug"
		CACHE STRING
			"Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) \"Debug\" \"Release\" \"MinSizeRel\" \"RelWithDebInfo\"."
		FORCE
	)
	
	set(LINAENGINE_RUNTIME_COPY_DIR "Debug")
endif ( CMAKE_BUILD_TYPE STREQUAL "" )

if ( CMAKE_BUILD_TYPE STREQUAL "Release" )
    target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_RELEASE=1)
	set(LINAENGINE_RUNTIME_COPY_DIR "Release")
endif ( CMAKE_BUILD_TYPE STREQUAL "Release" )

if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_DEBUG=1)
	set(LINAENGINE_RUNTIME_COPY_DIR "Debug")
endif ( CMAKE_BUILD_TYPE STREQUAL "Debug" )

if ( CMAKE_BUILD_TYPE STREQUAL "MinSizeRel" )
    target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_MINSIZEREL=1)
	set(LINAENGINE_RUNTIME_COPY_DIR "MinSizeRel")
endif ( CMAKE_BUILD_TYPE STREQUAL "MinSizeRel" )

if ( CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" )
    target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_RELWITHDEBINFO=1)
	set(LINAENGINE_RUNTIME_COPY_DIR "RelWithDebInfo")
endif ( CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" )

#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/vendor/glfw/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/vendor/glad/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/vendor/assimp/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaCommon/include)

if(LINA_GRAPHICS_OPENGL)
	include(../CMake/GLGraphicsDefinitions.cmake)
endif()

# Language standard
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

#--------------------------------------------------------------------
# Subdirectories & linking
#--------------------------------------------------------------------

target_link_libraries(${PROJECT_NAME} 
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw/lib/$<CONFIGURATION>/glfw3.lib
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/lib/$<CONFIGURATION>/glad.lib
	PRIVATE Lina::Common
	PRIVATE Lina::ECS
)
	

#--------------------------------------------------------------------
# Packages
#--------------------------------------------------------------------
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_OPENGL=1)

find_package(OpenGL REQUIRED)

#--------------------------------------------------------------------
# Generate export header for this dll project 
#--------------------------------------------------------------------
# include (GenerateExportHeader)
# GENERATE_EXPORT_HEADER (LinaEngine
#    BASE_NAME LinaEngine
#    EXPORT_MACRO_NAME LINA_API
#    EXPORT_FILE_NAME LinaCore.hpp
#)

# Copy engine resources to build directory
add_custom_command(
TARGET ${PROJECT_NAME}
PRE_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/LinaGraphics/resources" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/resources")

add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/lib/$<CONFIGURATION>/LinaGraphics.lib" "${CMAKE_SOURCE_DIR}/LinaEngine/internal/Graphics/lib/$<CONFIGURATION>")

if(EXISTS "${CMAKE_BINARY_DIR}/lib/$<CONFIGURATION>/LinaGraphics.pdb")
    add_custom_command(
	TARGET ${PROJECT_NAME}
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/lib/$<CONFIGURATION>/LinaGraphics.pdb" "${CMAKE_SOURCE_DIR}/LinaEngine/internal/Graphics/lib/$<CONFIGURATION>")
endif()



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


