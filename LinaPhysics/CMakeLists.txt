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
	src/Physics/PhysicsEngine.cpp
	src/Physics/PhysicsGizmoDrawer.cpp
	src/ECS/Systems/RigidbodySystem.cpp

)

#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(LINAPHYSICS_HEADERS
	
	#Physics
	include/Physics/PhysicsEngine.hpp
	include/Physics/PhysicsGizmoDrawer.hpp
	include/ECS/Components/RigidbodyComponent.hpp
	include/ECS/Systems/RigidbodySystem.hpp
)


if(NOT LINA_SHARED_PHYSICS)
	add_compile_definitions(LINAPHYSICS_STATIC_DEFINE)
	set(BUILD_SHARED_LIBS OFF)
else (LINA_SHARED_PHYSICS)
	set(BUILD_SHARED_LIBS ON)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

#--------------------------------------------------------------------
# Define the library & create an alias
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${LINAPHYSICS_SOURCES} ${LINAPHYSICS_HEADERS})
add_library(Lina::Physics ALIAS ${PROJECT_NAME}) 

#--------------------------------------------------------------------
# Config & Options & Compile Definitions
#--------------------------------------------------------------------



set_target_properties(
    ${PROJECT_NAME}
      PROPERTIES 
        CXX_STANDARD 17 
        CXX_STANDARD_REQUIRED YES 
        CXX_EXTENSIONS NO
)


if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
   target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_COMPILER_CLANG=1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
   target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_COMPILER_GNU=1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
   target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_COMPILER_MSVC=1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Intel")
   target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_COMPILER_INTEL=1)
else()
   target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_COMPILER_OTHER=1)
endif()

if(LINA_CORE_ENABLE_LOGGING)
	target_compile_definitions(${PROJECT_NAME} PRIVATE LINA_CORE_ENABLE_LOGGING=1)
endif()

if(LINA_ENABLE_EDITOR)
	target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_EDITOR=1)
endif()

if(LINA_ENABLE_TIMEPROFILING)
	target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_ENABLE_TIMEPROFILING=1)
endif()

#--------------------------------------------------------------------
# Build Type Config
#--------------------------------------------------------------------

if ( CMAKE_BUILD_TYPE STREQUAL "" )
	# CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
	# differentiation between debug and release builds.
	set(
		CMAKE_BUILD_TYPE
			"Debug"
		CACHE STRING
			"Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) \"Debug\" \"Release\" \"MinSizeRel\" \"RelWithDebInfo\"."
		FORCE
	)
	
	set(LINAENGINE_RUNTIME_COPY_DIR "Debug")
endif ( CMAKE_BUILD_TYPE STREQUAL "" )

if ( CMAKE_BUILD_TYPE STREQUAL "Release" )
    target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_RELEASE=1)
	set(LINAENGINE_RUNTIME_COPY_DIR "Release")
endif ( CMAKE_BUILD_TYPE STREQUAL "Release" )

if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_DEBUG=1)
	set(LINAENGINE_RUNTIME_COPY_DIR "Debug")
endif ( CMAKE_BUILD_TYPE STREQUAL "Debug" )

if ( CMAKE_BUILD_TYPE STREQUAL "MinSizeRel" )
    target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_MINSIZEREL=1)
	set(LINAENGINE_RUNTIME_COPY_DIR "MinSizeRel")
endif ( CMAKE_BUILD_TYPE STREQUAL "MinSizeRel" )

if ( CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" )
    target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_RELWITHDEBINFO=1)
	set(LINAENGINE_RUNTIME_COPY_DIR "RelWithDebInfo")
endif ( CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" )

#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaCommon/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaECS/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bullet3/include)

if(LINA_GRAPHICS_OPENGL)
	include(../CMake/GLGraphicsDefinitions.cmake)
endif()

# Language standard
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

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
	PRIVATE Lina::ECS
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


