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
project(LinaCommon CXX)
set(CMAKE_CXX_STANDARD 17)	
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------
set (LINACOMMON_SOURCES

	src/Core/Log.cpp
	src/Core/Timer.cpp
			
	src/EventSystem/EventSystem.cpp
	
	src/Memory/StackAllocator.cpp
	src/Memory/FrameAllocator.cpp
	src/Memory/PoolAllocator.cpp
	src/Memory/FreeListAllocator.cpp
	
	src/Math/Vector.cpp
	src/Math/Quaternion.cpp
	src/Math/Matrix.cpp
	
	src/Utility/EngineUtility.cpp
	src/Utility/FileUtility.cpp
	src/Utility/StringId.cpp
	
	src/ECS/ECS.cpp

)

#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(LINACOMMON_HEADERS

	include/Core/Log.hpp
	include/Core/MacroDetection.hpp
	include/Core/Timer.hpp
	include/Core/Common.hpp
	
	include/EventSystem/EventSystem.hpp
	include/EventSystem/Events.hpp
	include/EventSystem/EventCommon.hpp
	
	include/Profiling/Profiler.hpp
	
	include/JobSystem/JobSystem.hpp
	
	include/Memory/MemoryAllocator.hpp
	include/Memory/StackAllocator.hpp
	include/Memory/MemoryUtility.hpp
	include/Memory/FrameAllocator.hpp
	include/Memory/PoolAllocator.hpp
	include/Memory/StackLinkedList.hpp
	include/Memory/SinglyLinkedList.hpp
	include/Memory/FreeListAllocator.hpp	
	
	include/Math/Math.hpp
	include/Math/Vector.hpp
	include/Math/Color.hpp
	include/Math/Quaternion.hpp
	include/Math/Matrix.hpp
	
	include/Utility/EngineUtility.hpp
	include/Utility/FileUtility.hpp
	include/Utility/StringId.hpp
	
	include/ECS/Components/ECTransform.hpp
	include/ECS/ECS.hpp
	
	include/linacommon_export.h
)

#--------------------------------------------------------------------
# Define the library & create an alias
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} SHARED ${LINACOMMON_SOURCES} ${LINACOMMON_HEADERS})
add_library(Lina::Common ALIAS ${PROJECT_NAME}) 
include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})

#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/entt/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glm/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/fmt/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glfw/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/cereal/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/taskflow/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/easy_profiler/include)


#--------------------------------------------------------------------
# Subdirectories & linking
#--------------------------------------------------------------------

set(CMAKE_DEBUG_POSTFIX "_d")
set(CMAKE_RELEASE_POSTFIX "")

target_link_libraries(${PROJECT_NAME} 
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glm/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/glm_static.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/fmt/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/fmt.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glfw/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/glfw3dll.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/easy_profiler/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/easy_profiler.lib
)


#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	foreach(source IN LISTS LINACOMMON_HEADERS LINACOMMON_SOURCES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${LinaCommon_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
endif()


