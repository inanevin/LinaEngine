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
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------
set (LINAGRAPHICS_SOURCES

	#Rendering	
	src/Core/Backend/Vulkan/RenderEngineVulkan.cpp
	src/Core/Backend/Vulkan/VulkanLoader.cpp
	src/Core/Backend/Vulkan/WindowVulkan.cpp
	
	src/Core/Backend/Vulkan/Objects/VulkanSwapchain.cpp	
	src/Core/Backend/Vulkan/Objects/VulkanLogicalDevice.cpp	
	
	src/Core/Backend/Vulkan/Utility/VulkanFunctions.cpp

)

#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(LINAGRAPHICS_HEADERS

	include/Core/Backend/Vulkan/Utility/VulkanFunctions.hpp
	include/Core/Backend/Vulkan/Utility/VulkanFunctionList.inl
	
	include/Core/RenderingBackend.hpp
	include/Core/WindowBackend.hpp
	include/Core/RenderingCommon.hpp
	
	include/Core/Backend/Vulkan/WindowVulkan.hpp
	include/Core/Backend/Vulkan/RenderEngineVulkan.hpp
	include/Core/Backend/Vulkan/VulkanLoader.hpp
	include/Core/Backend/Vulkan/VulkanCommon.hpp

	include/Core/Backend/Vulkan/Objects/VulkanSwapchain.hpp
	include/Core/Backend/Vulkan/Objects/VulkanLogicalDevice.hpp
	


	include/linagraphics_export.h
)	


#--------------------------------------------------------------------
# Define the library & create an alias
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} SHARED ${LINAGRAPHICS_SOURCES} ${LINAGRAPHICS_HEADERS})
add_library(Lina::Graphics ALIAS ${PROJECT_NAME}) 
include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})
#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glfw/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/vulkan/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaCommon/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaResource/include)


#--------------------------------------------------------------------
# Subdirectories & linking
#--------------------------------------------------------------------

target_link_libraries(${PROJECT_NAME} 
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glfw/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/glfw3dll.lib
	PRIVATE Lina::Common
	PRIVATE Lina::Resource
)
	

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


