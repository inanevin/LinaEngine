/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: Platform
Timestamp: 4/7/2019 3:51:47 PM

*/

#pragma once

#ifndef Platform_HPP
#define Platform_HPP

//Defines for SIMD-enabled CPU architectures, and supported
//feature levels
#define SIMD_CPU_ARCH_OTHER  0
#define SIMD_CPU_ARCH_x86    1
#define SIMD_CPU_ARCH_x86_64 2

#define SIMD_LEVEL_NONE   0
#define SIMD_LEVEL_x86_SSE    1
#define SIMD_LEVEL_x86_SSE2   2
#define SIMD_LEVEL_x86_SSE3   3
#define SIMD_LEVEL_x86_SSSE3  4
#define SIMD_LEVEL_x86_SSE4_1 5
#define SIMD_LEVEL_x86_SSE4_2 6
#define SIMD_LEVEL_x86_AVX    7
#define SIMD_LEVEL_x86_AVX2   8

//Detect CPU architecture
#if (defined(_M_AMD64) || defined(_M_X64) || defined(__amd64) ) || defined(__x86_64__)
#define SIMD_CPU_ARCH SIMD_CPU_ARCH_x86_64
#elif defined(_M_X86) || defined(__i386__) || defined(_X86_) || defined(_M_IX86)
#define SIMD_CPU_ARCH SIMD_CPU_ARCH_x86
#else
#define SIMD_CPU_ARCH SIMD_CPU_ARCH_OTHER
#endif

//Detect supported SIMD features
#if defined(SIMD_CPU_ARCH_x86) || defined(SIMD_CPU_ARCH_x86_64)
#if defined(INSTRSET)
#define SIMD_SUPPORTED_LEVEL INSTRSET
#elif defined(__AVX2__)
#define SIMD_SUPPORTED_LEVEL SIMD_LEVEL_x86_AVX2
#elif defined(__AVX__)
#define SIMD_SUPPORTED_LEVEL SIMD_LEVEL_x86_AVX
#elif defined(__SSE4_2__)
#define SIMD_SUPPORTED_LEVEL SIMD_LEVEL_x86_SSE4_2
#elif defined(__SSE4_1__)
#define SIMD_SUPPORTED_LEVEL SIMD_LEVEL_x86_SSE4_1
#elif defined(__SSSE3__)
#define SIMD_SUPPORTED_LEVEL SIMD_LEVEL_x86_SSSE3
#elif defined(__SSE3__)
#define SIMD_SUPPORTED_LEVEL SIMD_LEVEL_x86_SSE3
#elif defined(__SSE2__) || defined(SIMD_CPU_ARCH_x86_64)
#define SIMD_SUPPORTED_LEVEL SIMD_LEVEL_x86_SSE2
#elif defined(__SSE__)
#define SIMD_SUPPORTED_LEVEL SIMD_LEVEL_x86_SSE
#elif defined(_M_IX86_FP)
#define SIMD_SUPPORTED_LEVEL _M_IX86_FP
#else
#define SIMD_SUPPORTED_LEVEL SIMD_LEVEL_NONE
#endif
#else
#define SIMD_SUPPORTED_LEVEL SIMD_LEVEL_NONE
#endif

// Detect operating system
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(WIN64)
#define OPERATING_SYSTEM_WINDOWS
#elif defined(__linux__)
#define OPERATING_SYSTEM_LINUX
// Probably detect MAC, iOS and Android at some point
#else
#define OPERATING_SYSTEM_OTHER
#endif

// Detect compiler
#if defined(__clang__)
#define COMPILER_CLANG
#elif defined(__GNUC__) || defined(__GNUG__)
#define COMPILER_GCC
#elif defined(_MSC_VER)
#define COMPILER_MSVC
#else
#define COMPILER_OTHER
#endif



#endif