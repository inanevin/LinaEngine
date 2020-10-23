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
	src/Utility/Math/Matrix.cpp
	src/Utility/Math/Quaternion.cpp
	src/Utility/Math/Transformation.cpp
	src/Utility/Math/Vector.cpp
	src/Utility/Math/Color.cpp
	src/Utility/UtilityFunctions.cpp
	src/Utility/Log.cpp
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
	
	# PAM
	include/PackageManager/Generic/cmwc4096.hpp
	include/PackageManager/Generic/GenericMemory.hpp
	include/PackageManager/PAMMemory.hpp
	include/PackageManager/PAMSIMD.hpp


	# Utility
	include/Utility/Math/Color.hpp
	include/Utility/Math/Math.hpp
	include/Utility/Math/Matrix.hpp
	include/Utility/Math/Quaternion.hpp
	include/Utility/Math/Transformation.hpp
	include/Utility/Math/Vector.hpp
	include/Utility/Log.hpp
	include/Utility/UtilityFunctions.hpp

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


