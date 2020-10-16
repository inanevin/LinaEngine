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
Class: PlatformSIMD

Used for platform dependent SIMD vector definitions.

Timestamp: 4/9/2019 12:30:55 AM
*/

#pragma once

#ifndef PlatformSIMD_HPP
#define PlatformSIMD_HPP

#include "Core/Environment.hpp"

//Include appropriate header files for SIMD features and CPU architecture
#if defined(SIMD_CPU_ARCH_x86) || defined(SIMD_CPU_ARCH_x86_64)
#if SIMD_SUPPORTED_LEVEL >= SIMD_LEVEL_x86_AVX2
#ifdef __GNUC__
#include <x86intrin.h>
#else
#include <immintrin.h>
#endif
#elif SIMD_SUPPORTED_LEVEL == SIMD_LEVEL_x86_AVX
#include <immintrin.h>
#elif SIMD_SUPPORTED_LEVEL == SIMD_LEVEL_x86_SSE4_2
#include <nmmintrin.h>
#elif SIMD_SUPPORTED_LEVEL == SIMD_LEVEL_x86_SSE4_1
#include <smmintrin.h>
#elif SIMD_SUPPORTED_LEVEL == SIMD_LEVEL_x86_SSSE3
#include <tmmintrin.h>
#elif SIMD_SUPPORTED_LEVEL == SIMD_LEVEL_x86_SSE3
#include <pmmintrin.h>
#elif SIMD_SUPPORTED_LEVEL == SIMD_LEVEL_x86_SSE2
#include <emmintrin.h>
#elif SIMD_SUPPORTED_LEVEL == SIMD_LEVEL_x86_SSE
#include <xmmintrin.h>
#endif
#endif



#endif