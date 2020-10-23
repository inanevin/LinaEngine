
# Language standard
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)


#--------------------------------------------------------------------
# Platform
#--------------------------------------------------------------------

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

#--------------------------------------------------------------------
# Properties
#--------------------------------------------------------------------

set_target_properties(
    ${PROJECT_NAME}
      PROPERTIES 
        CXX_STANDARD 17 
        CXX_STANDARD_REQUIRED YES 
        CXX_EXTENSIONS NO
)

#--------------------------------------------------------------------
# Compiler
#--------------------------------------------------------------------
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

#--------------------------------------------------------------------
# Lina
#--------------------------------------------------------------------

target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_OPENGL=1)
target_compile_definitions(${PROJECT_NAME} PUBLIC STB_IMAGE_IMPLEMENTATION=1)

if(LINA_CORE_ENABLE_LOGGING)
	target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_CORE_ENABLE_LOGGING=1)
endif()

if(LINA_ENABLE_EDITOR)
	target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_EDITOR=1)
endif()

if(LINA_ENABLE_TIMEPROFILING)
	target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_ENABLE_TIMEPROFILING=1)
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