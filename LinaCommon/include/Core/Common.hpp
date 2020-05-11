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

/****************************************** OPTIONS ******************************************/



#ifdef LINA_COMPILER_MSVC
#define FORCEINLINE __forceinline
#elif defined(LINA_COMPILER_GCC) || defined(LINA_COMPILER_CLANG)
#define FORCEINLINE inline __attribute__ ((always_inline))
#else
#define FORCEINLINE inline
#endif

#define BIT(x) (1 << x)
#define INVALID_VALUE 0xFFFFFFFF
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define LINA_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

#define NULL_COPY_AND_ASSIGN(T) \
	T(const T& other) {(void)other;} \
	void operator=(const T& other) { (void)other; }

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;      \
  void operator=(const TypeName&) = delete;

#define DISALLOW_COPY_ASSIGN_MOVE(TypeName) \
  TypeName(const TypeName&) = delete;      \
  void operator=(const TypeName&) = delete; \
	TypeName(TypeName&&) = delete; \
  TypeName& operator=(TypeName&&) = delete; 

#define DISALLOW_COPY_ASSIGN_NEW(TypeName) \
  TypeName(const TypeName&) = delete;      \
  void operator=(const TypeName&) = delete; \
  void* operator new(std::size_t) = delete;

#define DISALLOW_COPY_ASSIGN_NEW_MOVE(TypeName) \
  TypeName(const TypeName&) = delete;      \
  TypeName(TypeName&&) = delete; \
  TypeName& operator=(TypeName&&) = delete; \
  void operator=(const TypeName&) = delete; \
  void* operator new(std::size_t) = delete;



#endif