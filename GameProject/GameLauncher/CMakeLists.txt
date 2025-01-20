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
cmake_minimum_required (VERSION 3.10...3.31)
project(GameLauncher)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------

set(SOURCES 
src/Main.cpp
)

set(HEADERS
)

#--------------------------------------------------------------------
# Create executable project
#--------------------------------------------------------------------

if(WIN32)
set(ICON ${PROJECT_SOURCE_DIR}/_Resources/${PROJECT_NAME}.rc)
endif()

if(APPLE)
set(ICON ${PROJECT_SOURCE_DIR}/_Resources/${PROJECT_NAME}.icns)
endif()

# WIN32 Console Subsystem
if(WIN32)
	add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS} ${ICON})
else()
	add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES} ${HEADERS} ${ICON})
endif()


#--------------------------------------------------------------------
# Config & Options & Compile Definitions
#--------------------------------------------------------------------
include(CMake/ProjectSettings.cmake)
include(CMake/CommonDefs.cmake)
set(CMAKE_SHARED_LINKER_FLAGS_DEBUGFAST "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUGASAN "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUGPROFILE "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")

#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaCommon/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaCore/include)
target_compile_definitions(${PROJECT_NAME} PUBLIC _ITERATOR_DEBUG_LEVEL=${LINA_ITERATOR_DEBUG_LEVEL})

#--------------------------------------------------------------------
# Links
#--------------------------------------------------------------------
target_link_libraries(${PROJECT_NAME} 
PUBLIC Lina::Common
PUBLIC Lina::Core
)


include(../CMake/EditorSandboxCommands.cmake)

#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
include(CMake/CommonFunctions.cmake)
group_sources(HEADERS SOURCES)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CustomTargets")
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT GameLaunncher)