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
project(LinaSandbox)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------

set(SANDBOX_SOURCES 
src/Sandbox.cpp

src/Examples/TestExample/Levels/TestLevel.cpp

)

set(SANDBOX_HEADERS

include/Examples/TestExample/Levels/TestLevel.hpp

)

#--------------------------------------------------------------------
# Create executable project
#--------------------------------------------------------------------
add_executable(${PROJECT_NAME} ${SANDBOX_SOURCES} ${SANDBOX_HEADERS})

#--------------------------------------------------------------------
# Options & Definitions
#--------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

option(LINA_CLIENT_ENABLE_LOGGING "Enables console logging" ON)

if(LINA_CLIENT_ENABLE_LOGGING)
	target_compile_definitions(${PROJECT_NAME} PRIVATE LINA_CLIENT_ENABLE_LOGGING=1)
endif()


#--------------------------------------------------------------------
# Links
#--------------------------------------------------------------------
target_link_libraries(${PROJECT_NAME} Lina::Engine)


#if(WIN32)
#	string(REPLACE "/" "\\" source_path_windows "${CMAKE_CURRENT_SOURCE_DIR}/Resources")
#	string(REPLACE "/" "\\" build_path_windows "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/Resources")
#	execute_process(COMMAND cmd.exe /c mklink /J "${build_path_windows}" "${source_path_windows}" RESULT_VARIABLE exitcode)
#else()
#	execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/Resources ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/Resources RESULT_VARIABLE exitcode)
#endif()

#if(NOT ${exitcode} EQUAL 0)
#	MESSAGE("SYMLINKING FAILED: ${exitcode}")
#	MESSAGE("FALLING BACK TO COPYING")
#endif()

#file(GLOB_RECURSE Resources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/Resources/ ${CMAKE_CURRENT_SOURCE_DIR}/Resources/*.*)
#foreach(file IN LISTS Resources)
#	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Resources/${file} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/Resources/${file} COPYONLY)
#endforeach()

add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Resources" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/Resources")

add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND}
    -Dsrc_dir="${CMAKE_CURRENT_SOURCE_DIR}/Resources"
    -Ddst_dir="${CMAKE_BINARY_DIR}/Sandbox/Resources"
    -P "${CMAKE_SOURCE_DIR}/CMake/UpdateDirectory.cmake"
)

#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	foreach(source IN LISTS SANDBOX_HEADERS SANDBOX_SOURCES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${LINASANDBOX_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
endif()
