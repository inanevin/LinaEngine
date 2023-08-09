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
project(LinaCommon CXX)


#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------
set (LINACOMMON_SOURCES

	src/Core/Common.cpp
	src/Core/StringID.cpp
	src/Core/ISingleton.cpp
	
	# FileSystem
	src/FileSystem/FileSystem.cpp
	src/FileSystem/FileData.cpp

	# Log
	src/Log/Log.cpp
	
	# Data
	src/Data/Streams.cpp
	src/Data/SyncQueue.cpp
	src/Data/ActionQueue.cpp
	src/Data/DelegateQueue.cpp
	src/Data/String.cpp
	src/Data/CommonData.cpp

	# Events
	src/Event/Event.cpp
	src/Event/ISystemEventListener.cpp
	src/Event/IGameEventListener.cpp
	src/Event/ISystemEventDispatcher.cpp
	src/Event/IGameEventDispatcher.cpp

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
	src/Memory/MemoryAllocatorPool.cpp

	# Profiling
	src/Profiling/Profiler.cpp
	src/Profiling/MemoryTracer.cpp
	
	# Job System
	src/JobSystem/JobSystem.cpp

	# System
	src/System/IPlugin.cpp
	src/System/ISystem.cpp
	src/System/ISubsystem.cpp

	# Platform
	src/Platform/LinaGXIncl.cpp

)


#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(LINACOMMON_HEADERS

	#CORE
	include/Core/Common.hpp
	include/Core/SizeDefinitions.hpp
	include/Core/StringID.hpp
	include/Core/ISingleton.hpp
	include/Core/PlatformTime.hpp
	include/Core/ObjectWrapper.hpp
	
	# FileSystem
	include/FileSystem/FileSystem.hpp
	include/FileSystem/FileData.hpp

	# Job System
	include/JobSystem/JobSystem.hpp

	# Profiler
	include/Profiling/Profiler.hpp
	include/Profiling/MemoryTracer.hpp
	
	# Events
	include/Event/Event.hpp
	include/Event/ISystemEventListener.hpp
	include/Event/IGameEventListener.hpp
	include/Event/ISystemEventDispatcher.hpp
	include/Event/IGameEventDispatcher.hpp

	# Data
	include/Data/Functional.hpp
	include/Data/ActionQueue.hpp
	include/Data/GPUCommand.hpp
	include/Data/SimpleArray.hpp
	include/Data/Array.hpp
	include/Data/Vector.hpp
	include/Data/IntrusiveList.hpp
	include/Data/Map.hpp
	include/Data/MallocAllocator.hpp
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
	include/Data/CommonData.hpp
	include/Data/Bitmask.hpp
	include/Data/Mutex.hpp
	include/Data/Streams.hpp
	include/Data/SyncQueue.hpp
	include/Data/DelegateQueue.hpp

	# Memory
	include/Memory/Memory.hpp
	include/Memory/MemoryAllocatorPool.hpp
	
	# Log
	include/Log/Log.hpp

	# Serialization
	include/Serialization/CommonSerialization.hpp
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

	# System
	include/System/IPlugin.hpp
	include/System/ISystem.hpp
	include/System/ISubsystem.hpp

	# Platform
	include/Platform/LinaGXIncl.hpp
)

if(WIN32)

set (LINACOMMON_PLATFORM_HEADERS
include/Platform/Win32/Win32PlatformTime.hpp
include/Platform/Win32/Win32WindowsInclude.hpp
)

set (LINACOMMON_PLATFORM_SOURCES 
src/Platform/Win32/Win32PlatformTime.cpp
)

endif()

#--------------------------------------------------------------------
# Define the library & create an alias
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${LINACOMMON_SOURCES} ${LINACOMMON_HEADERS} ${LINACOMMON_PLATFORM_HEADERS} ${LINACOMMON_PLATFORM_SOURCES})
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
	
)


#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "LinaEngine")

#--------------------------------------------------------------------
# Subdirectories & linking
#--------------------------------------------------------------------

target_link_libraries(${PROJECT_NAME}
    PUBLIC Lina::GX
)

include(../Dependencies/phmap/link.cmake)
include(../Dependencies/taskflow/link.cmake)
link_phmap()
link_taskflow()

target_link_libraries(${PROJECT_NAME} 
PUBLIC memory-allocators
PUBLIC lz4
PUBLIC fmt
PUBLIC EASTL
PUBLIC glm_static
)

message("LinaCommon -> memory-allocators has been linked.")
message("LinaCommon -> lz4 has been linked.")
message("LinaCommon -> fmt has been linked.")
message("LinaCommon -> EASTL has been linked.")
message("LinaCommon -> glm has been linked.")

# timeBeginPeriod for adjusting timer precision requires Winmm.lib
if(WIN32)
target_link_libraries(${PROJECT_NAME} 
	PUBLIC Winmm.lib
)
message("LinaCommon -> Winmm Linked.")
endif()


##include (GenerateExportHeader)
##	GENERATE_EXPORT_HEADER (LinaCommon
##	BASE_NAME LinaCommon
##	EXPORT_MACRO_NAME LINACOMMON_API
##	EXPORT_FILE_NAME LinaCommonExports.hpp
##)

#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	foreach(source IN LISTS LINACOMMON_HEADERS LINACOMMON_SOURCES LINACOMMON_PLATFORM_HEADERS LINACOMMON_PLATFORM_SOURCES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${LinaCommon_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
endif()


