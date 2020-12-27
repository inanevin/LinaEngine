/* 
This file is a part of: Lina AudioEngine
https://github.com/inanevin/Lina

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
Class: Color

Defines color math representation.

Timestamp: 12/16/2020 2:31:06 PM
*/

#pragma once

#ifndef Color_HPP
#define Color_HPP

// Headers here.

namespace Lina
{

#define COLOR_RED Color(1, 0, 0, 1)
#define COLOR_GREEN Color(0, 1, 0)
#define COLOR_LIGHTBLUE Color(0.4f, 0.4f, 0.8f)
#define COLOR_BLUE Color(0, 0, 1)
#define COLOR_DARKBLUE Color(0.1f, 0.1f, 0.6f)
#define COLOR_CYAN Color(0, 1, 1)
#define COLOR_YELLOW Color(1, 1, 0)
#define COLOR_BLACK Color(0, 0, 0)
#define COLOR_WHITE Color(1, 1, 1)
#define COLOR_PURPLE Color(1, 0, 1)
#define COLOR_MAROON Color(0.5f, 0, 0)
#define COLOR_BEIGE Color(0.96f, 0.96f, 0.862f)
#define COLOR_BROWN Color(0.647f, 0.164f, 0.164f)
#define COLOR_GRAY Color(0.5f, 0.5f, 0.5f)

	class Color
	{
		
	public:

		Color(float rv = 1.0f, float gv = 1.0f, float bv = 1.0f, float av = 1.0f, bool is255 = false) :
			r(is255 ? rv / 255.0f : rv),
			g(is255 ? gv / 255.0f : gv),
			b(is255 ? bv / 255.0f : bv),
			a(is255 ? av / 255.0f : av) {};

		float r, g, b, a = 1.0f;

		bool operator!=(const Color& rhs) const
		{
			return !(r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a);
		}

		bool operator==(const Color& rhs) const
		{
			return (r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a);
		}

	public:


		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(r, g, b, a); // serialize things by passing them to the archive
		}
	
	};
}

#endif
