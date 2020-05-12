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
project(LinaCommon CXX)
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------
set (LINACOMMON_SOURCES

    src/Core/Layer.cpp
    src/Core/LayerStack.cpp
	
	src/PackageManager/Generic/cmwc4096.cpp
	src/PackageManager/Generic/GenericMemory.cpp
	
	#Utility
	src/Utility/Math/Matrix.cpp
	src/Utility/Math/Quaternion.cpp
	src/Utility/Math/Transformation.cpp
	src/Utility/Math/Vector.cpp
	src/Utility/Math/AABB.cpp
	src/Utility/Math/Plane.cpp
	src/Utility/UtilityFunctions.cpp
	src/Utility/Log.cpp
)

#--------------------------------------------------------------------
# Set headers
#--------------------------------------------------------------------
set(LINACOMMON_HEADERS

	#CORE
	include/Core/Common.hpp
	include/Core/Environment.hpp
	include/Core/Internal.hpp
	include/Core/LinaArray.hpp
	include/Core/SizeDefinitions.hpp
	include/Core/Layer.hpp
	include/Core/LayerStack.hpp
	include/Core/LinaAPI.hpp
	# PAM
	include/PackageManager/Generic/cmwc4096.hpp
	include/PackageManager/Generic/GenericMath.hpp
	include/PackageManager/Generic/GenericMemory.hpp
	include/PackageManager/Generic/GenericVectorMath.hpp
	include/PackageManager/SSE/SSEVectorMath.hpp
	include/PackageManager/PAMMath.hpp
	include/PackageManager/PAMMemory.hpp
	include/PackageManager/PAMSIMD.hpp
	include/PackageManager/PAMVectorMath.hpp


	# Utility
	include/Utility/Math/Color.hpp
	include/Utility/Math/Matrix.hpp
	include/Utility/Math/Quaternion.hpp
	include/Utility/Math/Transformation.hpp
	include/Utility/Math/Vector.hpp
	include/Utility/Math/AABB.hpp
	include/Utility/Math/Plane.hpp
	include/Utility/Log.hpp
	include/Utility/UtilityFunctions.hpp

# SPDLOG
	include/Utility/spdlog/spdlog.h
	include/Utility/spdlog/async.h
	include/Utility/spdlog/async_logger.h
	include/Utility/spdlog/common.h
	include/Utility/spdlog/formatter.h
	include/Utility/spdlog/logger.h
	include/Utility/spdlog/spdlog.h
	include/Utility/spdlog/tolib.c
	include/Utility/spdlog/tweakme.h
	include/Utility/spdlog/version.h
	include/Utility/spdlog/sinks/stdout_color_sinks.h
	include/Utility/spdlog/sinks/android_sink.h
	include/Utility/spdlog/sinks/ansicolor_sink.h
	include/Utility/spdlog/sinks/base_sink.h
	include/Utility/spdlog/sinks/basic_file_sink.h
	include/Utility/spdlog/sinks/daily_file_sink.h
	include/Utility/spdlog/sinks/dist_sink.h
	include/Utility/spdlog/sinks/msvc_sink.h
	include/Utility/spdlog/sinks/null_sink.h
	include/Utility/spdlog/sinks/ostream_sink.h
	include/Utility/spdlog/sinks/rotating_file_sink.h
	include/Utility/spdlog/sinks/sink.h
	include/Utility/spdlog/sinks/stdout_sinks.h
	include/Utility/spdlog/sinks/syslog_sink.h
	include/Utility/spdlog/sinks/systemd_sink.h
	include/Utility/spdlog/sinks/wincolor_sink.h
	include/Utility/spdlog/details/async_logger_impl.h
	include/Utility/spdlog/details/circular_q.h
	include/Utility/spdlog/details/console_globals.h
	include/Utility/spdlog/details/file_helper.h
	include/Utility/spdlog/details/fmt_helper.h
	include/Utility/spdlog/details/log_msg.h
	include/Utility/spdlog/details/logger_impl.h
	include/Utility/spdlog/details/mpmc_blocking_q.h
	include/Utility/spdlog/details/null_mutex.h
	include/Utility/spdlog/details/os.h
	include/Utility/spdlog/details/pattern_formatter.h
	include/Utility/spdlog/details/periodic_worker.h
	include/Utility/spdlog/details/registry.h
	include/Utility/spdlog/details/thread_pool.h
	include/Utility/spdlog/fmt/bin_to_hex.h
	include/Utility/spdlog/fmt/fmt.h
	include/Utility/spdlog/fmt/ostr.h
	include/Utility/spdlog/fmt/bundled/chrono.h
	include/Utility/spdlog/fmt/bundled/color.h
	include/Utility/spdlog/fmt/bundled/core.h
	include/Utility/spdlog/fmt/bundled/format.h
	include/Utility/spdlog/fmt/bundled/format-inl.h
	include/Utility/spdlog/fmt/bundled/locale.h
	include/Utility/spdlog/fmt/bundled/ostream.h
	include/Utility/spdlog/fmt/bundled/posix.h
	include/Utility/spdlog/fmt/bundled/printf.h
	include/Utility/spdlog/fmt/bundled/ranges.h
	include/Utility/spdlog/fmt/bundled/time.h

)

#--------------------------------------------------------------------
# Define the library & create an alias
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${LINACOMMON_SOURCES} ${LINACOMMON_HEADERS})
add_library(Lina::Common ALIAS ${PROJECT_NAME}) 

#--------------------------------------------------------------------
# Config & Options & Compile Definitions
#--------------------------------------------------------------------

set_target_properties(
    ${PROJECT_NAME}
      PROPERTIES 
        CXX_STANDARD 17 
        CXX_STANDARD_REQUIRED YES 
        CXX_EXTENSIONS NO
		LINKER_LANGUAGE CXX
)

if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_PLATFORM_WINDOWS=1)
endif()
if(APPLE)
    # for MacOS X or iOS, watchOS, tvOS (since 3.10.3)
    target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_PLATFORM_APPLE=1)
endif()
if(UNIX AND NOT APPLE)
    # for Linux, BSD, Solaris, Minix
    target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_PLATFORM_UNIX=1)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
   target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_COMPILER_CLANG=1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
   target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_COMPILER_GNU=1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
   target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_COMPILER_MSVC=1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Intel")
   target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_COMPILER_INTEL=1)
else()
   target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_COMPILER_OTHER=1)
endif()

if(LINA_CORE_ENABLE_LOGGING)
	target_compile_definitions(${PROJECT_NAME} PRIVATE LINA_CORE_ENABLE_LOGGING=1)
endif()
#--------------------------------------------------------------------
# Build Type Config
#--------------------------------------------------------------------

if ( CMAKE_BUILD_TYPE STREQUAL "" )
	# CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
	# differentiation between debug and release builds.
	set(
		CMAKE_BUILD_TYPE
			"Debug"
		CACHE STRING
			"Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) \"Debug\" \"Release\" \"MinSizeRel\" \"RelWithDebInfo\"."
		FORCE
	)
	
	set(LINAENGINE_RUNTIME_COPY_DIR "Debug")
endif ( CMAKE_BUILD_TYPE STREQUAL "" )

if ( CMAKE_BUILD_TYPE STREQUAL "Release" )
    target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_RELEASE=1)
	set(LINAENGINE_RUNTIME_COPY_DIR "Release")
endif ( CMAKE_BUILD_TYPE STREQUAL "Release" )

if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_DEBUG=1)
	set(LINAENGINE_RUNTIME_COPY_DIR "Debug")
endif ( CMAKE_BUILD_TYPE STREQUAL "Debug" )

if ( CMAKE_BUILD_TYPE STREQUAL "MinSizeRel" )
    target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_MINSIZEREL=1)
	set(LINAENGINE_RUNTIME_COPY_DIR "MinSizeRel")
endif ( CMAKE_BUILD_TYPE STREQUAL "MinSizeRel" )

if ( CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" )
    target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_RELWITHDEBINFO=1)
	set(LINAENGINE_RUNTIME_COPY_DIR "RelWithDebInfo")
endif ( CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" )

#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)


# Language standard
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

#--------------------------------------------------------------------
# Subdirectories & linking
#--------------------------------------------------------------------

#--------------------------------------------------------------------
# Packages
#--------------------------------------------------------------------


#--------------------------------------------------------------------
# Generate export header for this dll project 
#--------------------------------------------------------------------
 include (GenerateExportHeader)
	GENERATE_EXPORT_HEADER (LinaCommon
	BASE_NAME LinaCommon
	EXPORT_MACRO_NAME LINA_API
	EXPORT_FILE_NAME Common.hpp
)


#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	foreach(source IN LISTS LINACOMMON_HEADERS LINACOMMON_SOURCES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${LinaCommon_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
endif()


