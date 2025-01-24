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

set(LINAGX_FOLDER_BASE LinaEngine/Dependencies/LinaGXProject)
set(LINAVG_FOLDER_BASE LinaEngine/Dependencies/LinaVGProject)
set(LINAGX_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>)

add_subdirectory(LinaGX)
add_subdirectory(LinaVG)
## target_compile_definitions(LinaGX PUBLIC _ITERATOR_DEBUG_LEVEL=${LINA_ITERATOR_DEBUG_LEVEL})
## target_compile_definitions(LinaVG PUBLIC _ITERATOR_DEBUG_LEVEL=${LINA_ITERATOR_DEBUG_LEVEL})

# Jolt stuff
set(CPP_RTTI_ENABLED ON CACHE BOOL "" FORCE)
set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF CACHE BOOL "" FORCE)
set(OVERRIDE_CXX_FLAGS OFF CACHE BOOL "" FORCE)
set(OVERRIDE_MSVCCRT OFF CACHE BOOL "" FORCE)
set(GENERATE_DEBUG_SYMBOLS ON CACHE BOOL "" FORCE)
set(INTERPROCEDURAL_OPTIMIZATION OFF CACHE BOOL "" FORCE)

#OpenAL stuff
set(ALSOFT_INSTALL OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTANLL_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(Dependencies/JoltPhysics-5.2.0/Build)
add_subdirectory(Dependencies/memory-allocators)
add_subdirectory(Dependencies/lz4-1.9.4)
add_subdirectory(Dependencies/fmt-7.0.3)
add_subdirectory(Dependencies/glm-0.9.9.8/glm)
add_subdirectory(Dependencies/openal-soft-1.24.2)

set_property(TARGET lz4 PROPERTY FOLDER LinaEngine/Dependencies)
set_property(TARGET memory-allocators PROPERTY FOLDER LinaEngine/Dependencies)
set_property(TARGET fmt PROPERTY FOLDER LinaEngine/Dependencies)
set_property(TARGET glm_static PROPERTY FOLDER LinaEngine/Dependencies)
set_property(TARGET Jolt PROPERTY FOLDER LinaEngine/Dependencies)
set_property(TARGET OpenAL PROPERTY FOLDER LinaEngine/Dependencies)
set_property(TARGET aldebug PROPERTY FOLDER LinaEngine/Dependencies)
set_property(TARGET allafplay PROPERTY FOLDER LinaEngine/Dependencies)
set_property(TARGET alrecord PROPERTY FOLDER LinaEngine/Dependencies)
set_property(TARGET alsoft.common PROPERTY FOLDER LinaEngine/Dependencies)
set_property(TARGET alsoft.excommon PROPERTY FOLDER LinaEngine/Dependencies)
set_property(TARGET alsoft.fmt PROPERTY FOLDER LinaEngine/Dependencies)
set_property(TARGET altonegen PROPERTY FOLDER LinaEngine/Dependencies)
set_property(TARGET clang-tidy-check PROPERTY FOLDER LinaEngine/Dependencies)
set_property(TARGET openal-info PROPERTY FOLDER LinaEngine/Dependencies)
