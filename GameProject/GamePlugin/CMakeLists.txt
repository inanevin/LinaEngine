
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

cmake_minimum_required (VERSION 3.10...3.31)
project(GamePlugin)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;DebugFAST;DebugASAN;DebugProfile;" CACHE STRING "" FORCE)
set_property(GLOBAL PROPERTY DEBUG_CONFIGURATIONS "Debug;DebugFAST;DebugASAN;DebugProfile;")

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
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

#--------------------------------------------------------------------
# Config & Options & Compile Definitions
#--------------------------------------------------------------------
include(${CMAKE_SOURCE_DIR}/CMake/ProjectSettings.cmake)
set(CMAKE_SHARED_LINKER_FLAGS_DEBUGFAST "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUGASAN "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUGPROFILE "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")

#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaCore/include)

#--------------------------------------------------------------------
# Links
#--------------------------------------------------------------------
target_link_libraries(${PROJECT_NAME} 
PUBLIC Lina::Core
)

#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
include(${CMAKE_SOURCE_DIR}/CMake/CommonFunctions.cmake)
group_sources(HEADERS SOURCES)





