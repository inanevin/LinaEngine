cmake_minimum_required (VERSION 3.10...3.31)
project (memory-allocators CXX)

set(HEADERS src/Allocator.cpp
   includes/memoryallocators/Allocator.h
   includes/memoryallocators/Benchmark.h
   includes/memoryallocators/CAllocator.h
   includes/memoryallocators/DoublyLinkedList.h
   includes/memoryallocators/DoublyLinkedListImpl.h
   includes/memoryallocators/FreeListAllocator.h
   includes/memoryallocators/IO.h
   includes/memoryallocators/LinearAllocator.h
   includes/memoryallocators/PoolAllocator.h
   includes/memoryallocators/SinglyLinkedList.h
   includes/memoryallocators/SinglyLinkedListImpl.h
   includes/memoryallocators/StackAllocator.h
   includes/memoryallocators/StackLinkedList.h
   includes/memoryallocators/StackLinkedListImpl.h
   includes/memoryallocators/Utils.h
	)
	
set(SOURCES src/Allocator.cpp
   	src/CAllocator.cpp
   	src/LinearAllocator.cpp
   	src/StackAllocator.cpp
   	src/PoolAllocator.cpp
   	src/FreeListAllocator.cpp
   	src/Benchmark.cpp 
	)

add_library(${PROJECT_NAME}  ${HEADERS} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes)

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
