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


#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------
set (LINACOMMON_SOURCES

	src/Core/Common.cpp
    src/Core/Time.cpp
	src/Core/Clock.cpp
	src/Core/StringID.cpp
	
	# FileSystem
	src/FileSystem/FileSystem.cpp
	src/FileSystem/FileData.cpp

	# Log
	src/Log/Log.cpp
	
	# Data
	src/Data/Streams.cpp
	src/Data/SyncQueue.cpp

	# Events
	src/EventSystem/EventSystem.cpp

	#Math
	src/Math/Matrix.cpp
	src/Math/Quaternion.cpp
	src/Math/Transformation.cpp
	src/Math/Vector.cpp
	src/Math/Rect.cpp
	src/Math/Color.cpp
	src/Math/Frustum.cpp
	src/Math/AABB.cpp
	src/Math/Plane.cpp

	# Serialization
	src/Serialization/Serialization.cpp
	src/Serialization/Endianness.cpp
	src/Serialization/Compressor.cpp
	src/Serialization/ISerializable.cpp
	src/Serialization/StringSerialization.cpp
	
	# Memory
	src/Memory/Memory.cpp
	src/Memory/MemoryManager.cpp

	# Profiling
	src/Profiling/Profiler.cpp
	
	# Job System
	src/JobSystem/JobSystem.cpp
)


#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(LINACOMMON_HEADERS

	#CORE
	include/Core/Common.hpp
	include/Core/SizeDefinitions.hpp
	include/Core/Time.hpp
	include/Core/Clock.hpp
	include/Core/StringID.hpp
	
	# FileSystem
	include/FileSystem/FileSystem.hpp
	include/FileSystem/FileData.hpp

	# Job System
	include/JobSystem/JobSystem.hpp

	# Profiler
	include/Profiling/Profiler.hpp
	
	# Events
	include/EventSystem/EventCommon.hpp
	include/EventSystem/WindowEvents.hpp
	include/EventSystem/InputEvents.hpp
	include/EventSystem/LevelEvents.hpp
	include/EventSystem/GraphicsEvents.hpp
	include/EventSystem/MainLoopEvents.hpp
	include/EventSystem/EntityEvents.hpp
	include/EventSystem/ApplicationEvents.hpp
	include/EventSystem/ResourceEvents.hpp
	include/EventSystem/EventSystem.hpp
	include/EventSystem/WorldEvents.hpp

	# Data
	include/Data/Functional.hpp
	include/Data/Action.hpp
	include/Data/SimpleArray.hpp
	include/Data/Array.hpp
	include/Data/Vector.hpp
	include/Data/IntrusiveList.hpp
	include/Data/Map.hpp
	include/Data/List.hpp
	include/Data/Ringbuffer.hpp
	include/Data/IDList.hpp
	include/Data/SList.hpp
	include/Data/HashMap.hpp
	include/Data/Set.hpp
	include/Data/HashSet.hpp
	include/Data/Queue.hpp
	include/Data/PriorityQueue.hpp
	include/Data/Deque.hpp
	include/Data/FixedVector.hpp
	include/Data/FixedMap.hpp
	include/Data/FixedList.hpp
	include/Data/FixedSList.hpp
	include/Data/FixedHashMap.hpp
	include/Data/FixedSet.hpp
	include/Data/String.hpp
	include/Data/DataCommon.hpp
	include/Data/Bitmask.hpp
	include/Data/Mutex.hpp
	include/Data/Streams.hpp
	include/Data/SyncQueue.hpp

	# Memory
	include/Memory/Memory.hpp
	include/Memory/MemoryManager.hpp
	
	# Log
	include/Log/Log.hpp

	# Serialization
	include/Serialization/SerializationCommon.hpp
	include/Serialization/ISerializable.hpp
	include/Serialization/Compressor.hpp
	include/Serialization/Endianness.hpp
	include/Serialization/Serialization.hpp
	include/Serialization/StringSerialization.hpp
	include/Serialization/VectorSerialization.hpp
	include/Serialization/HashMapSerialization.hpp
	include/Serialization/QueueSerialization.hpp
	include/Serialization/SetSerialization.hpp

	# Math
	include/Math/Color.hpp
	include/Math/Math.hpp
	include/Math/Matrix.hpp
	include/Math/Quaternion.hpp
	include/Math/Transformation.hpp
	include/Math/Vector.hpp
	include/Math/Rect.hpp
	include/Math/AABB.hpp
	include/Math/Frustum.hpp
	include/Math/Plane.hpp
)

#--------------------------------------------------------------------
# Define the library & create an alias
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${LINACOMMON_SOURCES} ${LINACOMMON_HEADERS})
add_library(Lina::Common ALIAS ${PROJECT_NAME} ) 

#--------------------------------------------------------------------
# Config & Options & Compile Definitions
#--------------------------------------------------------------------
include(../CMake/ProjectSettings.cmake)

target_precompile_headers(${PROJECT_NAME}
PUBLIC
	<algorithm>
	<functional>
	<chrono>
	<assert.h>
	<xutility>
	<xstring>
	<exception>
	<filesystem>
	<xmemory>
	<type_traits>
	include/Math/Matrix.hpp
	include/Math/Vector.hpp
	include/Math/Quaternion.hpp
	include/Data/String.hpp
	include/Data/HashMap.hpp
	include/Data/Vector.hpp
	include/JobSystem/JobSystem.hpp
	include/Serialization/Endianness.hpp
	include/Data/Streams.hpp
	include/Log/Log.hpp
	include/Memory/Memory.hpp
	
)

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
include(../_Dependencies/eastl/link.cmake)
include(../_Dependencies/phmap/link.cmake)
include(../_Dependencies/taskflow/link.cmake)
include(../_Dependencies/memallocators/link.cmake)
include(../_Dependencies/lz4/link.cmake)
link_glm()
link_fmt()
link_eastl()
link_phmap()
link_taskflow()
link_memallocators()
link_lz4()

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


