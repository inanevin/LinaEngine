/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: PlatformSIMD
Timestamp: 4/9/2019 12:30:55 AM

*/

#pragma once

#ifndef PlatformSIMD_HPP
#define PlatformSIMD_HPP


#include "PAM.hpp"

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