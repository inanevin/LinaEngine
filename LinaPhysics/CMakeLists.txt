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
	# src/Core/Backend/Bullet/BulletPhysicsEngine.cpp
	# src/Core/Backend/Bullet/BulletGizmoDrawer.cpp
	src/Core/Backend/PhysX/PhysXPhysicsEngine.cpp
	src/Core/Backend/PhysX/PhysXCooker.cpp

	src/Core/PhysicsCommon.cpp
	src/ECS/Systems/RigidbodySystem.cpp

	src/Physics/PhysicsMaterial.cpp
	src/Physics/Raycast.cpp
)

#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(LINAPHYSICS_HEADERS
	
	#Physics
	# include/Core/Backend/Bullet/BulletPhysicsEngine.hpp
	# include/Core/Backend/Bullet/BulletGizmoDrawer.hpp
	include/Core/Backend/PhysX/PhysXPhysicsEngine.hpp
	include/Core/Backend/PhysX/PhysXCooker.hpp

	include/Core/PhysicsBackend.hpp
	include/Core/PhysicsBackendFwd.hpp
	include/Core/PhysicsCommon.hpp

	include/ECS/Systems/RigidbodySystem.hpp
	
	include/Physics/PhysicsMaterial.hpp
	include/Physics/Raycast.hpp
)


#--------------------------------------------------------------------
# Define the library & create an alias
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${LINAPHYSICS_SOURCES} ${LINAPHYSICS_HEADERS})
add_library(Lina::Physics ALIAS ${PROJECT_NAME}) 
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ON)
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD_MODE BATCH UNITY_BUILD_BATCH_SIZE 16)
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
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/PhysX/include)

#--------------------------------------------------------------------
# Subdirectories & linking
#--------------------------------------------------------------------

target_link_libraries(${PROJECT_NAME} 
#PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/Bullet3Common.lib
#PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/Bullet2FileLoader.lib
#PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/Bullet3Collision.lib
#PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/BulletCollision.lib
#PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/Bullet3Dynamics.lib
#PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/BulletDynamics.lib
#PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/Bullet3Geometry.lib
#PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/Bullet3OpenCL_clew.lib
#PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/BulletExampleBrowserLib.lib
#PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/BulletFileLoader.lib
#PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/BulletInverseDynamics.lib
#PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/BulletInverseDynamicsUtils.lib
#PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/BulletSoftBody.lib
#PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/BulletWorldImporter.lib
#PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/BulletXmlWorldImporter.lib
#PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/BussIK.lib
#PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/clsocket.lib
#PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/ConvexDecomposition.lib
#PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/GIMPACTUtils.lib
#PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/gtest.lib
#PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/gwen.lib
#PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/HACD.lib
#PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/LinearMath.lib
#PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/OpenGLWindow.lib
PUBLIC ${CMAKE_SOURCE_DIR}/vendor/PhysX/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/PhysX_64.lib
PUBLIC ${CMAKE_SOURCE_DIR}/vendor/PhysX/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/PhysXCommon_64.lib
PUBLIC ${CMAKE_SOURCE_DIR}/vendor/PhysX/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/PhysXFoundation_64.lib
PUBLIC ${CMAKE_SOURCE_DIR}/vendor/PhysX/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/PhysXCharacterKinematic_static_64.lib
PUBLIC ${CMAKE_SOURCE_DIR}/vendor/PhysX/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/PhysXExtensions_static_64.lib
PUBLIC ${CMAKE_SOURCE_DIR}/vendor/PhysX/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/PhysXPvdSDK_static_64.lib
PUBLIC ${CMAKE_SOURCE_DIR}/vendor/PhysX/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/PhysXTask_static_64.lib
PUBLIC ${CMAKE_SOURCE_DIR}/vendor/PhysX/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/PhysXVehicle_static_64.lib
PUBLIC ${CMAKE_SOURCE_DIR}/vendor/PhysX/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/PhysXCooking_64.lib
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


