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
project(LinaEditor)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------

set(LINAEDITOR_SOURCES 

# Core
src/Core/Editor.cpp
src/Core/EditorRenderer.cpp
src/Core/EditorGUIManager.cpp
src/Core/Theme.cpp
src/Core/ShortcutManager.cpp

# Components
src/Components/EditorFreeLookComponent.cpp

# Resource
src/Resource/EditorResourceLoader.cpp

# GUI
src/GUI/GNode.cpp
src/GUI/GWindow.cpp
src/GUI/GShape.cpp
src/GUI/GText.cpp
src/GUI/GImage.cpp

# Surfaces
src/Surfaces/STopSection.cpp
src/Surfaces/SBottomSection.cpp

# Utility
src/Utility/GUIParseUtility.cpp
)


set(LINAEDITOR_HEADERS

# Core
include/Core/Editor.hpp
include/Core/EditorRenderer.hpp
include/Core/EditorGUIManager.hpp
include/Core/Theme.hpp
include/Core/ShortcutManager.hpp
include/Core/EditorCommon.hpp

# Components
include/Components/EditorFreeLookComponent.hpp

# Resource
include/Resource/EditorResourceLoader.hpp

# GUI
include/GUI/GNode.hpp
include/GUI/GWindow.hpp
include/GUI/GShape.hpp
include/GUI/GText.hpp
include/GUI/GImage.hpp

# Surfaces
include/Surfaces/STopSection.hpp
include/Surfaces/SBottomSection.hpp

# Utility
include/Utility/GUIParseUtility.hpp
)

#--------------------------------------------------------------------
# Create executable project
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${LINAEDITOR_SOURCES} ${LINAEDITOR_HEADERS})
add_library(Lina::Editor ALIAS ${PROJECT_NAME}) 
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ${UNITY_BUILD_VALUE} UNITY_BUILD_UNIQUE_ID "LinaEditor")
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD_MODE BATCH UNITY_BUILD_BATCH_SIZE 16)

#--------------------------------------------------------------------
# Config & Options & Compile Definitions
#--------------------------------------------------------------------
include(../CMake/ProjectSettings.cmake)

#--------------------------------------------------------------------
# Include & Link
#--------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaEngine/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaGUI/include)

include(../CMake/ProjectSettings.cmake)

#--------------------------------------------------------------------
# Links
#--------------------------------------------------------------------
target_link_libraries(${PROJECT_NAME} 
PRIVATE Lina::Engine
PRIVATE Lina::GUI
)

#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	foreach(source IN LISTS LINAEDITOR_HEADERS LINAEDITOR_SOURCES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${LINAEDITOR_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
endif()