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
cmake_minimum_required (VERSION 3.8)
project(LinaCore)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------
set (SOURCES
	
# Audio
src/Audio/AudioManager.cpp

# Core
src/Application.cpp
src/ApplicationDelegate.cpp
src/Engine.cpp
src/SystemInfo.cpp
src/CommonCore.cpp
src/EngineInterface.cpp

# Graphics
src/Graphics/GUIBackend.cpp
src/Graphics/GfxManager.cpp
src/Graphics/ResourceUploadQueue.cpp
src/Graphics/MeshManager.cpp
src/Graphics/CameraSystem.cpp
src/Graphics/Utility/ShaderPreprocessor.cpp
src/Graphics/Utility/GfxHelpers.cpp
src/Graphics/Data/Vertex.cpp
src/Graphics/Resource/Shader.cpp
src/Graphics/Resource/Mesh.cpp
src/Graphics/Resource/Model.cpp
src/Graphics/Resource/Texture.cpp
src/Graphics/Resource/TextureSampler.cpp
src/Graphics/Resource/Material.cpp
src/Graphics/Resource/Font.cpp
src/Graphics/Renderers/SurfaceRenderer.cpp
src/Graphics/Renderers/WorldRenderer.cpp
src/Graphics/Pipeline/Buffer.cpp
src/Graphics/Pipeline/Renderpass.cpp

# Reflection
src/Reflection/ReflectionSystem.cpp

# Resources
src/Resources/CommonResources.cpp
src/Resources/ResourceManager.cpp
src/Resources/ResourceCache.cpp
src/Resources/Resource.cpp
src/Resources/Data/ResourceMetadata.cpp

# World
src/World/Entity.cpp
src/World/Component.cpp
src/World/ComponentCache.cpp
src/World/Level/Level.cpp
src/World/Level/LevelManager.cpp
src/World/EntityWorld.cpp


# Physics
src/Physics/PhysicsWorld.cpp

# Components
src/Components/RenderableComponent.cpp

)

#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(HEADERS

include/Core/Lina.hpp

# Audio
include/Core/Audio/AudioManager.hpp

# Core
include/Core/Application.hpp
include/Core/ApplicationDelegate.hpp
include/Core/Engine.hpp
include/Core/SystemInfo.hpp
include/Core/CommonCore.hpp
include/Core/EngineInterface.hpp

# Platform
include/Core/Platform/PlatformProcess.hpp

# Reflection
include/Core/Reflection/ClassReflection.hpp
include/Core/Reflection/ReflectionSystem.hpp

# Resources
include/Core/Resources/CommonResources.hpp
include/Core/Resources/ResourceManager.hpp
include/Core/Resources/ResourceCache.hpp
include/Core/Resources/Resource.hpp
include/Core/Resources/ResourcesReflection.hpp
include/Core/Resources/Data/ResourceMetadata.hpp

# Graphics
include/Core/Graphics/GUIBackend.hpp
include/Core/Graphics/GfxManager.hpp
include/Core/Graphics/CameraSystem.hpp
include/Core/Graphics/CommonGraphics.hpp
include/Core/Graphics/ResourceUploadQueue.hpp
include/Core/Graphics/MeshManager.hpp
include/Core/Graphics/Utility/ShaderPreprocessor.hpp
include/Core/Graphics/Utility/GfxHelpers.hpp
include/Core/Graphics/Data/RenderData.hpp
include/Core/Graphics/Data/Vertex.hpp
include/Core/Graphics/Resource/Shader.hpp
include/Core/Graphics/Resource/Mesh.hpp
include/Core/Graphics/Resource/Model.hpp
include/Core/Graphics/Resource/Texture.hpp
include/Core/Graphics/Resource/TextureSampler.hpp
include/Core/Graphics/Resource/Material.hpp
include/Core/Graphics/Resource/Font.hpp
include/Core/Graphics/Renderers/SurfaceRenderer.hpp
include/Core/Graphics/Renderers/WorldRenderer.hpp
include/Core/Graphics/Pipeline/Buffer.hpp
include/Core/Graphics/Pipeline/Renderpass.hpp

# World
include/Core/World/Entity.hpp
include/Core/World/Component.hpp
include/Core/World/ComponentCache.hpp
include/Core/World/Level/Level.hpp
include/Core/World/Level/LevelManager.hpp
include/Core/World/EntityWorld.hpp
include/Core/World/CommonWorld.hpp

# Physics
include/Core/Physics/PhysicsWorld.hpp

# Components
include/Core/Components/CameraComponent.hpp
include/Core/Components/RenderableComponent.hpp

)

if(WIN32)

set (PLATFORM_HEADERS
)

set (PLATFORM_SOURCES 
src/Platform/Win32/Win32PlatformProcess.cpp
)

endif()

if(APPLE)

set (PLATFORM_HEADERS
)

set (PLATFORM_SOURCES 
src/Platform/Apple/ApplePlatformProcess.mm
)

endif()


#--------------------------------------------------------------------
# Define the library & create an alias
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${PLATFORM_HEADERS} ${PLATFORM_SOURCES})
add_library(Lina::Core ALIAS ${PROJECT_NAME}) 

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "LinaEngine")

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

target_link_libraries(${PROJECT_NAME} 
	PUBLIC Lina::Common
)

if(APPLE)
	target_link_libraries(${PROJECT_NAME}
		PUBLIC "-framework AppKit"
		PUBLIC "-framework Foundation"
		PUBLIC "-framework QuartzCore"
		PUBLIC "-framework UniformTypeIdentifiers"
	)
endif()

message("LinaCore -> LinaCommon has been linked.")

if(WIN32)
target_link_libraries(${PROJECT_NAME} 
	PUBLIC user32.lib
	PUBLIC Shcore.lib
)
message("LinaCore -> User32 has been linked.")
message("LinaCore -> Shcore has been linked.")
endif()

#--------------------------------------------------------------------
# Folder structuring
#--------------------------------------------------------------------
include(../CMake/CommonFunctions.cmake)
group_sources(HEADERS SOURCES PLATFORM_HEADERS PLATFORM_SOURCES)
