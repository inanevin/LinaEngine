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
project(LinaGUI)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------

set(LINAGUI_SOURCES 

# Core
src/Core/BaseGUIElement.cpp

# Layout
src/Layout/Container.cpp

# ImGui
src/imgui/imgui_impl_opengl3.cpp
src/imgui/imgui_impl_glfw.cpp
src/imgui/imgui.cpp
src/imgui/imgui_demo.cpp
src/imgui/imgui_draw.cpp
src/imgui/imgui_widgets.cpp
src/imgui/imgui_tables.cpp
src/imgui/imguizmo/ImCurveEdit.cpp
src/imgui/imguizmo/ImGradient.cpp
src/imgui/imguizmo/ImGuizmo.cpp
src/imgui/imguizmo/ImSequencer.cpp
src/imgui/implot/implot_demo.cpp
src/imgui/implot/implot.cpp
src/imgui/implot/implot_items.cpp

)

set_source_files_properties( src/imgui/imgui_impl_opengl3.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON )
set_source_files_properties( src/imgui/imgui_impl_glfw.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON )
set_source_files_properties( src/imgui/imgui.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON )
set_source_files_properties( src/imgui/imgui_demo.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON )
set_source_files_properties( src/imgui/imgui_draw.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON )
set_source_files_properties( src/imgui/imgui_widgets.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON )
set_source_files_properties( src/imgui/imgui_tables.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON )
set_source_files_properties( src/imgui/imguizmo/ImCurveEdit.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON )
set_source_files_properties( src/imgui/imguizmo/ImGradient.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON )
set_source_files_properties( src/imgui/imguizmo/ImGuizmo.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON )
set_source_files_properties( src/imgui/imguizmo/ImSequencer.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON )
set_source_files_properties( src/imgui/implot/implot_demo.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON )
set_source_files_properties( src/imgui/implot/implot.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON )
set_source_files_properties( src/imgui/implot/implot_items.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON )

set(LINAGUI_HEADERS

# Core
include/Core/GUICommon.hpp
include/Core/BaseGUIElement.hpp

# Layout
include/Layout/Container.hpp

# ImGui
include/imgui/dirent.h
include/imgui/imconfig.h
include/imgui/imgui.h
include/imgui/imgui_impl_glfw.h
include/imgui/imgui_impl_opengl3.h
include/imgui/imgui_impl_opengl3_loader.h
include/imgui/imgui_internal.h
include/imgui/imstb_rectpack.h
include/imgui/imstb_textedit.h
include/imgui/imstb_truetype.h
include/imgui/imguizmo/ImCurveEdit.h
include/imgui/imguizmo/ImGradient.h
include/imgui/imguizmo/ImGuizmo.h
include/imgui/imguizmo/ImSequencer.h
include/imgui/implot/implot.h
include/imgui/implot/implot_internal.h

CMakeLists.txt

)

#--------------------------------------------------------------------
# Create executable project
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${LINAGUI_SOURCES} ${LINAGUI_HEADERS})
add_library(Lina::GUI ALIAS ${PROJECT_NAME}) 
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ON UNITY_BUILD_UNIQUE_ID "LinaGUI")
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD_MODE BATCH UNITY_BUILD_BATCH_SIZE 16)

#--------------------------------------------------------------------
# Options & Definitions
#--------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/IconFontCPPHeaders/include)

include(../CMake/ProjectSettings.cmake)


#--------------------------------------------------------------------
# Links
#--------------------------------------------------------------------
target_link_libraries(${PROJECT_NAME} 
PRIVATE Lina::Common
PRIVATE Lina::Input
)

#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	foreach(source IN LISTS LINAGUI_HEADERS LINAGUI_SOURCES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${LINAEDITOR_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
endif()