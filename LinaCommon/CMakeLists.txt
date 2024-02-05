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
project(LinaCommon)
set(CMAKE_CXX_STANDARD 17)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------
set (SOURCES

	src/Common.cpp
	src/StringID.cpp
	
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
	src/Event/SystemEventListener.cpp
	src/Event/GameEventListener.cpp
	src/Event/SystemEventDispatcher.cpp
	src/Event/GameEventDispatcher.cpp

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
	src/System/Plugin.cpp
	src/System/System.cpp
	src/System/Subsystem.cpp

	# Platform
	src/Platform/LinaGXIncl.cpp

)


#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(HEADERS

	#CORE
	include/Common/Common.hpp
	include/Common/SizeDefinitions.hpp
	include/Common/StringID.hpp
	include/Common/ObjectWrapper.hpp
	
	# FileSystem
	include/Common/FileSystem/FileSystem.hpp
	include/Common/FileSystem/FileData.hpp

	# Job System
	include/Common/JobSystem/JobSystem.hpp

	# Profiler
	include/Common/Profiling/Profiler.hpp
	include/Common/Profiling/MemoryTracer.hpp
	
	# Events
	include/Common/Event/Event.hpp
	include/Common/Event/SystemEventListener.hpp
	include/Common/Event/GameEventListener.hpp
	include/Common/Event/SystemEventDispatcher.hpp
	include/Common/Event/GameEventDispatcher.hpp

	# Data
	include/Common/Data/Functional.hpp
	include/Common/Data/ActionQueue.hpp
	include/Common/Data/GPUCommand.hpp
	include/Common/Data/SimpleArray.hpp
	include/Common/Data/Array.hpp
	include/Common/Data/Vector.hpp
	include/Common/Data/IntrusiveList.hpp
	include/Common/Data/Map.hpp
	include/Common/Data/MallocAllocator.hpp
	include/Common/Data/List.hpp
	include/Common/Data/Ringbuffer.hpp
	include/Common/Data/IDList.hpp
	include/Common/Data/SList.hpp
	include/Common/Data/HashMap.hpp
	include/Common/Data/Set.hpp
	include/Common/Data/HashSet.hpp
	include/Common/Data/Queue.hpp
	include/Common/Data/PriorityQueue.hpp
	include/Common/Data/Deque.hpp
	include/Common/Data/FixedVector.hpp
	include/Common/Data/FixedMap.hpp
	include/Common/Data/FixedList.hpp
	include/Common/Data/FixedSList.hpp
	include/Common/Data/FixedHashMap.hpp
	include/Common/Data/FixedSet.hpp
	include/Common/Data/String.hpp
	include/Common/Data/CommonData.hpp
	include/Common/Data/Bitmask.hpp
	include/Common/Data/Mutex.hpp
	include/Common/Data/Streams.hpp
	include/Common/Data/SyncQueue.hpp
	include/Common/Data/DelegateQueue.hpp

	# Memory
	include/Common/Memory/Memory.hpp
	include/Common/Memory/MemoryAllocatorPool.hpp
	
	# Log
	include/Common/Log/Log.hpp

	# Serialization
	include/Common/Serialization/CommonSerialization.hpp
	include/Common/Serialization/Compressor.hpp
	include/Common/Serialization/Endianness.hpp
	include/Common/Serialization/Serialization.hpp
	include/Common/Serialization/StringSerialization.hpp
	include/Common/Serialization/VectorSerialization.hpp
	include/Common/Serialization/HashMapSerialization.hpp
	include/Common/Serialization/QueueSerialization.hpp
	include/Common/Serialization/SetSerialization.hpp

	# Math
	include/Common/Math/Color.hpp
	include/Common/Math/Math.hpp
	include/Common/Math/Matrix.hpp
	include/Common/Math/Quaternion.hpp
	include/Common/Math/Transformation.hpp
	include/Common/Math/Vector.hpp
	include/Common/Math/Rect.hpp
	include/Common/Math/AABB.hpp
	include/Common/Math/Frustum.hpp
	include/Common/Math/Plane.hpp

	# System
	include/Common/System/Plugin.hpp
	include/Common/System/System.hpp
	include/Common/System/Subsystem.hpp

	# Platform
	include/Common/Platform/LinaGXIncl.hpp
	include/Common/Platform/LinaVGIncl.hpp
    include/Common/Platform/PlatformTime.hpp
	include/Common/Platform/PlatformInclude.hpp
)

if(WIN32)

set (PLATFORM_HEADERS
)

set (PLATFORM_SOURCES 
src/Platform/Win32/Win32PlatformTime.cpp
)

endif()

if(APPLE)

set (PLATFORM_HEADERS
)

set (PLATFORM_SOURCES 
src/Platform/Apple/ApplePlatformTime.mm
)

endif()

#--------------------------------------------------------------------
# Define the library & create an alias
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${PLATFORM_HEADERS} ${PLATFORM_SOURCES})
add_library(Lina::Common ALIAS ${PROJECT_NAME}) 

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
	<string>
	<exception>
	<filesystem>
	<memory>
	<type_traits>
	include/Common/Math/Matrix.hpp
	include/Common/Math/Vector.hpp
	include/Common/Math/Quaternion.hpp
	include/Common/Data/String.hpp
	include/Common/Data/HashMap.hpp
	include/Common/Data/Vector.hpp
	include/Common/JobSystem/JobSystem.hpp
	include/Common/Serialization/Endianness.hpp
	include/Common/Data/Streams.hpp
	include/Common/Log/Log.hpp
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
	PUBLIC Lina::VG
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


#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
include(../CMake/CommonFunctions.cmake)
group_sources(HEADERS SOURCES PLATFORM_HEADERS PLATFORM_SOURCES)
