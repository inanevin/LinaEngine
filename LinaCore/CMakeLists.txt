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
cmake_minimum_required (VERSION 3.10...3.31)
project(LinaCore)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------
set (SOURCES
	
# Audio
src/Audio/AudioBackend.cpp
src/Audio/Audio.cpp

# Core
src/Application.cpp
src/ApplicationDelegate.cpp
src/CommonCore.cpp

# Graphics
src/Graphics/ResourceUploadQueue.cpp
src/Graphics/MeshManager.cpp
src/Graphics/GfxContext.cpp

src/Graphics/GUI/GUIBackend.cpp
src/Graphics/Utility/ShaderPreprocessor.cpp
src/Graphics/Utility/GfxHelpers.cpp
src/Graphics/Utility/TextureAtlas.cpp
src/Graphics/Data/Vertex.cpp
src/Graphics/Data/ModelAnimation.cpp
src/Graphics/Data/ModelSkin.cpp
src/Graphics/Data/RenderData.cpp
src/Graphics/Data/Mesh.cpp
src/Graphics/Data/Primitive.cpp
src/Graphics/Data/ModelNode.cpp
src/Graphics/Data/ModelMaterial.cpp
src/Graphics/Resource/Shader.cpp
src/Graphics/Resource/ShaderVariant.cpp
src/Graphics/Resource/Model.cpp
src/Graphics/Resource/Texture.cpp
src/Graphics/Resource/TextureSampler.cpp
src/Graphics/Resource/Material.cpp
src/Graphics/Resource/GUIWidget.cpp
src/Graphics/Resource/Font.cpp
src/Graphics/Renderers/WorldRenderer.cpp
src/Graphics/Renderers/DrawCollector.cpp
src/Graphics/Renderers/ShapeRenderer.cpp
src/Graphics/Renderers/PhysicsDebugRenderer.cpp
src/Graphics/Pipeline/Buffer.cpp
src/Graphics/Pipeline/RenderPass.cpp
src/Graphics/Pipeline/DescriptorSet.cpp
src/Graphics/Pipeline/View.cpp
src/Graphics/CommonGraphics.cpp

# Meta
src/Meta/ProjectData.cpp


# Resources
src/Resources/CommonResources.cpp
src/Resources/ResourceDirectory.cpp
src/Resources/ResourceManager.cpp
src/Resources/ResourceCache.cpp
src/Resources/Resource.cpp
src/Resources/Data/ResourceMetadata.cpp

# World
src/World/Entity.cpp
src/World/Component.cpp
src/World/ComponentCache.cpp
src/World/Screen.cpp
src/World/WorldInput.cpp
src/World/EntityWorld.cpp
src/World/EntityTemplate.cpp
src/World/Camera.cpp
src/World/Components/CompCamera.cpp
src/World/Components/CompModel.cpp
src/World/Components/CompLight.cpp
src/World/Components/CompWidget.cpp
src/World/Components/CompAudio.cpp
src/World/Animation/AnimationController.cpp
src/World/WorldUtility.cpp

# Physics
src/Physics/PhysicsWorld.cpp
src/Physics/PhysicsMaterial.cpp
src/Physics/PhysicsBackend.cpp
src/Physics/CommonPhysics.cpp
src/Physics/PhysicsLayerFilter.cpp
src/Physics/PhysicsBPLayerInterface.cpp
src/Physics/PhysicsObjectBPLayerFilter.cpp
src/Physics/PhysicsRayCollector.cpp

# GUI
src/GUI/Theme.cpp
src/GUI/CommonGUI.cpp
src/GUI/Widgets/Widget.cpp
src/GUI/Widgets/WidgetManager.cpp
src/GUI/Widgets/WidgetCache.cpp
src/GUI/Widgets/WidgetUtility.cpp
src/GUI/Widgets/Layout/DirectionalLayout.cpp
src/GUI/Widgets/Layout/Popup.cpp
src/GUI/Widgets/Layout/ForegroundItem.cpp
src/GUI/Widgets/Layout/ScrollArea.cpp
src/GUI/Widgets/Layout/FoldLayout.cpp
src/GUI/Widgets/Layout/GridLayout.cpp
src/GUI/Widgets/Primitives/Text.cpp
src/GUI/Widgets/Primitives/Circle.cpp
src/GUI/Widgets/Primitives/Icon.cpp
src/GUI/Widgets/Primitives/Slider.cpp
src/GUI/Widgets/Primitives/Button.cpp
src/GUI/Widgets/Primitives/Checkbox.cpp
src/GUI/Widgets/Primitives/InputField.cpp
src/GUI/Widgets/Primitives/Dropdown.cpp
src/GUI/Widgets/Primitives/ColorField.cpp
src/GUI/Widgets/Primitives/ColorSlider.cpp
src/GUI/Widgets/Compound/FileMenu.cpp


)

#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(HEADERS

include/Core/Lina.hpp
include/Core/Application.hpp
include/Core/ApplicationDelegate.hpp
include/Core/CommonCore.hpp

# Audio
include/Core/Audio/AudioBackend.hpp
include/Core/Audio/Audio.hpp
include/Core/Audio/Loaders/AudioFile.h

# Platform
include/Core/Platform/PlatformProcess.hpp
include/Core/System/PluginInterface.hpp
include/Core/System/Plugin.hpp

# Reflection
include/Core/Reflection/ComponentReflection.hpp
include/Core/Reflection/WidgetReflection.hpp
include/Core/Reflection/ResourceReflection.hpp
include/Core/Reflection/CommonReflection.hpp

# Resources
include/Core/Resources/CommonResources.hpp
include/Core/Resources/ResourceDirectory.hpp
include/Core/Resources/ResourceManager.hpp
include/Core/Resources/ResourceManagerListener.hpp
include/Core/Resources/ResourceCache.hpp
include/Core/Resources/Resource.hpp
include/Core/Resources/ResourcesReflection.hpp
include/Core/Resources/Data/ResourceMetadata.hpp

# Graphics
include/Core/Graphics/CommonGraphics.hpp
include/Core/Graphics/ResourceUploadQueue.hpp
include/Core/Graphics/MeshManager.hpp
include/Core/Graphics/GfxContext.hpp

include/Core/Graphics/GUI/GUIBackend.hpp
include/Core/Graphics/Utility/ShaderPreprocessor.hpp
include/Core/Graphics/Utility/GfxHelpers.hpp
include/Core/Graphics/Utility/TextureAtlas.hpp
include/Core/Graphics/Data/RenderData.hpp
include/Core/Graphics/Data/Vertex.hpp
include/Core/Graphics/Data/ModelAnimation.hpp
include/Core/Graphics/Data/ModelSkin.hpp
include/Core/Graphics/Data/Mesh.hpp
include/Core/Graphics/Data/Primitive.hpp
include/Core/Graphics/Data/ModelNode.hpp
include/Core/Graphics/Data/ModelMaterial.hpp
include/Core/Graphics/Resource/Shader.hpp
include/Core/Graphics/Resource/ShaderVariant.hpp
include/Core/Graphics/Resource/Mesh.hpp
include/Core/Graphics/Resource/Model.hpp
include/Core/Graphics/Resource/Texture.hpp
include/Core/Graphics/Resource/TextureSampler.hpp
include/Core/Graphics/Resource/Material.hpp
include/Core/Graphics/Resource/GUIWidget.hpp
include/Core/Graphics/Resource/Font.hpp
include/Core/Graphics/Renderers/WorldRenderer.hpp
include/Core/Graphics/Renderers/WorldRendererListener.hpp
include/Core/Graphics/Renderers/DrawCollector.hpp
include/Core/Graphics/Renderers/ShapeRenderer.hpp
include/Core/Graphics/Renderers/PhysicsDebugRenderer.hpp
include/Core/Graphics/Pipeline/Buffer.hpp
include/Core/Graphics/Pipeline/RenderPass.hpp
include/Core/Graphics/Pipeline/DescriptorSet.hpp
include/Core/Graphics/Pipeline/View.hpp

#Meta
include/Core/Meta/ProjectData.hpp

# World
include/Core/World/Entity.hpp
include/Core/World/Component.hpp
include/Core/World/ComponentCache.hpp
include/Core/World/Screen.hpp
include/Core/World/WorldInput.hpp
include/Core/World/EntityWorld.hpp
include/Core/World/EntityTemplate.hpp
include/Core/World/EntityWorldListener.hpp
include/Core/World/CommonWorld.hpp
include/Core/World/Camera.hpp
include/Core/World/Components/CompCamera.hpp
include/Core/World/Components/CompModel.hpp
include/Core/World/Components/CompLight.hpp
include/Core/World/Components/CompWidget.hpp
include/Core/World/Components/CompAudio.hpp
include/Core/World/Animation/AnimationController.hpp
include/Core/World/WorldUtility.hpp

# Physics
include/Core/Physics/PhysicsWorld.hpp
include/Core/Physics/PhysicsContactListener.hpp
include/Core/Physics/PhysicsMaterial.hpp
include/Core/Physics/PhysicsBackend.hpp
include/Core/Physics/CommonPhysics.hpp
include/Core/Physics/PhysicsLayerFilter.hpp
include/Core/Physics/PhysicsBPLayerInterface.hpp
include/Core/Physics/PhysicsObjectBPLayerFilter.hpp
include/Core/Physics/PhysicsRayCollector.hpp

# GUI
include/Core/GUI/CommonGUI.hpp
include/Core/GUI/Theme.hpp
include/Core/GUI/Widgets/WidgetManager.hpp
include/Core/GUI/Widgets/WidgetCache.hpp
include/Core/GUI/Widgets/Widget.hpp
include/Core/GUI/Widgets/WidgetUtility.hpp
include/Core/GUI/Widgets/Layout/DirectionalLayout.hpp
include/Core/GUI/Widgets/Layout/Popup.hpp
include/Core/GUI/Widgets/Layout/ForegroundItem.hpp
include/Core/GUI/Widgets/Layout/ScrollArea.hpp
include/Core/GUI/Widgets/Layout/FoldLayout.hpp
include/Core/GUI/Widgets/Layout/GridLayout.hpp
include/Core/GUI/Widgets/Primitives/Text.hpp
include/Core/GUI/Widgets/Primitives/Circle.hpp
include/Core/GUI/Widgets/Primitives/Icon.hpp
include/Core/GUI/Widgets/Primitives/Slider.hpp
include/Core/GUI/Widgets/Primitives/Button.hpp
include/Core/GUI/Widgets/Primitives/Checkbox.hpp
include/Core/GUI/Widgets/Primitives/InputField.hpp
include/Core/GUI/Widgets/Primitives/Dropdown.hpp
include/Core/GUI/Widgets/Primitives/ColorField.hpp
include/Core/GUI/Widgets/Primitives/ColorSlider.hpp
include/Core/GUI/Widgets/Compound/FileMenu.hpp

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
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Dependencies/JoltPhysics-5.2.0)

#--------------------------------------------------------------------
# Subdirectories & linking
#--------------------------------------------------------------------

target_link_libraries(${PROJECT_NAME} 
	PUBLIC Lina::Common
	PUBLIC Jolt
	PUBLIC OpenAL::OpenAL
)

message("LinaCore -> LinaCommon has been linked.")
message("LinaCore -> Jolt Physics has been linked.")
message("LinaCore -> OpenAL has been linked.")

if(APPLE)
	target_link_libraries(${PROJECT_NAME}
		PUBLIC "-framework AppKit"
		PUBLIC "-framework Foundation"
		PUBLIC "-framework QuartzCore"
		PUBLIC "-framework UniformTypeIdentifiers"
	)
endif()


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
