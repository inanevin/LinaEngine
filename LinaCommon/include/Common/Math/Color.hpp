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

#include "LinaVG/Core/Vectors.hpp"

namespace LinaVG
{
	struct Vec4Grad;
}
namespace Lina
{
	class Vector4;

	class Color
	{

	public:
		Color(float rv = 1.0f, float gv = 1.0f, float bv = 1.0f, float av = 1.0f, bool is255 = false, bool convertToLinear = false) : x(is255 ? rv / 255.0f : rv), y(is255 ? gv / 255.0f : gv), z(is255 ? bv / 255.0f : bv), w(is255 ? av / 255.0f : av)
		{
			if (convertToLinear)
				*this = SRGB2Linear();
		};
		Color(const LinaVG::Vec4& v) : x(v.x), y(v.y), z(v.z), w(v.w){};
		Color(const Vector4& v);

		static Color Color255(float r, float g, float b, float a);
		static Color ColorLinear(float r, float g, float b, float a);
		static Color Color255Linear(float r, float g, float b, float);

		Color Brighten(float amt) const;
		Color Darken(float amt) const;

		LinaVG::Vec4 AsLVG4() const
		{
			return LinaVG::Vec4(x, y, z, w);
		}

		float x, y, z, w = 1.0f;

		bool operator!=(const Color& rhs) const
		{
			return !(x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w);
		}

		bool operator==(const Color& rhs) const
		{
			return (x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w);
		}

		Color operator*(const float& rhs) const
		{
			return Color(x * rhs, y * rhs, z * rhs, w * rhs);
		}

		Color operator+(const Color& rhs) const
		{
			return Color(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
		}

		Color operator/(float v) const
		{
			return Color(x / v, y / v, z / v, w / v);
		}

		Color operator/=(float v) const
		{
			return Color(x / v, y / v, z / v, w / v);
		}

		Color operator*=(float v) const
		{
			return Color(x * v, y * v, z * v, w * v);
		}

		float& operator[](unsigned int i)
		{
			return (&x)[i];
		}

		Color	HS2SRGB() const;
		Color	SRGB2HSV() const;
		Color	HSV2SRGB() const;
		Color	SRGB2Linear() const;
		Color	Linear2SRGB() const;
		Vector4 ToVector() const;
		String	GetHex() const;
		void	FromHex(const String& hex);

		void Round();

	public:
		static Color Red;
		static Color Green;
		static Color LightBlue;
		static Color Blue;
		static Color DarkBlue;
		static Color Cyan;
		static Color Yellow;
		static Color Black;
		static Color White;
		static Color Purple;
		static Color Maroon;
		static Color Beige;
		static Color Brown;
		static Color Gray;

		void SaveToStream(OStream& stream) const;
		void LoadFromStream(IStream& stream);
	};

	struct ColorGrad
	{
		Color start = Color::White;
		Color end	= Color::White;

		ColorGrad(){};
		ColorGrad(const Color& col)
		{
			start = col;
			end	  = col;
		}

		ColorGrad(const Color& colStart, const Color& colEnd)
		{
			start = colStart;
			end	  = colEnd;
		}

		LinaVG::Vec4Grad AsLVG() const;

		void SaveToStream(OStream& stream) const
		{
			stream << start << end;
		}

		void LoadFromStream(IStream& stream)
		{
			stream >> start >> end;
		}
	};

} // namespace Lina
