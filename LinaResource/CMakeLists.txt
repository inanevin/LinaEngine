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
project(LinaResource)



#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------
set (LINARESOURCE_SOURCES
	
	src/Core/ResourceManager.cpp
	src/Core/ResourcesCommon.cpp
	src/Core/ResourceBundle.cpp
	
	src/Utility/Packager.cpp
	
	src/Resources/ImageResource.cpp
	src/Resources/MeshResource.cpp
	src/Resources/ShaderResource.cpp
	src/Resources/AudioResource.cpp
	src/Resources/LevelResource.cpp
	src/Resources/MaterialResource.cpp
	
	
)

#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(LINARESOURCE_HEADERS


	include/Core/ResourceManager.hpp
	include/Core/ResourcesCommon.hpp
	include/Core/ResourceBundle.hpp
	include/Core/ResourcePackages.hpp
	
	include/Utility/Packager.hpp
	
	
	include/Resources/ImageResource.hpp
	include/Resources/MeshResource.hpp
	include/Resources/ShaderResource.hpp
	include/Resources/AudioResource.hpp
	include/Resources/LevelResource.hpp
	include/Resources/MaterialResource.hpp
)


#--------------------------------------------------------------------
# Define the library & create an alias
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${LINARESOURCE_SOURCES} ${LINARESOURCE_HEADERS})
add_library(Lina::Resource ALIAS ${PROJECT_NAME}) 


#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------


target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/LinaCommon/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/alut/include)
#target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/stb/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bit7z/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/assimp/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/openal/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/libshaderc/include)

#--------------------------------------------------------------------
# Subdirectories & linking
#--------------------------------------------------------------------


target_link_libraries(${PROJECT_NAME} 

	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/alut/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/alut.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/bit7z/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/bit7z64.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/assimp/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/${ASSIMP_LIB}
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/openal/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/OpenAL32.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/libshaderc/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/shaderc_shared.lib
	PUBLIC oleaut32.lib
	PUBLIC user32.lib
	PRIVATE Lina::Common
)


#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	foreach(source IN LISTS LINARESOURCE_HEADERS LINARESOURCE_SOURCES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${LinaResource_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
endif()


