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


#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------
set (LINAPHYSICS_SOURCES

	#Physics
	src/Core/PhysicsEngine.cpp
	src/Physics/PhysicsGizmoDrawer.cpp
	src/ECS/Systems/RigidbodySystem.cpp

)

#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(LINAPHYSICS_HEADERS
	
	#Physics
	include/Core/PhysicsEngine.hpp
	include/Physics/PhysicsGizmoDrawer.hpp
	include/ECS/Components/RigidbodyComponent.hpp
	include/ECS/Systems/RigidbodySystem.hpp
	include/ECS/Components/AABBComponent.hpp
)


#--------------------------------------------------------------------
# Define the library & create an alias
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${LINAPHYSICS_SOURCES} ${LINAPHYSICS_HEADERS})
add_library(Lina::Physics ALIAS ${PROJECT_NAME}) 

#--------------------------------------------------------------------
# Config & Options & Compile Definitions
#--------------------------------------------------------------------
include(../CMake/ProjectSettings.cmake)


#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaCommon/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/include)


#--------------------------------------------------------------------
# Subdirectories & linking
#--------------------------------------------------------------------

target_link_libraries(${PROJECT_NAME} 
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/Bullet3Common.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/Bullet2FileLoader.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/Bullet3Collision.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/BulletCollision.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/Bullet3Dynamics.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/BulletDynamics.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/Bullet3Geometry.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/Bullet3OpenCL_clew.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/BulletExampleBrowserLib.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/BulletFileLoader.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/BulletInverseDynamics.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/BulletInverseDynamicsUtils.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/BulletSoftBody.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/BulletWorldImporter.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/BulletXmlWorldImporter.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/BussIK.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/clsocket.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/ConvexDecomposition.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/GIMPACTUtils.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/gtest.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/gwen.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/HACD.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/LinearMath.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/OpenGLWindow.lib
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


