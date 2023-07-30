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
project(LinaCore)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------
set (LINACORE_SOURCES
	
# Audio
src/Audio/Core/AudioManager.cpp

# Core
src/Core/Application.cpp
src/Core/Engine.cpp
src/Core/SystemInfo.cpp
src/Core/CommonCore.cpp
src/Core/IEngineInterface.cpp
src/Core/CoreResourcesRegistry.cpp

# World
src/World/Core/Entity.cpp
src/World/Core/Component.cpp
src/World/Core/ComponentCache.cpp
src/World/Level/Level.cpp
src/World/Level/LevelManager.cpp
src/World/Core/EntityWorld.cpp

# Input
src/Input/Core/Input.cpp

# Graphics

src/Graphics/Core/WindowManager.cpp
src/Graphics/Core/GUIBackend.cpp
src/Graphics/Core/GfxManager.cpp
src/Graphics/Core/View.cpp
src/Graphics/Core/GUIRenderer.cpp
src/Graphics/Core/DrawPass.cpp
src/Graphics/Core/CameraSystem.cpp
src/Graphics/Core/WorldRenderer.cpp
src/Graphics/Core/SurfaceRenderer.cpp
src/Graphics/Core/GfxMeshManager.cpp
src/Graphics/Interfaces/IGUIDrawer.cpp
src/Graphics/Components/RenderableComponent.cpp
src/Graphics/Components/ModelNodeComponent.cpp
src/Graphics/Data/Vertex.cpp
src/Graphics/Resource/Shader.cpp
src/Graphics/Resource/Mesh.cpp
src/Graphics/Resource/Model.cpp
src/Graphics/Resource/ModelNode.cpp
src/Graphics/Resource/SkinnedMesh.cpp
src/Graphics/Resource/StaticMesh.cpp
src/Graphics/Resource/Texture.cpp
src/Graphics/Resource/TextureSampler.cpp
src/Graphics/Resource/Material.cpp
src/Graphics/Resource/MaterialProperty.cpp
src/Graphics/Resource/Font.cpp


# Physics
src/Physics/Core/PhysicsWorld.cpp

# Reflection
src/Reflection/ReflectionSystem.cpp

# Resources
src/Resources/Core/CommonResources.cpp
src/Resources/Core/ResourceManager.cpp
src/Resources/Core/ResourceCache.cpp
src/Resources/Core/IResource.cpp
src/Resources/Data/ResourceMetadata.cpp

)

#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(LINACORE_HEADERS

include/Lina.hpp

# Audio
include/Audio/Core/AudioManager.hpp

# Core
include/Core/Application.hpp
include/Core/Engine.hpp
include/Core/SystemInfo.hpp
include/Core/CommonCore.hpp
include/Core/IEngineInterface.hpp
include/Core/PlatformProcess.hpp
include/Core/CoreResourcesRegistry.hpp

# World
include/World/Core/Entity.hpp
include/World/Core/Component.hpp
include/World/Core/ComponentCache.hpp
include/World/Level/Level.hpp
include/World/Level/LevelManager.hpp
include/World/Core/EntityWorld.hpp
include/World/Core/CommonWorld.hpp

# Input
include/Input/Core/Input.hpp
include/Input/Core/CommonInput.hpp
include/Input/Core/InputMappings.hpp

# Physics
include/Physics/Core/PhysicsWorld.hpp

# Reflection
include/Reflection/ClassReflection.hpp
include/Reflection/ReflectionSystem.hpp

# Resources
include/Resources/Core/CommonResources.hpp
include/Resources/Core/ResourceManager.hpp
include/Resources/Core/ResourceCache.hpp
include/Resources/Core/IResource.hpp
include/Resources/Core/ResourcesReflection.hpp
include/Resources/Data/ResourceMetadata.hpp

# Graphics
include/Graphics/Core/WindowManager.hpp
include/Graphics/Core/GUIBackend.hpp
include/Graphics/Core/GUIRenderer.hpp

include/Graphics/Core/GfxManager.hpp
include/Graphics/Core/WorldRenderer.hpp
include/Graphics/Core/SurfaceRenderer.hpp
include/Graphics/Core/CommonGraphics.hpp
include/Graphics/Core/View.hpp
include/Graphics/Core/DrawPass.hpp
include/Graphics/Core/GfxMeshManager.hpp
include/Graphics/Core/CameraSystem.hpp
include/Graphics/Interfaces/IGUIDrawer.hpp
include/Graphics/Data/RenderData.hpp
include/Graphics/Data/Vertex.hpp
include/Graphics/Data/DescriptorHandle.hpp
include/Graphics/Components/CameraComponent.hpp
include/Graphics/Components/RenderableComponent.hpp
include/Graphics/Components/ModelNodeComponent.hpp
include/Graphics/Platform/LinaVGIncl.hpp
include/Graphics/Resource/Shader.hpp
include/Graphics/Resource/Mesh.hpp
include/Graphics/Resource/Model.hpp
include/Graphics/Resource/ModelNode.hpp
include/Graphics/Resource/SkinnedMesh.hpp
include/Graphics/Resource/StaticMesh.hpp
include/Graphics/Resource/Texture.hpp
include/Graphics/Resource/TextureSampler.hpp
include/Graphics/Resource/Material.hpp
include/Graphics/Resource/MaterialProperty.hpp
include/Graphics/Resource/Font.hpp

)

if(WIN32)

set (LINACORE_PLATFORM_HEADERS
include/Platform/Win32/Win32PlatformProcess.hpp
)

set (LINACORE_PLATFORM_SOURCES 
src/Platform/Win32/Win32PlatformProcess.cpp
)

endif()

set(SHADER_SOURCES
)

#--------------------------------------------------------------------
# Define the library & create an alias
#--------------------------------------------------------------------


add_library(${PROJECT_NAME} ${LINACORE_SOURCES} ${LINACORE_HEADERS} ${LINACORE_PLATFORM_HEADERS} ${LINACORE_PLATFORM_SOURCES} ${SHADER_SOURCES})
add_library(Lina::Core ALIAS ${PROJECT_NAME}) 


## Remove Nvidia PhysX linker pdb warning
target_link_options(${PROJECT_NAME} INTERFACE "/ignore:4099") 
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Lina")

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

include(../_Dependencies/freetype/link.cmake)
include(../_Dependencies/linavg/link.cmake)
include(../_Dependencies/alut/link.cmake)
include(../_Dependencies/openal/link.cmake)
link_freetype()
link_linavg()
link_alut()
link_openal()


target_link_libraries(${PROJECT_NAME} 
	PUBLIC Lina::Common
)

if(WIN32)
target_link_libraries(${PROJECT_NAME} 
	PUBLIC user32.lib
	PUBLIC Shcore.lib
)
message("LinaCore -> User32 Linked.")
message("LinaCore -> Shcore Linked.")
endif()



##include (GenerateExportHeader)
##	GENERATE_EXPORT_HEADER (LinaCore
##	BASE_NAME LinaCore
##	EXPORT_MACRO_NAME LINACORE_API
##	EXPORT_FILE_NAME LinaCoreExports.hpp
##)

#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	foreach(source IN LISTS LINACORE_HEADERS LINACORE_SOURCES LINACORE_PLATFORM_HEADERS LINACORE_PLATFORM_SOURCES SHADER_SOURCES) 
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${LinaCore_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
	endforeach()

endif()
