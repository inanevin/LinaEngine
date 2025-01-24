/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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

#pragma once

#if defined(__GNUC__) || defined(__clang__) || (defined(_MSC_VER) && _MSC_VER >= 1600)
#include <stdint.h>
#elif defined(_MSC_VER)
typedef signed __int8	 int8_t;
typedef unsigned __int8	 uint8_t;
typedef signed __int16	 int16_t;
typedef unsigned __int16 uint16_t;
typedef signed __int32	 int32_t;
typedef unsigned __int32 uint32_t;
typedef signed __int64	 int64_t;
typedef unsigned __int64 uint64_t;
typedef uint64_t		 uintptr_t;
typedef int64_t			 intptr_t;
typedef int16_t			 wchar_t;
#else
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

typedef signed char		   int8_t;
typedef unsigned char	   uint8_t;
typedef signed short int   int16_t;
typedef unsigned short int uint16_t;
typedef signed int		   int32_t;
typedef unsigned int	   uint32_t;
typedef long long		   int64_t;
typedef unsigned long long uint64_t;
typedef uint64_t		   uintptr_t;
typedef int64_t			   intptr_t;
typedef int16_t			   wchar_t;
#endif

typedef uint8_t	  CHART;
typedef int8_t	  int8;
typedef int16_t	  int16;
typedef int32_t	  int32;
typedef int64_t	  int64;
typedef uint8_t	  uint8;
typedef uint16_t  uint16;
typedef uint32_t  uint32;
typedef uint64_t  uint64;
typedef intptr_t  intptr;
typedef uintptr_t uintptr;

namespace Lina
{
	typedef uint64 EntityID;
	typedef uint64 ResourceID;
	typedef uint64 WidgetID;
} // namespace Lina
