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
project(LinaPhysics)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------
set (LINAPHYSICS_SOURCES

	#Physics
	src/Core/Backend/NvidiaPhysX/PhysicsEnginePhysX.cpp
	src/ECS/Systems/RigidbodySystem.cpp

)

#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(LINAPHYSICS_HEADERS
	
	#Physics
	include/Core/Backend/NvidiaPhysX/PhysicsEnginePhysX.hpp
	include/Core/PhysicsBackend.hpp
	
	include/ECS/Components/RigidbodyComponent.hpp
	include/ECS/Systems/RigidbodySystem.hpp
	include/ECS/Components/AABBComponent.hpp
	
	include/linaphysics_export.h
)


#--------------------------------------------------------------------
# Define the library & create an alias
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} SHARED ${LINAPHYSICS_SOURCES} ${LINAPHYSICS_HEADERS})
add_library(Lina::Physics ALIAS ${PROJECT_NAME}) 
include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})

#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaCommon/include)

#--------------------------------------------------------------------
# Subdirectories & linking
#--------------------------------------------------------------------

target_link_libraries(${PROJECT_NAME} 
	PRIVATE Lina::Common
)
	

#--------------------------------------------------------------------
# Packages
#--------------------------------------------------------------------


#--------------------------------------------------------------------
# Generate export header for this dll project 
#--------------------------------------------------------------------
# include (GenerateExportHeader)
# GENERATE_EXPORT_HEADER (LinaEngine
#    BASE_NAME LinaEngine
#    EXPORT_MACRO_NAME LINA_API
#    EXPORT_FILE_NAME LinaCore.hpp
#)





#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	foreach(source IN LISTS LINAPHYSICS_HEADERS LINAPHYSICS_SOURCES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${LinaPhysics_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
endif()


