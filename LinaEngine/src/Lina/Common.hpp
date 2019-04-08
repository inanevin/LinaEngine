/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: Common
Timestamp: 4/7/2019 3:29:18 PM

*/

#pragma once

#ifndef Common_HPP
#define Common_HPP

#include "Lina/Platform.hpp"

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

#if __cplusplus < 201103L
#define nullptr NULL
#define CONSTEXPR
#else
#define CONSTEXPR constexpr
#endif

#define NULL_COPY_AND_ASSIGN(T) \
	T(const T& other) {(void)other;} \
	void operator=(const T& other) { (void)other; }

#ifdef COMPILER_MSVC
#define FORCEINLINE __forceinline
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG)
#define FORCEINLINE inline __attribute__ ((always_inline))
#else
#define FORCEINLINE inline
#endif

#endif