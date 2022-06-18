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

	src/Core/Common.cpp
    src/Core/Timer.cpp
	
	#Utility
	src/Utility/UtilityFunctions.cpp
	
	# Log
	src/Log/Log.cpp
	
	# Events
	src/EventSystem/EventSystem.cpp
	
	#Math
	src/Math/Matrix.cpp
	src/Math/Quaternion.cpp
	src/Math/Transformation.cpp
	src/Math/Vector.cpp
	src/Math/Color.cpp
	src/Math/Frustum.cpp
	src/Math/AABB.cpp
	src/Math/Plane.cpp
	
	# Memory
	src/Memory/StackAllocator.cpp
	src/Memory/FrameAllocator.cpp
	src/Memory/PoolAllocator.cpp
	src/Memory/FreeListAllocator.cpp
	src/Memory/Memory.cpp
	
	#src/Utility/FileUtility.cpp
	src/Utility/StringID.cpp
	src/Utility/FileWatcher.cpp
	
	# ECS 
	src/ECS/SystemList.cpp
	src/ECS/System.cpp
	src/ECS/Registry.cpp
	src/ECS/Components/EntityDataComponent.cpp
	src/ECS/Components/PhysicsComponent.cpp
	
	# Reosurces
	src/Resources/ResourceStorage.cpp

)


#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(LINACOMMON_HEADERS

	#CORE
	include/Core/CommonApplication.hpp
	include/Core/CommonWindow.hpp
	include/Core/CommonInput.hpp
	include/Core/CommonPhysics.hpp
	include/Core/CommonResources.hpp
	include/Core/CommonECS.hpp
	include/Core/CommonUtility.hpp
	include/Core/CommonReflection.hpp
	include/Core/SizeDefinitions.hpp
	include/Core/Timer.hpp
	include/Core/PlatformMacros.hpp
	
	# Job System
	include/JobSystem/JobSystem.hpp

	# Profiler
	include/Profiling/Profiler.hpp
	
	# Memory
	include/Memory/MemoryAllocator.hpp
	include/Memory/StackAllocator.hpp
	include/Memory/MemoryUtility.hpp
	include/Memory/FrameAllocator.hpp
	include/Memory/PoolAllocator.hpp
	include/Memory/StackLinkedList.hpp
	include/Memory/SinglyLinkedList.hpp
	include/Memory/FreeListAllocator.hpp	
	include/Memory/Memory.hpp
	
	# Events
	include/EventSystem/EventCommon.hpp
	include/EventSystem/PhysicsEvents.hpp
	include/EventSystem/WindowEvents.hpp
	include/EventSystem/InputEvents.hpp
	include/EventSystem/LevelEvents.hpp
	include/EventSystem/GraphicsEvents.hpp
	include/EventSystem/MainLoopEvents.hpp
	include/EventSystem/ApplicationEvents.hpp
	include/EventSystem/ResourceEvents.hpp
	include/EventSystem/ECSEvents.hpp
	include/EventSystem/EventSystem.hpp

	# Data
	include/Data/Vector.hpp
	include/Data/Map.hpp
	include/Data/List.hpp
	include/Data/SList.hpp
	include/Data/HashMap.hpp
	include/Data/Set.hpp
	include/Data/Queue.hpp
	include/Data/Deque.hpp
	include/Data/FixedVector.hpp
	include/Data/FixedMap.hpp
	include/Data/FixedList.hpp
	include/Data/FixedSList.hpp
	include/Data/FixedHashMap.hpp
	include/Data/FixedSet.hpp
	include/Data/String.hpp

	# Log
	include/Log/Log.hpp
	
	# Utility
	include/Utility/UtilityFunctions.hpp

	# Math
	include/Math/Color.hpp
	include/Math/Math.hpp
	include/Math/Matrix.hpp
	include/Math/Quaternion.hpp
	include/Math/Transformation.hpp
	include/Math/Vector.hpp
	include/Math/AABB.hpp
	include/Math/Frustum.hpp
	include/Math/Plane.hpp
	
	#ECS
	include/ECS/Components/EntityDataComponent.hpp
	include/ECS/Components/PhysicsComponent.hpp
	include/ECS/Registry.hpp
	include/ECS/Component.hpp
	include/ECS/System.hpp
	include/ECS/SystemList.hpp
	
	# Resources
	include/Resources/IResource.hpp
	include/Resources/ResourceStorage.hpp
	include/Resources/ResourceHandle.hpp
	
	#include/Utility/FileUtility.hpp
	include/Utility/StringID.hpp
	include/Utility/FileWatcher.hpp
)

#--------------------------------------------------------------------
# Define the library & create an alias
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${LINACOMMON_SOURCES} ${LINACOMMON_HEADERS})
add_library(Lina::Common ALIAS ${PROJECT_NAME}) 
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ON UNITY_BUILD_UNIQUE_ID "LinaCommon")
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD_MODE BATCH UNITY_BUILD_BATCH_SIZE 16)

target_precompile_headers(${PROJECT_NAME}
  PUBLIC
	include/Memory/MemoryAllocator.hpp
	include/Memory/StackAllocator.hpp
	include/Memory/MemoryUtility.hpp
	include/Memory/FrameAllocator.hpp
	include/Memory/PoolAllocator.hpp
	include/Memory/StackLinkedList.hpp
	include/Memory/SinglyLinkedList.hpp
	include/Memory/FreeListAllocator.hpp	
	include/Memory/Memory.hpp
	include/Core/SizeDefinitions.hpp
	include/Core/Timer.hpp
	include/Core/PlatformMacros.hpp
	include/JobSystem/JobSystem.hpp
	include/Profiling/Profiler.hpp
	include/Log/Log.hpp
	include/ECS/Component.hpp
	include/ECS/System.hpp
	include/ECS/SystemList.hpp
	include/Utility/StringID.hpp
	include/Data/Vector.hpp
	include/Data/Map.hpp
	include/Data/List.hpp
	include/Data/SList.hpp
	include/Data/HashMap.hpp
	include/Data/Set.hpp
	include/Data/Queue.hpp
	include/Data/Deque.hpp
	include/Data/FixedVector.hpp
	include/Data/FixedMap.hpp
	include/Data/FixedList.hpp
	include/Data/FixedSList.hpp
	include/Data/FixedHashMap.hpp
	include/Data/FixedSet.hpp
	include/Data/String.hpp
)
#--------------------------------------------------------------------
# Config & Options & Compile Definitions
#--------------------------------------------------------------------
include(../CMake/ProjectSettings.cmake)

#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)

#--------------------------------------------------------------------
# Subdirectories & linking
#--------------------------------------------------------------------

include(../_Dependencies/glm/link.cmake)
include(../_Dependencies/fmt/link.cmake)
include(../_Dependencies/glfw/link.cmake)
include(../_Dependencies/cereal/link.cmake)
include(../_Dependencies/entt/link.cmake)
include(../_Dependencies/eabase/link.cmake)
include(../_Dependencies/eastl/link.cmake)
link_glm()
link_fmt()
link_glfw()
link_cereal()
link_entt()
link_eabase()
link_eastl()

#--------------------------------------------------------------------
# Generate export header for this dll project 
#--------------------------------------------------------------------
# include (GenerateExportHeader)
#	GENERATE_EXPORT_HEADER (LinaCommon
#	BASE_NAME LinaCommon
#	EXPORT_MACRO_NAME LINA_API
#	EXPORT_FILE_NAME Common.hpp
#)

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


