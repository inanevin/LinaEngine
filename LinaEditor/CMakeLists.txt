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
src/Core/GUILayer.cpp
src/Core/EditorCommon.cpp
src/Core/GizmoLayer.cpp
src/Core/SplashScreen.cpp

#Drawers
src/Drawers/EntityDrawer.cpp

#Modals
src/Modals/SelectComponentModal.cpp

# Panels
src/Panels/EditorPanel.cpp
src/Panels/ECSPanel.cpp
src/Panels/MaterialPanel.cpp
src/Panels/ResourcesPanel.cpp
src/Panels/ScenePanel.cpp
src/Panels/PropertiesPanel.cpp
src/Panels/LogPanel.cpp
src/Panels/HeaderPanel.cpp


# World
src/Levels/Example1Level.cpp
src/Levels/GroundCubeSystem.cpp

# Utility
src/Utility/EditorUtility.cpp

# Widgets
src/Widgets/MenuButton.cpp
src/Widgets/WidgetsUtility.cpp

# ImGui
src/imgui/imgui_impl_opengl3.cpp
src/imgui/imgui_impl_glfw.cpp
src/imgui/imgui.cpp
src/imgui/imgui_demo.cpp
src/imgui/imgui_draw.cpp
src/imgui/imgui_widgets.cpp
src/imgui/imguizmo/ImCurveEdit.cpp
src/imgui/imguizmo/ImGradient.cpp
src/imgui/imguizmo/ImGuizmo.cpp
src/imgui/imguizmo/ImSequencer.cpp
src/imgui/ImGuiFileDialogue/ImGuiFileDialog.cpp

)

set(LINAEDITOR_HEADERS

# Core
include/Core/GUILayer.hpp
include/Core/EditorCommon.hpp
include/Core/GizmoLayer.hpp
include/Core/SplashScreen.hpp

# World
include/Levels/Example1Level.hpp
include/Levels/GroundCubeSystem.hpp
include/Levels/GroundCubeComponent.hpp

#Drawers
include/Drawers/EntityDrawer.hpp

# Modals
include/Modals/SelectComponentModal.hpp

# Panels
include/Panels/EditorPanel.hpp
include/Panels/ECSPanel.hpp
include/Panels/MaterialPanel.hpp
include/Panels/ResourcesPanel.hpp
include/Panels/ScenePanel.hpp
include/Panels/PropertiesPanel.hpp
include/Panels/LogPanel.hpp
include/Panels/HeaderPanel.hpp

# Utility
include/Utility/EditorUtility.hpp

# Widgets
include/Widgets/MenuButton.hpp
include/Widgets/WidgetsUtility.hpp

# ImGui
include/imgui/dirent.h
include/imgui/imconfig.h
include/imgui/imgui.h
include/imgui/imgui_impl_glfw.h
include/imgui/imgui_impl_opengl3.h
include/imgui/imgui_internal.h
include/imgui/imstb_rectpack.h
include/imgui/imstb_textedit.h
include/imgui/imstb_truetype.h
include/imgui/imguizmo/ImCurveEdit.h
include/imgui/imguizmo/ImGradient.h
include/imgui/imguizmo/ImGuizmo.h
include/imgui/imguizmo/ImSequencer.h
include/imgui/ImGuiFileDialogue/ImGuiFileDialog.h
include/imgui/ImGuiFileDialogue/ImGuiFileDialogConfig.h


CMakeLists.txt

)

#--------------------------------------------------------------------
# Create executable project
#--------------------------------------------------------------------
add_executable(${PROJECT_NAME} ${LINAEDITOR_SOURCES} ${LINAEDITOR_HEADERS})

#--------------------------------------------------------------------
# Options & Definitions
#--------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/glfw/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/glad/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/IconFontCPPHeaders/include)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

option(LINA_CLIENT_ENABLE_LOGGING "Enables console logging" ON)

if(LINA_CLIENT_ENABLE_LOGGING)
	target_compile_definitions(${PROJECT_NAME} PRIVATE LINA_CLIENT_ENABLE_LOGGING=1)
endif()

#--------------------------------------------------------------------
# Links
#--------------------------------------------------------------------
target_link_libraries(${PROJECT_NAME} 
PRIVATE Lina::Engine
PRIVATE ${CMAKE_SOURCE_DIR}/vendor/glfw/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/glfw3.lib
PRIVATE ${CMAKE_SOURCE_DIR}/vendor/glad/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/glad.lib
)



#--------------------------------------------------------------------
# Build Commands
#--------------------------------------------------------------------

#add_custom_command(TARGET ${PROJECT_NAME}
#POST_BUILD
#COMMAND ${CMAKE_COMMAND}
#	-Dsrc_dir="${CMAKE_CURRENT_SOURCE_DIR}/Resources"
#    -Ddst_dir="${CMAKE_BINARY_DIR}/Sandbox/Resources"
#    -P "${CMAKE_SOURCE_DIR}/CMake/UpdateDirectory.cmake"
#)


#Copy PDBs
#add_custom_command(
#TARGET ${PROJECT_NAME}
#PRE_BUILD
#COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/LinaEngine/vendor/glfw/lib/$<CONFIGURATION>/glfw.pdb" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>")

#Copy PDBs
#add_custom_command(
#TARGET ${PROJECT_NAME}
#PRE_BUILD
#COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/LinaEngine/vendor/glad/lib/$<CONFIGURATION>/glad.pdb" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>")

# Copy engine resources to project solution directory to run within IDE.
add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/LinaEngine/resources" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/resources/")

# Copy project resources to binary directory
add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/resources" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/resources")

# Copy project resources to project solution directory to run within IDE.
add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/resources" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/resources/")

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