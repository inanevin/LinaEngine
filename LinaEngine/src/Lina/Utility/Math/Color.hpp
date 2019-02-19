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
10/26/2018 7:19:07 PM

*/

#pragma once

#ifndef Lina_Color_HPP
#define Lina_Color_HPP

#include "Vector.hpp"

namespace LinaEngine
{
	class LINA_API Color : public Vector4F
	{

	public:

		Color(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f) : Vector4F(r, g, b, a) {};

		
	};

	typedef Color Color;

	static const Color COLOR_Red = Color(1, 0, 0);
	static const Color COLOR_Green = Color(0, 1, 0);
	static const Color COLOR_Blue = Color(0, 0, 1);
	static const Color COLOR_Cyan = Color(0, 1, 1);
	static const Color COLOR_Yellow = Color(1, 1, 0);
	static const Color COLOR_Black = Color(0, 0, 0);
	static const Color COLOR_White = Color(1, 1, 1);
	static const Color COLOR_Purple = Color(1, 0, 1);
	static const Color COLOR_Maroon = Color(0.5f, 0, 0);
	static const Color COLOR_Beige = Color(0.96f, 0.96f, 0.862f);
	static const Color COLOR_Brown = Color(0.647f, 0.164f, 0.164f);
	static const Color COLOR_Gray = Color(0.5f, 0.5f, 0.5f);
}


#endif