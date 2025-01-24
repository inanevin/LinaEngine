cmake_minimum_required (VERSION 3.10...3.31)
project(lz4)

set (SOURCES
	lz4.c
)

set (HEADERS
	include/lz4/lz4.h
)

add_library(${PROJECT_NAME}  ${HEADERS} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	foreach(source IN LISTS HEADERS SOURCES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${LinaCommon_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
endif()
