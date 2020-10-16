/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


/*
Class: Environment

Architecture and configuration related macros are defined here.

Timestamp: 4/17/2019 1:50:21 AM
*/

#pragma once

#ifndef Environment_HPP
#define Environment_HPP

#if __cplusplus < 201103L

#else

#endif

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
#if (defined(SIMD_CPU_ARCH_x86) || defined(SIMD_CPU_ARCH_x86_64))
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


#endif