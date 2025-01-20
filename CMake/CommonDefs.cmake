
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# This file is a part of: Lina Engine
# https://github.com/inanevin/LinaEngine
# 
# Author: Inan Evin
# http://www.inanevin.com
# 
# Copyright (c) [2018-] [Inan Evin]
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------


add_compile_definitions($<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=1>)
add_compile_definitions($<$<CONFIG:Debug>:LINA_DEBUG=1>)

add_compile_definitions($<$<CONFIG:DebugFAST>:_ITERATOR_DEBUG_LEVEL=0>)
add_compile_definitions($<$<CONFIG:DebugFAST>:LINA_DEBUG=1>)
add_compile_definitions($<$<CONFIG:DebugFAST>:_DEBUG=1>)
add_compile_definitions($<$<CONFIG:DebugFAST>:NDEBUG=0>)

add_compile_definitions($<$<CONFIG:DebugASAN>:_ITERATOR_DEBUG_LEVEL=2>)
add_compile_definitions($<$<CONFIG:DebugASAN>:LINA_DEBUG=1>)
add_compile_definitions($<$<CONFIG:DebugASAN>:_DEBUG=1>)
add_compile_definitions($<$<CONFIG:DebugASAN>:NDEBUG=0>)

add_compile_definitions($<$<CONFIG:DebugProfile>:_ITERATOR_DEBUG_LEVEL=1>)
add_compile_definitions($<$<CONFIG:DebugProfile>:LINA_DEBUG=1>)
add_compile_definitions($<$<CONFIG:DebugProfile>:_DEBUG=1>)
add_compile_definitions($<$<CONFIG:DebugProfile>:NDEBUG=0>)
add_compile_definitions($<$<CONFIG:DebugProfile>:LINA_MEMLEAK_CHECK=1>)

if(MSVC)
  add_compile_options($<$<CONFIG:DebugFAST>:/O2>)
  add_compile_options($<$<CONFIG:DebugFAST>:/MDd>)
  add_compile_options($<$<CONFIG:DebugProfile>:/MDd>)
  add_compile_options($<$<CONFIG:DebugProfile>:/Zi>)

  add_compile_options($<$<CONFIG:DebugASAN>:/Od>)
  add_compile_options($<$<CONFIG:DebugASAN>:/Ob0>)
  add_compile_options($<$<CONFIG:DebugASAN>:/MDd>)
  add_compile_options($<$<CONFIG:DebugASAN>:/fsanitize=address>)
  add_compile_options($<$<CONFIG:DebugASAN>:/Zi>)
  add_link_options($<$<CONFIG:DebugASAN>:/DEBUG>)
  add_link_options($<$<CONFIG:DebugProfile>:/DEBUG>)
  add_link_options($<$<CONFIG:DebugFAST>:/DEBUG>)
else()
  add_compile_options($<$<CONFIG:DebugASAN>:-fsanitize=address>)
  # add_link_options(-fsanitize=address)
endif()


# add_link_options($<$<CONFIG:DebugASAN>:/DEBUG>)
# add_link_options($<$<CONFIG:DebugProfile>:/DEBUG>)
# add_link_options($<$<CONFIG:DebugFAST>:/DEBUG>)
# add_link_options($<$<CONFIG:DebugASAN>:-fsanitize=address)

add_compile_definitions(LINA_VERSION_MAJOR=2)
add_compile_definitions(LINA_VERSION_MINOR=0)
add_compile_definitions(LINA_VERSION_PATCH=0)

#--------------------------------------------------------------------
# Platform
#--------------------------------------------------------------------

if (WIN32)
    add_compile_definitions(LINA_PLATFORM_WINDOWS=1)
endif()
if(APPLE)
    add_compile_definitions(LINA_PLATFORM_APPLE=1)
endif()
if(UNIX AND NOT APPLE)
    add_compile_definitions(LINA_PLATFORM_UNIX=1)
endif()
if(NOT WIN32 AND NOT APPLE AND NOT UNIX)
    add_compile_definitions(LINA_PLATFORM_UNKNOWN=1)
endif()


#--------------------------------------------------------------------
# Compiler
#--------------------------------------------------------------------
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_definitions(LINA_COMPILER_CLANG=1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_definitions(LINA_COMPILER_GNU=1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_definitions(LINA_COMPILER_MSVC=1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    add_compile_definitions(LINA_COMPILER_INTEL=1)
else()
    add_compile_definitions(LINA_COMPILER_OTHER=1)
endif()
