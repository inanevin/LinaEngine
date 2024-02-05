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
project(LinaGame)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------
set (SOURCES


# World
src/World/Entity.cpp
src/World/Component.cpp
src/World/ComponentCache.cpp
src/World/Level/Level.cpp
src/World/Level/LevelManager.cpp
src/World/EntityWorld.cpp

# Physics
src/Physics/PhysicsWorld.cpp


)

#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(HEADERS

# World
include/Game/World/Entity.hpp
include/Game/World/Component.hpp
include/Game/World/ComponentCache.hpp
include/Game/World/Level/Level.hpp
include/Game/World/Level/LevelManager.hpp
include/Game/World/EntityWorld.hpp
include/Game/World/CommonWorld.hpp

# Physics
include/Game/Physics/PhysicsWorld.hpp

)

#--------------------------------------------------------------------
# Define the library & create an alias
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
add_library(Lina::Game ALIAS ${PROJECT_NAME}) 

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "LinaEngine")

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
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaCore/include)

#--------------------------------------------------------------------
# Subdirectories & linking
#--------------------------------------------------------------------

target_link_libraries(${PROJECT_NAME} 
	PUBLIC Lina::Common
	PUBLIC Lina::Core
)

#--------------------------------------------------------------------
# Folder structuring
#--------------------------------------------------------------------
include(../CMake/CommonFunctions.cmake)
group_sources(HEADERS SOURCES PLATFORM_HEADERS PLATFORM_SOURCES)
