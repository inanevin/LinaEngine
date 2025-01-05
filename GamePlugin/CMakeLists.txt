
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# This file is a part of: Lina Engine
# https://github.com/inanevin/LinaEngine
# 
# Author: Inan Evin
# http://www.inanevin.com
# 
# Copyright (c) [2018-] [Inan Evin]
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------

cmake_minimum_required (VERSION 3.8)
project(GamePlugin)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;DebugFAST;DebugASAN;DebugProfile;" CACHE STRING "" FORCE)
set_property(GLOBAL PROPERTY DEBUG_CONFIGURATIONS "Debug;DebugFAST;DebugASAN;DebugProfile;")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_SOURCE_DIR}/bin>)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------

set(SOURCES 
include/GamePluginExports.hpp
include/GamePlugin.hpp
)

set(HEADERS
src/GamePluginEntryPoint.cpp
src/GamePlugin.cpp
)

#--------------------------------------------------------------------
# Create project
#--------------------------------------------------------------------
## WIN32 Console Subsystem
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

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
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/Dependencies/LinaCommon)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/Dependencies/LinaCore)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/Dependencies/fmt)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/Dependencies/glm)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/Dependencies/lz4)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/Dependencies/phmap)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/Dependencies/taskflow)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/Dependencies/memory-allocators)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/Dependencies/LinaGX)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/Dependencies/LinaVG)




#--------------------------------------------------------------------
# Links
#--------------------------------------------------------------------
target_link_libraries(${PROJECT_NAME} 
${PROJECT_SOURCE_DIR}/Dependencies/lib//$<CONFIGURATION>/LinaCommon.lib
${PROJECT_SOURCE_DIR}/Dependencies/lib//$<CONFIGURATION>/fmtd.lib
${PROJECT_SOURCE_DIR}/Dependencies/lib//$<CONFIGURATION>/LinaCore.lib
)

#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
include(CMake/CommonFunctions.cmake)
group_sources(HEADERS SOURCES)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CustomTargets")
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT GamePlugin)




