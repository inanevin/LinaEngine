/*
Author: Inan Evin
www.inanevin.com

MIT License

Lina Engine, Copyright (c) 2018 Inan Evin

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

4.0.30319.42000
10/23/2018 11:07:16 PM

*/

#pragma once

#ifndef Lina_Common_HPP
#define Lina_Common_HPP

#include <cassert>
#include <stdio.h>
#include <stdexcept>

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


//Include known-size integer files, based on compiler. Some compilers do not have these
//files, so they must be created manually.
#if defined(__GNUC__) || defined(__clang__) || (defined(_MSC_VER) && _MSC_VER >= 1600)
#include <stdint.h>
#elif defined(_MSC_VER)
typedef signed   __int8  int8_t;
typedef unsigned __int8  uint8_t;
typedef signed   __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef signed   __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef signed   __int64 int64_t;
typedef unsigned __int64 uint64_t;
typedef uint64_t uintptr_t;
typedef int64_t intptr_t;
typedef int16_t wchar_t;
#else
	// TODO: Have a better "if all else fails" condition.
	//
	// Preferably, make all sizes at least big enough for the data size.
	// Also, try including C++11 standard types if possible.
	//
	// List of C++'s standard data sizing rules:
	// 
	// sizeof(char) == 1
	// sizeof(char) <= sizeof(short)
	// sizeof(short) <= sizeof(int)
	// sizeof(int) <= sizeof(long)
	// sizeof(long) <= sizeof(long long)
	// sizeof(char) * CHAR_BIT >= 8
	// sizeof(short) * CHAR_BIT >= 16
	// sizeof(int) * CHAR_BIT >= 16
	// sizeof(long) * CHAR_BIT >= 32
	// sizeof(long long) * CHAR_BIT >= 64

typedef signed   char      int8_t;
typedef unsigned char      uint8_t;
typedef signed   short int int16_t;
typedef unsigned short int uint16_t;
typedef signed   int       int32_t;
typedef unsigned int       uint32_t;
typedef long long          int64_t;
typedef unsigned long long uint64_t;
typedef uint64_t uintptr_t;
typedef int64_t intptr_t;
typedef int16_t wchar_t;
#endif

typedef uint8_t CHART;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef intptr_t intptr;
typedef uintptr_t uintptr;

#ifdef DEBUG
#define assertCheck assert
#else
#define assertCheck (void)
#endif

#ifdef COMPILER_MSVC
#define FORCEINLINE __forceinline
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG)
#define FORCEINLINE inline __attribute__ ((always_inline))
#else
#define FORCEINLINE inline
#endif

#if __cplusplus < 201103L
#define nullptr NULL
#define CONSTEXPR
#else
#define CONSTEXPR constexpr
#endif

#define NULL_COPY_AND_ASSIGN(T) \
	T(const T& other) {(void)other;} \
	void operator=(const T& other) { (void)other; }

#define LOG_ERROR "Error"
#define LOG_WARNING "Warning"
#define LOG_TYPE_RENDERER "Renderer"
#define LOG_TYPE_IO "IO"
#define DEBUG_LOG(category, level, message, ...) \
	fprintf(stderr, "[%s] ", category); \
	fprintf(stderr, "[%s] (%s:%d): ", level, __FILE__, __LINE__); \
	fprintf(stderr, message, ##__VA_ARGS__); \
	fprintf(stderr, "\n")
#define DEBUG_LOG_TEMP(message, ...) DEBUG_LOG("TEMP", "TEMP", message, ##__VA_ARGS__)
#define DEBUG_LOG_TEMP2(message) DEBUG_LOG("TEMP", "TEMP", "%s", message)
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

#endif