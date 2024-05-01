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

#include "Common/Data/Vector.hpp"
#include "Common/Data/String.hpp"
#include <span>

namespace Lina
{
	template <typename T, typename U> using Pair = std::pair<T, U>;
	template <typename T> using Span			 = std::span<T>;

	namespace linatl = std;

	typedef std::exception Exception;

	template <class F, class... Args> F ForEachArg(F f, Args&&... args)
	{
		(f(std::forward<Args>(args)), ...);
		return f;
	}

	namespace Internal
	{
		extern Vector<String> SplitIntoUTF8Chars(const String& str);
	}

#define ALIGN_SIZE_POW(sizeToAlign, PowerOfTwo) (((sizeToAlign) + (PowerOfTwo)-1) & ~((PowerOfTwo)-1))
#define ALIGN_SIZE(sizeToAlign, Alignment)		(sizeToAlign + Alignment - 1) - sizeToAlign % Alignment;
#define IS_SIZE_ALIGNED(sizeToTest, PowerOfTwo) (((sizeToTest) & ((PowerOfTwo)-1)) == 0)
#define IS_ODD(num)								((num) & 1)
#define IS_EVEN(num)							(!IS_ODD((num)))
#define IS_BETWEEN(numToTest, numLow, numHigh)	((unsigned char)((numToTest) >= (numLow) && (numToTest) <= (numHigh)))
#define MAKE_EVEN_LB(X)                                                                                                                                                                                                                                            \
	if (X % 2 != 0)                                                                                                                                                                                                                                                \
		X--;
#define MAKE_EVEN_UB(X)                                                                                                                                                                                                                                            \
	if (X % 2 != 0)                                                                                                                                                                                                                                                \
		X++;
#define MAKE_ODD_LB(X)                                                                                                                                                                                                                                             \
	if (X % 2 == 0)                                                                                                                                                                                                                                                \
		X--;
#define MAKE_ODD_UB(X)                                                                                                                                                                                                                                             \
	if (X % 2 == 0)                                                                                                                                                                                                                                                \
		X - ++;

#ifndef LINA_DISABLE_VC_WARNING
#if defined(_MSC_VER)
#define LINA_DISABLE_VC_WARNING(w) __pragma(warning(push)) __pragma(warning(disable : w))
#else
#define LINA_DISABLE_VC_WARNING(w)
#endif
#endif

#ifndef LINA_RESTORE_VC_WARNING
#if defined(_MSC_VER)
#define LINA_RESTORE_VC_WARNING() __pragma(warning(pop))
#else
#define LINA_RESTORE_VC_WARNING()
#endif
#endif

} // namespace Lina
