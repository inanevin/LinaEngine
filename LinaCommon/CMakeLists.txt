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

    src/Core/Layer.cpp
    src/Core/LayerStack.cpp
    src/Core/Timer.cpp
	
	src/PackageManager/Generic/cmwc4096.cpp
	src/PackageManager/Generic/GenericMemory.cpp
	
	#Utility
	src/Utility/UtilityFunctions.cpp
	
	# Log
	src/Log/Log.cpp
	
	#Math
	src/Math/Matrix.cpp
	src/Math/Quaternion.cpp
	src/Math/Transformation.cpp
	src/Math/Vector.cpp
	src/Math/Color.cpp
	
	# Memory
	src/Memory/StackAllocator.cpp
	src/Memory/FrameAllocator.cpp
	src/Memory/PoolAllocator.cpp
	src/Memory/FreeListAllocator.cpp
	
	#src/Utility/EngineUtility.cpp
	#src/Utility/FileUtility.cpp
	#src/Utility/StringID.cpp
)

#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(LINACOMMON_HEADERS

	#CORE
	include/Core/Common.hpp
	include/Core/Environment.hpp
	include/Core/Internal.hpp
	include/Core/LinaArray.hpp
	include/Core/SizeDefinitions.hpp
	include/Core/Layer.hpp
	include/Core/LayerStack.hpp
	include/Core/LinaAPI.hpp
	include/Core/Timer.hpp
	
	# Job System
	include/JobSystem/JobSystem.hpp
	
	# PAM
	include/PackageManager/Generic/cmwc4096.hpp
	include/PackageManager/Generic/GenericMemory.hpp
	include/PackageManager/PAMMemory.hpp
	include/PackageManager/PAMSIMD.hpp

	# Memory
	include/Memory/MemoryAllocator.hpp
	include/Memory/StackAllocator.hpp
	include/Memory/MemoryUtility.hpp
	include/Memory/FrameAllocator.hpp
	include/Memory/PoolAllocator.hpp
	include/Memory/StackLinkedList.hpp
	include/Memory/SinglyLinkedList.hpp
	include/Memory/FreeListAllocator.hpp	
	
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
	
	#include/Utility/EngineUtility.hpp
	#include/Utility/FileUtility.hpp
	#include/Utility/StringID.hpp
)

#--------------------------------------------------------------------
# Define the library & create an alias
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${LINACOMMON_SOURCES} ${LINACOMMON_HEADERS})
add_library(Lina::Common ALIAS ${PROJECT_NAME}) 

#--------------------------------------------------------------------
# Config & Options & Compile Definitions
#--------------------------------------------------------------------
include(../CMake/ProjectSettings.cmake)

#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glm/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/fmt/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/cereal/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/taskflow/include)



#--------------------------------------------------------------------
# Subdirectories & linking
#--------------------------------------------------------------------

target_link_libraries(${PROJECT_NAME} 
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glm/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/glm_static.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/fmt/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/fmt.lib
)

#--------------------------------------------------------------------
# Packages
#--------------------------------------------------------------------


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


