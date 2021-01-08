#
# This file is a part of: Lina Engine
# https://github.com/inanevin/LinaEngine
#
# Author: Inan Evin
# http://www.inanevin.com
# 
# Copyright (c) [2018-2020] [Inan Evin]
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
# 

cmake_minimum_required (VERSION 3.6)
project (Lina)
set(CMAKE_CXX_STANDARD 17)
#--------------------------------------------------------------------
# Options & Definitions
#--------------------------------------------------------------------

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_SUPPRESS_REGENERATION true)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
set(TARGET_ARCHITECTURE "x64")
set(ASSIMP_LIB "assimp-vc142-mtd.lib")


option(LINA_ENABLE_LOGGING "Enables console logging" ON)
option(LINA_ENABLE_PROFILING "Enables profiling" ON)
option(LINA_PRODUCTION_BUILD "Enable distribution ready build." OFF)

if(LINA_ENABLE_LOGGING)
	add_compile_definitions(LINA_ENABLE_LOGGING)
endif()

if(LINA_ENABLE_PROFILING)
	add_compile_definitions(LINA_ENABLE_PROFILING)
endif()

if(LINA_PRODUCTION_BUILD)
	add_compile_definitions(LINA_PRODUCTION_BUILD)
endif()

add_compile_definitions(LINA_AUDIO_OPENAL)
#add_compile_definitions(LINA_GRAPHICS_VULKAN)
add_compile_definitions(LINA_GRAPHICS_OPENGL)
add_compile_definitions(LINA_INPUT_GLFW)
add_compile_definitions(LINA_PHYSICS_NVIDIAPHYSX)

add_definitions(-DLINA_VMAJOR=2)
add_definitions(-DLINA_VMINOR=0)
add_definitions(-DLINA_VPATCH=0)

#--------------------------------------------------------------------
# Sub directories
#--------------------------------------------------------------------

add_subdirectory(LinaCommon)
add_subdirectory(LinaPhysics)
add_subdirectory(LinaGraphics)
add_subdirectory(LinaInput)
add_subdirectory(LinaEngine)
add_subdirectory(LinaAudio)
add_subdirectory(LinaResource)
add_subdirectory(Sandbox)

# VS startup project.
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Sandbox)