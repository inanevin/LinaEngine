#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Author: Inan Evin
# www.inanevin.com
# 
# Copyright (C) 2022 Inan Evin
# 
# Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
# and limitations under the License.
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------

# All libraries
add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/Resources/Editor/GamePlugin/Dependencies/lib/")

# Lina Common Include
add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/LinaCommon/include" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/Resources/Editor/GamePlugin/Dependencies/LinaCommon/")

# Lina Core Include
add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/LinaCore/include" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/Resources/Editor/GamePlugin/Dependencies/LinaCore/")

# Game Plugin template
add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/GamePlugin/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/Resources/Editor/GamePlugin/")

# MISC - CMAKE stuff
add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/CMake/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/Resources/Editor/GamePlugin/CMake/")

## DEPENDENCIES

add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/Dependencies/fmt-7.0.3/include/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/Resources/Editor/GamePlugin/Dependencies/fmt/")

add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/Dependencies/glm-0.9.9.8/glm/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/Resources/Editor/GamePlugin/Dependencies/glm/glm/")

add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/Dependencies/lz4-1.9.4/include/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/Resources/Editor/GamePlugin/Dependencies/lz4/")

add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/Dependencies/memory-allocators/includes/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/Resources/Editor/GamePlugin/Dependencies/memory-allocators/")

add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/Dependencies/phmap/include/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/Resources/Editor/GamePlugin/Dependencies/phmap/")

add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/Dependencies/taskflow/include/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/Resources/Editor/GamePlugin/Dependencies/taskflow/")

add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/LinaGX/include/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/Resources/Editor/GamePlugin/Dependencies/LinaGX/")

add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/LinaVG/include/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/Resources/Editor/GamePlugin/Dependencies/LinaVG/")