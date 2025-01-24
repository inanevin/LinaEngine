
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
project (GameProject)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;DebugFAST;DebugASAN;DebugProfile;" CACHE STRING "" FORCE)
set_property(GLOBAL PROPERTY DEBUG_CONFIGURATIONS "Debug;DebugFAST;DebugASAN;DebugProfile;")

set(TARGET_ARCHITECTURE "x64")
set(CMAKE_CXX_STANDARD 23)

#--------------------------------------------------------------------
# Options & Definitions
#--------------------------------------------------------------------
 
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_SOURCE_DIR}/bin>)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_EXE_LINKER_FLAGS_DEBUGFAST ${CMAKE_EXE_LINKER_FLAGS})
set(CMAKE_EXE_LINKER_FLAGS_DEBUGPROFILE ${CMAKE_EXE_LINKER_FLAGS})
set(CMAKE_EXE_LINKER_FLAGS_DEBUGASAN ${CMAKE_EXE_LINKER_FLAGS})
include(CMake/CommonDefs.cmake)

#--------------------------------------------------------------------
# Sub directories
#--------------------------------------------------------------------

include(Dependencies/Dependencies.cmake)
add_subdirectory(LinaCommon)
add_subdirectory(LinaCore)
add_subdirectory(GamePlugin)
add_subdirectory(GameLauncher)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CustomTargets")
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT GameLauncher)

