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

class Lina_Vector3F;

class Lina_Color
{

public:

	Lina_Color() : red(0), green(0), blue(0), alpha(1) {};
	Lina_Color(float r, float g, float b, float a = 1.0f) : red(r), green(g), blue(b), alpha(a) {};
	
	float red, green, blue, alpha;

	Lina_Vector3F ToVector3() const;

};

typedef Lina_Color Color;

static const Lina_Color COLOR_Red = Lina_Color(1, 0, 0);
static const Lina_Color COLOR_Green = Lina_Color(0, 1, 0);
static const Lina_Color COLOR_Blue = Lina_Color(0, 0, 1);
static const Lina_Color COLOR_Cyan = Lina_Color(0, 1, 1);
static const Lina_Color COLOR_Yellow = Lina_Color(1, 1, 0);
static const Lina_Color COLOR_Black = Lina_Color(0, 0, 0);
static const Lina_Color COLOR_White = Lina_Color(1, 1, 1);
static const Lina_Color COLOR_Purple = Lina_Color(1, 0, 1);
static const Lina_Color COLOR_Maroon = Lina_Color(0.5f, 0, 0);
static const Lina_Color COLOR_Beige = Lina_Color(0.96f, 0.96f, 0.862f);
static const Lina_Color COLOR_Brown = Lina_Color(0.647f, 0.164f, 0.164f);
static const Lina_Color COLOR_Gray = Lina_Color(0.5f, 0.5f, 0.5f);

#endif