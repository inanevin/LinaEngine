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
	src/Core/GUIBackend.cpp
	src/Core/Window.cpp
	src/Core/GraphicsCommon.cpp
	src/Core/Renderer.cpp
	src/Core/FramePacket.cpp
	src/Core/View.cpp
	src/Core/DrawPass.cpp
	src/Core/CameraSystem.cpp
	src/Core/WorldRenderer.cpp

	# Data
	src/Data/Vertex.cpp

	# Components
	src/Components/RenderableComponent.cpp
	src/Components/ModelNodeComponent.cpp

	# Vulkan
	src/PipelineObjects/Buffer.cpp
	src/PipelineObjects/DescriptorSet.cpp
	src/PipelineObjects/DescriptorSetLayout.cpp
	src/PipelineObjects/DescriptorPool.cpp
	src/PipelineObjects/Framebuffer.cpp
	src/PipelineObjects/SwapChain.cpp
	src/PipelineObjects/RQueue.cpp
	src/PipelineObjects/CommandPool.cpp
	src/PipelineObjects/CommandBuffer.cpp
	src/PipelineObjects/RenderPass.cpp
	src/PipelineObjects/Fence.cpp
	src/PipelineObjects/UploadContext.cpp
	src/PipelineObjects/Semaphore.cpp
	src/PipelineObjects/Pipeline.cpp
	src/PipelineObjects/PipelineLayout.cpp
	src/PipelineObjects/Image.cpp
	src/PipelineObjects/Sampler.cpp

	# Resource
	src/Resource/Shader.cpp
	src/Resource/Mesh.cpp
	src/Resource/Model.cpp
	src/Resource/ModelNode.cpp
	src/Resource/SkinnedMesh.cpp
	src/Resource/StaticMesh.cpp
	src/Resource/Texture.cpp
	src/Resource/Material.cpp
	src/Resource/MaterialProperty.cpp

	# Features
	src/FeatureRenderers/FeatureRendererManager.cpp
	src/FeatureRenderers/SkyRenderer.cpp
	src/FeatureRenderers/StaticMeshRenderer.cpp
	src/FeatureRenderers/SkinnedMeshRenderer.cpp
	src/FeatureRenderers/DecalRenderer.cpp
	src/FeatureRenderers/SpriteRenderer.cpp
	src/FeatureRenderers/ParticleRenderer.cpp

	# Utility
	src/Utility/Vulkan/VkBootstrap.cpp
	src/Utility/Vulkan/SPIRVUtility.cpp
	src/Utility/Vulkan/VulkanUtility.cpp
	src/Utility/DeletionQueue.cpp
	src/Utility/Command.cpp
	src/Utility/ModelLoader.cpp
	src/Utility/ShaderUtility.cpp
)

#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(LINAGRAPHICS_HEADERS

	# Core
	include/Core/GraphicsCommon.hpp
	include/Core/RenderEngine.hpp
	include/Core/Backend.hpp
	include/Core/GUIBackend.hpp
	include/Core/Window.hpp
	include/Core/Renderer.hpp
	include/Core/RenderData.hpp
	include/Core/FramePacket.hpp
	include/Core/View.hpp
	include/Core/DrawPass.hpp
	include/Core/CameraSystem.hpp
	include/Core/WorldRenderer.hpp

	# Data
	include/Data/Vertex.hpp

	# Components
	include/Components/RenderableComponent.hpp
	include/Components/SkyComponent.hpp
	include/Components/SpriteComponent.hpp
	include/Components/ModelNodeComponent.hpp
	include/Components/DecalComponent.hpp
	include/Components/LightComponent.hpp
	include/Components/ParticleComponent.hpp
	include/Components/CameraComponent.hpp

	# Vulkan
	include/PipelineObjects/Buffer.hpp
	include/PipelineObjects/DescriptorSet.hpp
	include/PipelineObjects/DescriptorSetLayout.hpp
	include/PipelineObjects/DescriptorPool.hpp
	include/PipelineObjects/Framebuffer.hpp
	include/PipelineObjects/Swapchain.hpp
	include/PipelineObjects/RQueue.hpp
	include/PipelineObjects/CommandPool.hpp
	include/PipelineObjects/CommandBuffer.hpp
	include/PipelineObjects/RenderPass.hpp
	include/PipelineObjects/Fence.hpp
	include/PipelineObjects/UploadContext.hpp
	include/PipelineObjects/Semaphore.hpp
	include/PipelineObjects/Pipeline.hpp
	include/PipelineObjects/PipelineLayout.hpp
	include/PipelineObjects/Image.hpp
	include/PipelineObjects/Sampler.hpp

	# Resource
	include/Resource/Shader.hpp
	include/Resource/Mesh.hpp
	include/Resource/Model.hpp
	include/Resource/ModelNode.hpp
	include/Resource/SkinnedMesh.hpp
	include/Resource/StaticMesh.hpp
	include/Resource/Texture.hpp
	include/Resource/Material.hpp
	include/Resource/MaterialProperty.hpp
	
	# Features
	include/FeatureRenderers/FeatureRendererManager.hpp
	include/FeatureRenderers/SkyRenderer.hpp
	include/FeatureRenderers/StaticMeshRenderer.hpp
	include/FeatureRenderers/SkinnedMeshRenderer.hpp
	include/FeatureRenderers/DecalRenderer.hpp
	include/FeatureRenderers/SpriteRenderer.hpp
	include/FeatureRenderers/ParticleRenderer.hpp
	include/FeatureRenderers/FeatureRenderer.hpp

	# Utility
	include/Utility/stb/stb_image.h
	include/Utility/stb/stb_image_write.h
	include/Utility/Vulkan/VkBootstrap.h
	include/Utility/Vulkan/VkBootstrapDispatch.h
	include/Utility/Vulkan/SPIRVUtility.hpp
	include/Utility/Vulkan/VulkanUtility.hpp
	include/Utility/Vulkan/vk_mem_alloc.h
	include/Utility/DeletionQueue.hpp
	include/Utility/Command.hpp
	include/Utility/ModelLoader.hpp
	include/Utility/ShaderUtility.hpp
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
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaWorld/include)

target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})


#--------------------------------------------------------------------
# Subdirectories & linking
#--------------------------------------------------------------------

target_link_libraries(${PROJECT_NAME} 
PRIVATE Lina::Common
PRIVATE Lina::Resource 
PRIVATE Lina::World 
Vulkan::Vulkan)
	
include(../_Dependencies/glad/link.cmake)
include(../_Dependencies/glfw/link.cmake)
include(../_Dependencies/assimp/link.cmake)
include(../_Dependencies/glslang/link.cmake)
include(../_Dependencies/freetype/link.cmake)
include(../_Dependencies/linavg/link.cmake)
link_glad()
link_glfw()
link_assimp()
link_glslang()
link_freetype()
link_linavg()

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


