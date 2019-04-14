/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: PlatformVectorMath
Timestamp: 4/9/2019 12:55:47 AM

*/

#pragma once

#ifndef PlatformVectorMath_HPP
#define PlatformVectorMath_HPP

#include "Platform.hpp"

#if defined(SIMD_CPU_ARCH_x86) || defined(SIMD_CPU_ARCH_x86_64)

#include "SSE/SSEVectorMath.hpp"
typedef LinaEngine::SSEVector PlatformVector;

#else
#include "Generic/GenericVectorMath.hpp"
typedef LinaEngine::GenericVector PlatformVector;
#endif


#endif