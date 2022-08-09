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

	# Core
	src/Core/RenderEngine.cpp
	src/Core/Backend.cpp
	src/Core/Window.cpp
	src/Core/GraphicsCommon.cpp

	# Vulkan
	src/Data/Attachment.cpp
	src/Data/Framebuffer.cpp
	src/Data/SwapChain.cpp
	src/Data/RQueue.cpp
	src/Data/CommandPool.cpp
	src/Data/CommandBuffer.cpp
	src/Data/RenderPass.cpp
	src/Data/Fence.cpp
	src/Data/Semaphore.cpp
	src/Data/Pipeline.cpp
	src/Data/PipelineLayout.cpp

	# Resource
	src/Resource/Shader.cpp

	# ECS
	src/ECS/Systems/SkySystem.cpp
	src/ECS/Systems/SpriteSystem.cpp
	src/ECS/Systems/MeshSystem.cpp
	src/ECS/Systems/DecalSystem.cpp
	src/ECS/Systems/LightingSystem.cpp
	src/ECS/Systems/ParticleSystem.cpp

	# Utility
	src/Utility/VkBootstrap.cpp
	src/Utility/SPIRVUtility.cpp
	src/Utility/VulkanUtility.cpp
	src/Utility/DeletionQueue.cpp
)

#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(LINAGRAPHICS_HEADERS

	# Core
	include/Core/GraphicsCommon.hpp
	include/Core/RenderEngine.hpp
	include/Core/Backend.hpp
	include/Core/Window.hpp

	# Vulkan
	include/Data/Attachment.hpp
	include/Data/Framebuffer.hpp
	include/Data/Swapchain.hpp
	include/Data/RQueue.hpp
	include/Data/CommandPool.hpp
	include/Data/CommandBuffer.hpp
	include/Data/RenderPass.hpp
	include/Data/Fence.hpp
	include/Data/Semaphore.hpp
	include/Data/Pipeline.hpp
	include/Data/PipelineLayout.hpp

	# Resource
	include/Resource/Shader.hpp

	# ECS
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

	# Utility
	include/Utility/stb/stb_image.h
	include/Utility/stb/stb_image_write.h
	include/Utility/VkBootstrap.h
	include/Utility/VkBootstrapDispatch.h
	include/Utility/SPIRVUtility.hpp
	include/Utility/VulkanUtility.hpp
	include/Utility/DeletionQueue.hpp

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
find_package(Vulkan REQUIRED FATAL_ERROR)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaCommon/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaResource/include)

target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})


#--------------------------------------------------------------------
# Subdirectories & linking
#--------------------------------------------------------------------

target_link_libraries(${PROJECT_NAME} 
PRIVATE Lina::Common
PRIVATE Lina::Resource 
Vulkan::Vulkan)
	
include(../_Dependencies/glad/link.cmake)
include(../_Dependencies/glfw/link.cmake)
include(../_Dependencies/assimp/link.cmake)
include(../_Dependencies/glslang/link.cmake)
link_glad()
link_glfw()
link_assimp()
link_glslang()

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


