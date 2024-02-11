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
cmake_minimum_required (VERSION 3.8)
project(FlarePacker)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------

set(SOURCES 
include/FlarePackerAppDelegate.hpp
include/FlarePackerCommon.hpp
include/FlarePacker.hpp
include/GUI/TopSection.hpp
)

set(HEADERS
src/FlarePackerAppDelegate.cpp
src/FlarePacker.cpp
src/GUI/TopSection.cpp
)

#--------------------------------------------------------------------
# Create executable project
#--------------------------------------------------------------------

# WIN32 Console Subsystem
if(WIN32)
	add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})
else()
	add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER LinaEngine/Apps/)


#--------------------------------------------------------------------
# Config & Options & Compile Definitions
#--------------------------------------------------------------------
include(${CMAKE_SOURCE_DIR}/CMake/ProjectSettings.cmake)
include(${CMAKE_SOURCE_DIR}/CMake/ExecutableSettings.cmake)

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