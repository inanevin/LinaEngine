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
project(LinaECS)
set(CMAKE_CXX_STANDARD 17)
set(BUILD_SHARED_LIBS ON)
#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------
set (LINAECS_SOURCES

	# ECS 
	src/ECS/Systems/MovementControlSystem.cpp
	src/ECS/Systems/MotionSystem.cpp
	src/ECS/Systems/CubeChunkSystem.cpp
	src/ECS/Systems/CubeChunkRenderSystem.cpp
	src/ECS/Interactions/TestInteraction.cpp
	src/ECS/ECSComponent.cpp
	src/ECS/ECSSystem.cpp
	src/ECS/EntityComponentSystem.cpp	
)

#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(LINAECS_HEADERS


	#ECS
	include/ECS/Components/MovementControlComponent.hpp
	include/ECS/Components/TransformComponent.hpp
	include/ECS/Components/MotionComponent.hpp
	include/ECS/Components/CubeChunkComponent.hpp
	include/ECS/Components/ColliderComponent.hpp
	include/ECS/Systems/MovementControlSystem.hpp
	include/ECS/Systems/MotionSystem.hpp
	include/ECS/Systems/CubeChunkSystem.hpp
	include/ECS/Systems/CubeChunkRenderSystem.hpp
	include/ECS/Interactions/TestInteraction.hpp
	include/ECS/ECSComponent.hpp
	include/ECS/ECSSystem.hpp
	include/ECS/EntityComponentSystem.hpp
	include/ECS/Utility/MotionIntegrators.hpp
	include/ECS/ECSInteraction.hpp
)

#--------------------------------------------------------------------
# Define the library & create an alias
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${LINAECS_SOURCES} ${LINAECS_HEADERS})
add_library(Lina::ECS ALIAS ${PROJECT_NAME}) 

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

if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_PLATFORM_WINDOWS=1)
endif()
if(APPLE)
    # for MacOS X or iOS, watchOS, tvOS (since 3.10.3)
    target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_PLATFORM_APPLE=1)
endif()
if(UNIX AND NOT APPLE)
    # for Linux, BSD, Solaris, Minix
    target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_PLATFORM_UNIX=1)
endif()

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

include(../CMake/GLGraphicsDefinitions.cmake)

# Language standard
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

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
	foreach(source IN LISTS LINAECS_HEADERS LINAECS_SOURCES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${LinaECS_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
endif()


