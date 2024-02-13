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

#include "Common/SizeDefinitions.hpp"
#include "Common/Math/Color.hpp"

namespace Lina
{
	struct TBLR
	{
		float top	 = 0;
		float bottom = 0;
		float left	 = 0;
		float right	 = 0;
	};

	struct RectBackground
	{
		Color startColor = Color(0.0f, 0.0f, 0.0f, 0.0f);
		Color endColor	 = Color(0.0f, 0.0f, 0.0f, 0.0f);
		float rounding	 = 0.0f;
	};

	enum class MainAlignment
	{
		Free,
		EvenlyDistribute,
		SpaceBetween,
		ChildAlignment,
	};

	enum class CrossAlignment
	{
		Start,
		Center,
		End
	};

	enum class Fit
	{
		Fixed,
		FromParent,
		FromChildren,
	};

#define MARGINS_EQ(X)                                                                                                                                                                                                                                              \
	Margins                                                                                                                                                                                                                                                       \
	{                                                                                                                                                                                                                                                              \
		.top = X, .bottom = X, .left = X, .right = X                                                                                                                                                                                                               \
	}
#define MARGINS_V(X)                                                                                                                                                                                                                                               \
	Margins                                                                                                                                                                                                                                                        \
	{                                                                                                                                                                                                                                                              \
		.top = X, .bottom = X                                                                                                                                                                                                                                      \
	}
#define MARGINS_H(X)                                                                                                                                                                                                                                               \
	Margins                                                                                                                                                                                                                                                        \
	{                                                                                                                                                                                                                                                              \
		.left = X, .right = X                                                                                                                                                                                                                                      \
	}
#define MARGINS_T(X)                                                                                                                                                                                                                                               \
	Margins                                                                                                                                                                                                                                                        \
	{                                                                                                                                                                                                                                                              \
		.top = X                                                                                                                                                                                                                                                   \
	}
#define MARGINS_B(X)                                                                                                                                                                                                                                               \
	Margins                                                                                                                                                                                                                                                        \
	{                                                                                                                                                                                                                                                              \
		.bottom = X                                                                                                                                                                                                                                                \
	}
#define MARGINS_L(X)                                                                                                                                                                                                                                               \
	Margins                                                                                                                                                                                                                                                        \
	{                                                                                                                                                                                                                                                              \
		.left = X                                                                                                                                                                                                                                                  \
	}
#define MARGINS_R(X)                                                                                                                                                                                                                                               \
	Margins                                                                                                                                                                                                                                                        \
	{                                                                                                                                                                                                                                                              \
		.right = X                                                                                                                                                                                                                                                 \
	}
} // namespace Lina
