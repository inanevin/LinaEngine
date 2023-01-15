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
project(LinaEngine)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------
set (LINAENGINE_SOURCES
	
	# Audio
	src/Audio/Audio.cpp
	src/Audio/AudioEngine.cpp

	# Input
	src/Input/Core/InputEngine.cpp
	src/Input/Core/InputAxis.cpp

	# CORE
    src/Core/Application.cpp
	src/Core/Engine.cpp

	# Settings
	src/Settings/EngineSettings.cpp
	src/Settings/RenderSettings.cpp

	# Game
	src/Game/GameManager.cpp
	src/EntryPoint.cpp

	# Graphics
	src/Graphics/Core/RenderEngine.cpp
	src/Graphics/Core/Backend.cpp
	src/Graphics/Core/GUIBackend.cpp
	src/Graphics/Core/WindowManager.cpp
	src/Graphics/Core/GraphicsCommon.cpp
	src/Graphics/Core/Renderer.cpp
	src/Graphics/Core/View.cpp
	src/Graphics/Core/DrawPass.cpp
	src/Graphics/Core/CameraSystem.cpp
	src/Graphics/Core/Screen.cpp
	src/Graphics/Data/Vertex.cpp
	src/Graphics/Components/RenderableComponent.cpp
	src/Graphics/Components/ModelNodeComponent.cpp
	src/Graphics/PipelineObjects/Buffer.cpp
	src/Graphics/PipelineObjects/DescriptorSet.cpp
	src/Graphics/PipelineObjects/DescriptorSetLayout.cpp
	src/Graphics/PipelineObjects/DescriptorPool.cpp
	src/Graphics/PipelineObjects/Framebuffer.cpp
	src/Graphics/PipelineObjects/SwapChain.cpp
	src/Graphics/PipelineObjects/RQueue.cpp
	src/Graphics/PipelineObjects/CommandPool.cpp
	src/Graphics/PipelineObjects/CommandBuffer.cpp
	src/Graphics/PipelineObjects/RenderPass.cpp
	src/Graphics/PipelineObjects/Fence.cpp
	src/Graphics/PipelineObjects/UploadContext.cpp
	src/Graphics/PipelineObjects/Semaphore.cpp
	src/Graphics/PipelineObjects/Pipeline.cpp
	src/Graphics/PipelineObjects/PipelineLayout.cpp
	src/Graphics/PipelineObjects/Image.cpp
	src/Graphics/PipelineObjects/Sampler.cpp
	src/Graphics/Platform/Win32/Win32Window.cpp
	src/Graphics/Resource/Shader.cpp
	src/Graphics/Resource/Mesh.cpp
	src/Graphics/Resource/Model.cpp
	src/Graphics/Resource/ModelNode.cpp
	src/Graphics/Resource/SkinnedMesh.cpp
	src/Graphics/Resource/StaticMesh.cpp
	src/Graphics/Resource/Texture.cpp
	src/Graphics/Resource/Material.cpp
	src/Graphics/Resource/MaterialProperty.cpp
	src/Graphics/Resource/Font.cpp
	src/Graphics/Utility/Vulkan/VkBootstrap.cpp
	src/Graphics/Utility/Vulkan/SPIRVUtility.cpp
	src/Graphics/Utility/Vulkan/VulkanUtility.cpp
	src/Graphics/Utility/DeletionQueue.cpp
	src/Graphics/Utility/ModelLoader.cpp
	src/Graphics/Utility/ShaderUtility.cpp

	# Physics
	src/Physics/Components/PhysicsComponent.cpp
	src/Physics/Core/PhysicsCommon.cpp
	src/Physics/Core/PhysicsCooker.cpp
	src/Physics/Core/PhysicsEngine.cpp
	src/Physics/Core/PhysicsMaterial.cpp
	src/Physics/Core/Raycast.cpp
	src/Physics/Core/RigidbodySystem.cpp

	# Resource
	src/Resource/Core/ResourceManager.cpp
	src/Resource/Core/Resource.cpp
	src/Resource/Core/ResourceCommon.cpp
	src/Resource/Core/ResourceLoader.cpp
	src/Resource/Core/ResourceCache.cpp
	src/Resource/Core/ResourcePackager.cpp
	src/Resource/Utility/ResourceUtility.cpp

	# World
	src/World/Core/Level.cpp
	src/World/Core/LevelManager.cpp
	src/World/Core/World.cpp
	src/World/Core/Entity.cpp
	src/World/Core/Component.cpp
	src/World/Core/ComponentCache.cpp
)

#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(LINAENGINE_HEADERS

	# Audio
	include/Audio/AudioEngine.hpp
	include/Audio/Audio.hpp

	# Input
	include/Input/Core/InputAxis.hpp
	include/Input/Core/InputMappings.hpp
	include/Input/Core/InputEngine.hpp
	include/Input/Core/InputCommon.hpp

	#CORE
	include/Core/Application.hpp
	include/Core/Engine.hpp

	# Settings
	include/Settings/EngineSettings.hpp
	include/Settings/RenderSettings.hpp

	# Game
	include/Game/GameManager.hpp
	
	include/EntryPoint.hpp
	include/Lina.hpp

	# Graphics
	include/Graphics/Core/GraphicsCommon.hpp
	include/Graphics/Core/RenderEngine.hpp
	include/Graphics/Core/Backend.hpp
	include/Graphics/Core/GUIBackend.hpp
	include/Graphics/Core/WindowManager.hpp
	include/Graphics/Core/Renderer.hpp
	include/Graphics/Core/RenderData.hpp
	include/Graphics/Core/View.hpp
	include/Graphics/Core/DrawPass.hpp
	include/Graphics/Core/CameraSystem.hpp
	include/Graphics/Core/Screen.hpp
	include/Graphics/Data/Vertex.hpp
	include/Graphics/Components/RenderableComponent.hpp
	include/Graphics/Components/SkyComponent.hpp
	include/Graphics/Components/SpriteComponent.hpp
	include/Graphics/Components/ModelNodeComponent.hpp
	include/Graphics/Components/DecalComponent.hpp
	include/Graphics/Components/LightComponent.hpp
	include/Graphics/Components/ParticleComponent.hpp
	include/Graphics/Components/CameraComponent.hpp
	include/Graphics/PipelineObjects/Buffer.hpp
	include/Graphics/PipelineObjects/DescriptorSet.hpp
	include/Graphics/PipelineObjects/DescriptorSetLayout.hpp
	include/Graphics/PipelineObjects/DescriptorPool.hpp
	include/Graphics/PipelineObjects/Framebuffer.hpp
	include/Graphics/PipelineObjects/Swapchain.hpp
	include/Graphics/PipelineObjects/RQueue.hpp
	include/Graphics/PipelineObjects/CommandPool.hpp
	include/Graphics/PipelineObjects/CommandBuffer.hpp
	include/Graphics/PipelineObjects/RenderPass.hpp
	include/Graphics/PipelineObjects/Fence.hpp
	include/Graphics/PipelineObjects/UploadContext.hpp
	include/Graphics/PipelineObjects/Semaphore.hpp
	include/Graphics/PipelineObjects/Pipeline.hpp
	include/Graphics/PipelineObjects/PipelineLayout.hpp
	include/Graphics/PipelineObjects/Image.hpp
	include/Graphics/PipelineObjects/Sampler.hpp
	include/Graphics/Platform/Win32/Win32Window.hpp
	include/Graphics/Platform/LinaVGIncl.hpp
	include/Graphics/Resource/Shader.hpp
	include/Graphics/Resource/Mesh.hpp
	include/Graphics/Resource/Model.hpp
	include/Graphics/Resource/ModelNode.hpp
	include/Graphics/Resource/SkinnedMesh.hpp
	include/Graphics/Resource/StaticMesh.hpp
	include/Graphics/Resource/Texture.hpp
	include/Graphics/Resource/Material.hpp
	include/Graphics/Resource/MaterialProperty.hpp
	include/Graphics/Resource/Font.hpp
	include/Graphics/Utility/stb/stb_image.h
	include/Graphics/Utility/stb/stb_image_resize.h
	include/Graphics/Utility/stb/stb_image_write.h
	include/Graphics/Utility/Vulkan/VkBootstrap.h
	include/Graphics/Utility/Vulkan/VkBootstrapDispatch.h
	include/Graphics/Utility/Vulkan/SPIRVUtility.hpp
	include/Graphics/Utility/Vulkan/VulkanUtility.hpp
	include/Graphics/Utility/Vulkan/vk_mem_alloc.h
	include/Graphics/Utility/DeletionQueue.hpp
	include/Graphics/Utility/ModelLoader.hpp
	include/Graphics/Utility/ShaderUtility.hpp

	# Physics
	include/Physics/Components/PhysicsComponent.hpp
	include/Physics/Core/PhysicsCommon.hpp
	include/Physics/Core/PhysicsCooker.hpp
	include/Physics/Core/PhysicsEngine.hpp
	include/Physics/Core/PhysicsMaterial.hpp
	include/Physics/Core/Raycast.hpp
	include/Physics/Core/RigidbodySystem.hpp

	# Resource
	include/Resource/Core/ResourceCommon.hpp
	include/Resource/Core/ResourceManager.hpp
	include/Resource/Core/Resource.hpp
	include/Resource/Core/ResourceLoader.hpp
	include/Resource/Core/ResourceHandle.hpp
	include/Resource/Core/ResourceCache.hpp
	include/Resource/Core/ResourcePackager.hpp
	include/Resource/Utility/ResourceUtility.hpp

	# World
	include/World/Core/Level.hpp
	include/World/Core/ComponentCache.hpp
	include/World/Core/LevelManager.hpp
	include/World/Core/World.hpp
	include/World/Core/Entity.hpp
	include/World/Core/Component.hpp
	include/World/Core/WorldCommon.hpp
)

#--------------------------------------------------------------------
# Define the library & create an alias
#--------------------------------------------------------------------

add_library(${PROJECT_NAME} ${LINAENGINE_SOURCES} ${LINAENGINE_HEADERS})
add_library(Lina::Engine ALIAS ${PROJECT_NAME}) 
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ${UNITY_BUILD_VALUE} UNITY_BUILD_UNIQUE_ID "LinaEngine")
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD_MODE BATCH UNITY_BUILD_BATCH_SIZE 20)

## Remove Nvidia PhysX linker pdb warning
target_link_options(${PROJECT_NAME} INTERFACE "/ignore:4099") 

#--------------------------------------------------------------------
# Config & Options & Compile Definitions
#--------------------------------------------------------------------
include(../CMake/ProjectSettings.cmake)
add_definitions(/bigobj)


#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------
find_package(Vulkan REQUIRED FATAL_ERROR)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaCommon/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})

if(NOT LINA_PRODUCTION_BUILD)
	target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaEditor/include)
endif()

#--------------------------------------------------------------------
# Subdirectories & linking
#--------------------------------------------------------------------

include(../_Dependencies/assimp/link.cmake)
include(../_Dependencies/glslang/link.cmake)
include(../_Dependencies/freetype/link.cmake)
include(../_Dependencies/linavg/link.cmake)
include(../_Dependencies/physx/link.cmake)
include(../_Dependencies/alut/link.cmake)
include(../_Dependencies/openal/link.cmake)
link_assimp()
link_glslang()
link_freetype()
link_linavg()
link_physx()
link_alut()
link_openal()

# Link libraries (e.g. logging)
target_link_libraries(${PROJECT_NAME} 
	# ${CMAKE_THREAD_LIBS_INIT}     # Needed for unix builds to link to pthread
	PUBLIC Lina::Common
	PUBLIC user32.lib
	Vulkan::Vulkan
)

if(WIN32)
	target_link_libraries(${PROJECT_NAME} PUBLIC Shcore.lib)
endif()

if(NOT LINA_PRODUCTION_BUILD)
	target_link_libraries(${PROJECT_NAME} PUBLIC Lina::Editor)
endif()



#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	foreach(source IN LISTS LINAENGINE_HEADERS LINAENGINE_SOURCES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${LinaEngine_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
endif()
