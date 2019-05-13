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


namespace LinaEngine
{
	class LINA_API Color 
	{

	public:

		constexpr Color(float rv = 1.0f, float gv = 1.0f, float bv = 1.0f, float av = 1.0f) : r(rv), g(gv), b(bv), a(av) {};
		FORCEINLINE float R() { return r; }
		FORCEINLINE float G() { return g; }
		FORCEINLINE float B() { return b; }
		FORCEINLINE float A() { return a; }
		FORCEINLINE float R() const { return r; }
		FORCEINLINE float G() const { return g; }
		FORCEINLINE float B() const { return b; }
		FORCEINLINE float A() const { return a; }


		static constexpr Color Red() { return Color(1, 0, 0); }
		static constexpr Color Green() { return Color(0, 1, 0); }
		static constexpr Color Blue() { return Color(0, 0, 1); }
		static constexpr Color Cyan() { return Color(0, 1, 1); }
		static constexpr Color Yellow() { return Color(1, 1, 0); }
		static constexpr Color Black() { return Color(0, 0, 0); }
		static constexpr Color White() { return Color(1, 1, 1); }
		static constexpr Color Purple() { return Color(1, 0, 1); }
		static constexpr Color Maroon() { return Color(0.5f, 0, 0); }
		static constexpr Color Beige() { return Color(0.96f, 0.96f, 0.862f); }
		static constexpr Color Brown() { return Color(0.647f, 0.164f, 0.164f); }
		static constexpr Color Gray() { return Color(0.5f, 0.5f, 0.5f); }

		float r, g, b, a = 1.0f;
	};



}


#endif