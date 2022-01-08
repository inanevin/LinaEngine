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

	#Animation
	src/Animation/Animation.cpp
	src/Animation/Skeleton.cpp
	
	#Rendering
	src/Rendering/ArrayBitmap.cpp
	src/Rendering/Texture.cpp
	src/Rendering/Mesh.cpp
	src/Rendering/SkinnedMesh.cpp
	src/Rendering/StaticMesh.cpp
	src/Rendering/Material.cpp
	src/Rendering/Model.cpp
	src/Rendering/ModelNode.cpp
	src/Rendering/ModelAssetData.cpp
	src/Rendering/RenderingCommon.cpp
	src/Rendering/Shader.cpp
	src/Rendering/ShaderInclude.cpp
	src/Rendering/RenderSettings.cpp
	src/Rendering/PostProcessEffect.cpp
	src/Rendering/RenderBuffer.cpp
	src/Rendering/RenderTarget.cpp
	src/Rendering/Sampler.cpp
	src/Rendering/UniformBuffer.cpp
	src/Rendering/VertexArray.cpp
	src/Rendering/ImageAssetData.cpp
	
	#Utility 
	src/Utility/AssimpUtility.cpp
	src/Utility/ModelLoader.cpp

	src/Core/Backend/OpenGL/OpenGLWindow.cpp
	src/Core/Backend/OpenGL/OpenGLRenderEngine.cpp
	src/Core/Backend/OpenGL/OpenGLRenderDevice.cpp

	src/ECS/Systems/ModelNodeSystem.cpp
	src/ECS/Systems/SpriteRendererSystem.cpp
	src/ECS/Systems/CameraSystem.cpp
	src/ECS/Systems/LightingSystem.cpp
	src/ECS/Systems/AnimationSystem.cpp
	src/ECS/Systems/FrustumSystem.cpp
		
	src/ECS/Components/ModelRendererComponent.cpp
)

#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(LINAGRAPHICS_HEADERS

	# Backend
	include/Core/WindowBackend.hpp
	include/Core/RenderDeviceBackend.hpp
	include/Core/RenderEngineBackend.hpp
	include/Core/Backend/OpenGL/OpenGLRenderEngine.hpp
	include/Core/Backend/OpenGL/OpenGLRenderDevice.hpp
	include/Core/Backend/OpenGL/OpenGLWindow.hpp
	include/Core/RenderBackendFwd.hpp
	
	#Helpers
	include/Helpers/DrawParameterHelper.hpp
	
	#Animation
	include/Animation/Animation.hpp
	include/Animation/Skeleton.hpp


	#Rendering
	include/Rendering/ArrayBitmap.hpp
	include/Rendering/Shader.hpp
	include/Rendering/Texture.hpp
	include/Rendering/ImageAssetData.hpp
	include/Rendering/Mesh.hpp
	include/Rendering/SkinnedMesh.hpp
	include/Rendering/StaticMesh.hpp
	include/Rendering/VertexArray.hpp
	include/Rendering/Material.hpp
	include/Rendering/Shader.hpp
	include/Rendering/ShaderInclude.hpp
	include/Rendering/Sampler.hpp
	include/Rendering/UniformBuffer.hpp
	include/Rendering/RenderTarget.hpp
	include/Rendering/Model.hpp
	include/Rendering/ModelNode.hpp
	include/Rendering/ModelAssetData.hpp
	include/Rendering/RenderingCommon.hpp
	include/Rendering/RenderConstants.hpp
	include/Rendering/RenderBuffer.hpp
	include/Rendering/RenderSettings.hpp
	include/Rendering/PostProcessEffect.hpp
	
	
	include/ECS/Systems/AnimationSystem.hpp
	include/ECS/Systems/CameraSystem.hpp
	include/ECS/Systems/ModelNodeSystem.hpp
	include/ECS/Systems/LightingSystem.hpp
	include/ECS/Systems/SpriteRendererSystem.hpp
	include/ECS/Systems/FrustumSystem.hpp
	
	include/ECS/Components/MeshRendererComponent.hpp
	include/ECS/Components/ModelRendererComponent.hpp
	include/ECS/Components/CameraComponent.hpp
	include/ECS/Components/LightComponent.hpp
	include/ECS/Components/SpriteRendererComponent.hpp
	include/ECS/Components/AnimationComponent.hpp
	include/ECS/Components/ModelNodeComponent.hpp

	include/Utility/AssimpUtility.hpp
	include/Utility/ModelLoader.hpp

	include/Utility/stb/stb_image.h
	include/Utility/stb/stb_image_write.h


)


#--------------------------------------------------------------------
# Define the library & create an alias
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${LINAGRAPHICS_SOURCES} ${LINAGRAPHICS_HEADERS})
add_library(Lina::Graphics ALIAS ${PROJECT_NAME}) 
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ON)
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD_MODE BATCH UNITY_BUILD_BATCH_SIZE 16)

#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glfw/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glad/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/assimp/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaCommon/include)


#--------------------------------------------------------------------
# Subdirectories & linking
#--------------------------------------------------------------------

target_link_libraries(${PROJECT_NAME} 
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glad/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/glad.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/assimp/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/${ASSIMP_LIB}
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glfw/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/glfw3.lib
	PRIVATE Lina::Common
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


