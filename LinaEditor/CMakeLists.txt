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
project(LinaEditor)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------

set(HEADERS

# Core
include/Editor/CommonEditor.hpp
include/Editor/EditorLocale.hpp
include/Editor/Editor.hpp
include/Editor/WindowPanelManager.hpp

# Project
include/Editor/Project/ProjectManager.hpp

# Atlas
include/Editor/Atlas/AtlasManager.hpp

# Graphics
include/Editor/Graphics/WorldRendererExtEditor.hpp

# IO
include/Editor/IO/FileManager.hpp
include/Editor/IO/ThumbnailGenerator.hpp
include/Editor/IO/DirectoryItem.hpp
include/Editor/IO/ExtensionSupport.hpp

# Widgets
include/Editor/Widgets/CommonWidgets.hpp
include/Editor/Widgets/Testbed.hpp
include/Editor/Widgets/DockTestbed.hpp
include/Editor/Widgets/EditorRoot.hpp
include/Editor/Widgets/Compound/SelectableListLayout.hpp
include/Editor/Widgets/Compound/ColorWheelCompound.hpp
include/Editor/Widgets/Compound/Tab.hpp
include/Editor/Widgets/Compound/TabRow.hpp
include/Editor/Widgets/Compound/IconTabs.hpp
include/Editor/Widgets/Compound/WindowBar.hpp
include/Editor/Widgets/Docking/DockArea.hpp
include/Editor/Widgets/Docking/DockBorder.hpp
include/Editor/Widgets/Docking/DockWidget.hpp
include/Editor/Widgets/Docking/DockPreview.hpp
include/Editor/Widgets/Gizmo/Gizmo.hpp
include/Editor/Widgets/Screens/SplashScreen.hpp
include/Editor/Widgets/Popups/ProjectSelector.hpp
include/Editor/Widgets/Popups/NotificationDisplayer.hpp
include/Editor/Widgets/Popups/InfoTooltip.hpp
include/Editor/Widgets/Popups/GenericPopup.hpp
include/Editor/Widgets/Panel/Panel.hpp
include/Editor/Widgets/Panel/PanelResources.hpp
include/Editor/Widgets/Panel/PanelWorld.hpp
include/Editor/Widgets/Panel/PanelPerformance.hpp
include/Editor/Widgets/Panel/PanelEntities.hpp
include/Editor/Widgets/Panel/PanelFactory.hpp
include/Editor/Widgets/FX/LinaLoading.hpp

# Meta
include/Editor/Meta/EditorSettings.hpp
include/Editor/Meta/EditorLayout.hpp
)

set(SOURCES
# Core
src/CommonEditor.cpp
src/EditorLocale.cpp
src/Editor.cpp
src/WindowPanelManager.cpp

# Project
src/Project/ProjectManager.cpp

# Atlas
src/Atlas/AtlasManager.cpp

# Graphics
src/Graphics/WorldRendererExtEditor.cpp

# IO
src/IO/FileManager.cpp
src/IO/ThumbnailGenerator.cpp
src/IO/DirectoryItem.cpp
src/IO/ExtensionSupport.cpp

# Widgets
src/Widgets/CommonWidgets.cpp
src/Widgets/Testbed.cpp
src/Widgets/DockTestbed.cpp
src/Widgets/EditorRoot.cpp
src/Widgets/Compound/ColorWheelCompound.cpp
src/Widgets/Compound/SelectableListLayout.cpp
src/Widgets/Compound/TabRow.cpp
src/Widgets/Compound/Tab.cpp
src/Widgets/Compound/IconTabs.cpp
src/Widgets/Compound/WindowBar.cpp
src/Widgets/Docking/DockWidget.cpp
src/Widgets/Docking/DockBorder.cpp
src/Widgets/Docking/DockArea.cpp
src/Widgets/Docking/DockPreview.cpp
src/Widgets/Gizmo/Gizmo.cpp
src/Widgets/Screens/SplashScreen.cpp
src/Widgets/Popups/ProjectSelector.cpp
src/Widgets/Popups/NotificationDisplayer.cpp
src/Widgets/Popups/InfoTooltip.cpp
src/Widgets/Popups/GenericPopup.cpp
src/Widgets/Panel/Panel.cpp
src/Widgets/Panel/PanelResources.cpp
src/Widgets/Panel/PanelPerformance.cpp
src/Widgets/Panel/PanelWorld.cpp
src/Widgets/Panel/PanelEntities.cpp
src/Widgets/Panel/PanelFactory.cpp
src/Widgets/FX/LinaLoading.cpp

# Meta
src/Meta/EditorSettings.cpp
src/Meta/EditorLayout.cpp
)

#--------------------------------------------------------------------
# Create executable project
#--------------------------------------------------------------------

if(WIN32)
set(ICON ${PROJECT_SOURCE_DIR}/_Resources/${PROJECT_NAME}.rc)
endif()

if(APPLE)
set(ICON ${PROJECT_SOURCE_DIR}/_Resources/${PROJECT_NAME}.icns)
endif()

# WIN32 Console Subsystem
if(WIN32)
	add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS} ${ICON})
else()
	add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES} ${HEADERS} ${ICON})
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER LinaEngine)


#--------------------------------------------------------------------
# Config & Options & Compile Definitions
#--------------------------------------------------------------------
include(${CMAKE_SOURCE_DIR}/CMake/ProjectSettings.cmake)
include(${CMAKE_SOURCE_DIR}/CMake/ExecutableSettings.cmake)

#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaCore/include)
#--------------------------------------------------------------------
# Links
#--------------------------------------------------------------------
target_link_libraries(${PROJECT_NAME} 
PUBLIC Lina::Core
)

#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------

include(${CMAKE_SOURCE_DIR}/CMake/CommonFunctions.cmake)
group_sources(HEADERS SOURCES)
