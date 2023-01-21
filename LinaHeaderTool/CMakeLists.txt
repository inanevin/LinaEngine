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
project(LinaHeaderTool)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------

set(HEADERTOOL_SOURCES 

src/HeaderTool.cpp
)

set(HEADERTOOL_HEADERS

include/HeaderTool.hpp

)

#--------------------------------------------------------------------
# Create executable project
#--------------------------------------------------------------------
add_executable(${PROJECT_NAME} ${HEADERTOOL_SOURCES} ${HEADERTOOL_HEADERS})
add_executable(Lina::LinaHeaderTool ALIAS ${PROJECT_NAME}) 

#--------------------------------------------------------------------
# Options & Definitions
#--------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaCommon/include)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

# Link libraries (e.g. logging)
target_link_libraries(${PROJECT_NAME} 
	PUBLIC Lina::Common
)

#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	foreach(source IN LISTS HEADERTOOL_HEADERS HEADERTOOL_SOURCES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${HEADERTOOL_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
endif()