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
src/Core/EditorApplication.cpp
src/Core/EditorResourcesRegistry.cpp
src/Core/Editor.cpp
src/Core/Theme.cpp
src/EditorEntryPoint.cpp
src/GUI/Widgets/Widget.cpp
src/GUI/Widgets/WidgetProgressBar.cpp
src/GUI/Utility/GUIUtility.cpp
src/GUI/Nodes/Panels/GUIPanelFactory.cpp
src/GUI/Nodes/Panels/GUINodePanel.cpp
src/GUI/Nodes/Panels/GUINodePanelResourceViewer.cpp
src/GUI/Nodes/Panels/GUINodePanelEntities.cpp
src/GUI/Nodes/Panels/GUINodePanelHierarchy.cpp
src/GUI/Nodes/Panels/GUINodePanelContentBrowser.cpp
src/GUI/Nodes/Panels/GUINodePanelProperties.cpp
src/GUI/Nodes/Panels/GUINodePanelLevel.cpp
src/GUI/Nodes/GUINode.cpp
src/GUI/Nodes/Docking/GUINodeDockArea.cpp
src/GUI/Nodes/Docking/GUINodeDockPreview.cpp
src/GUI/Nodes/Custom/GUINodeTitleSection.cpp
src/GUI/Nodes/Custom/GUINodeTopPanel.cpp
src/GUI/Nodes/Custom/GUINodeCustomLogo.cpp
src/GUI/Nodes/Widgets/GUINodeFileMenu.cpp
src/GUI/Nodes/Widgets/GUINodeButton.cpp
src/GUI/Nodes/Widgets/GUINodeTooltip.cpp
src/GUI/Nodes/Widgets/GUINodeTab.cpp
src/GUI/Nodes/Widgets/GUINodeTabArea.cpp
src/GUI/Nodes/Widgets/GUINodeWindowButtons.cpp
src/GUI/Nodes/Layouts/GUINodeDivisible.cpp
src/GUI/Nodes/Layouts/GUINodeDivider.cpp
src/GUI/Nodes/Layouts/GUINodeLayoutHorizontal.cpp
src/GUI/Nodes/Layouts/GUINodeLayoutVertical.cpp
src/GUI/Drawers/GUIDrawerBase.cpp
src/GUI/Drawers/GUIDrawerMainWindow.cpp
src/GUI/Drawers/GUIDrawerChildWindow.cpp
src/GUI/Drawers/GUIDrawerSplashScreen.cpp
src/GUI/Drawers/GUIDrawerPayload.cpp
src/GUI/EditorPayloadManager.cpp
)


set(LINAEDITOR_HEADERS

# Core
include/GUI/Nodes/Panels/GUINodePanelResourceViewer.hpp
include/GUI/Nodes/Panels/GUINodePanelEntities.hpp
include/GUI/Nodes/Panels/GUINodePanelHierarchy.hpp
include/GUI/Nodes/Panels/GUINodePanelContentBrowser.hpp
include/GUI/Nodes/Panels/GUINodePanelProperties.hpp
include/GUI/Nodes/Panels/GUINodePanelLevel.hpp
include/GUI/Nodes/Panels/GUINodePanel.hpp
include/GUI/Nodes/Panels/GUIPanelFactory.hpp
include/GUI/Nodes/GUINode.hpp
include/GUI/Nodes/Docking/GUINodeDockArea.hpp
include/GUI/Nodes/Docking/GUINodeDockPreview.hpp
include/GUI/Nodes/Custom/GUINodeTitleSection.hpp
include/GUI/Nodes/Custom/GUINodeTopPanel.hpp
include/GUI/Nodes/Custom/GUINodeCustomLogo.hpp
include/GUI/Nodes/Widgets/GUINodeFileMenu.hpp
include/GUI/Nodes/Widgets/GUINodeButton.hpp
include/GUI/Nodes/Widgets/GUINodeTooltip.hpp
include/GUI/Nodes/Widgets/GUINodeTab.hpp
include/GUI/Nodes/Widgets/GUINodeTabArea.hpp
include/GUI/Nodes/Widgets/GUINodeWindowButtons.hpp
include/GUI/Nodes/Layouts/GUINodeDivider.hpp
include/GUI/Nodes/Layouts/GUINodeDivisible.hpp
include/GUI/Nodes/Layouts/GUINodeLayoutHorizontal.hpp
include/GUI/Nodes/Layouts/GUINodeLayoutVertical.hpp
include/GUI/Drawers/GUIDrawerBase.hpp
include/GUI/Drawers/GUIDrawerSplashScreen.hpp
include/GUI/Drawers/GUIDrawerMainWindow.hpp
include/GUI/Drawers/GUIDrawerChildWindow.hpp
include/GUI/Drawers/GUIDrawerPayload.hpp
include/GUI/Widgets/WidgetProgressBar.hpp
include/GUI/Widgets/Widget.hpp
include/GUI/Utility/GUIUtility.hpp
include/GUI/EditorPayloadManager.hpp
include/Core/EditorApplication.hpp
include/Core/EditorCommon.hpp
include/Core/Editor.hpp
include/Core/Theme.hpp
include/Core/EditorResourcesRegistry.hpp

)

#--------------------------------------------------------------------
# Create executable project
#--------------------------------------------------------------------
if(WIN32)
	add_executable(${PROJECT_NAME} WIN32 ${LINAEDITOR_SOURCES} ${LINAEDITOR_HEADERS})
else()
	add_executable(${PROJECT_NAME} ${LINAEDITOR_SOURCES} ${LINAEDITOR_HEADERS})
endif()

#--------------------------------------------------------------------
# Config & Options & Compile Definitions
#--------------------------------------------------------------------
include(../CMake/ProjectSettings.cmake)

#--------------------------------------------------------------------
# Include & Link
#--------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaCore/include)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Lina")


#--------------------------------------------------------------------
# Links
#--------------------------------------------------------------------
target_link_libraries(${PROJECT_NAME} 
PUBLIC Lina::Core
)

include(../CMake/EditorSandboxCommands.cmake)
include(../_Dependencies/linavg/link.cmake)
link_linavg()

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