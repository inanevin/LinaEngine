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

#include "Common/Math/Color.hpp"
#include "Common/Data/Streams.hpp"
#include "Common/Math/Math.hpp"

namespace Lina
{
	Color Color::Red	   = Color(1, 0, 0, 1);
	Color Color::Green	   = Color(0, 1, 0);
	Color Color::LightBlue = Color(0.4f, 0.4f, 0.8f);
	Color Color::Blue	   = Color(0, 0, 1);
	Color Color::DarkBlue  = Color(0.1f, 0.1f, 0.6f);
	Color Color::Cyan	   = Color(0, 1, 1);
	Color Color::Yellow	   = Color(1, 1, 0);
	Color Color::Black	   = Color(0, 0, 0);
	Color Color::White	   = Color(1, 1, 1);
	Color Color::Purple	   = Color(1, 0, 1);
	Color Color::Maroon	   = Color(0.5f, 0, 0);
	Color Color::Beige	   = Color(0.96f, 0.96f, 0.862f);
	Color Color::Brown	   = Color(0.647f, 0.164f, 0.164f);
	Color Color::Gray	   = Color(0.5f, 0.5f, 0.5f);

	void Color::SaveToStream(OStream& stream)
	{
		stream << x << y << z << w;
	}
	void Color::LoadFromStream(IStream& stream)
	{
		stream >> x >> y >> z >> w;
	}

	Color Color::HSToRGB()
	{
		const float hue		   = x;
		const float saturation = y;
		const float angle	   = hue * 6.0f;
		const float r		   = Math::Clamp(Math::Abs(angle - 3.0f) - 1.0f, 0.0f, 1.0f);
		const float g		   = Math::Clamp(2.0f - Math::Abs(angle - 2.0f), 0.0f, 1.0f);
		const float b		   = Math::Clamp(2.0f - Math::Abs(angle - 4.0f), 0.0f, 1.0f);
		return Math::Lerp(Color::White, Color(r, g, b, 1.0f), saturation);
	}

	Color Color::RGBToHSV()
	{
		Color hsv;

		float minVal = Math::Min(x, Math::Min(y, z));
		float maxVal = Math::Max(x, Math::Max(y, z));
		float delta	 = maxVal - minVal;

		// Hue calculation
		if (delta == 0)
		{
			hsv.x = 0;
		}
		else if (maxVal == x)
		{
			hsv.x = 60 * Math::Fmod(((y - z) / delta), 6);
		}
		else if (maxVal == y)
		{
			hsv.x = 60 * (((z - x) / delta) + 2);
		}
		else if (maxVal == z)
		{
			hsv.x = 60 * (((x - y) / delta) + 4);
		}

		if (hsv.x < 0)
		{
			hsv.x += 360;
		}

		// Saturation calculation
		hsv.y = (maxVal == 0) ? 0 : (delta / maxVal);

		// Value calculation
		hsv.z = maxVal;
		hsv.w = w;

		return hsv;
	}

	Color Color::HSVToRGB()
	{
		Color rgb;
		float C = z * y;
		float X = C * (1 - Math::Abs(Math::Fmod(x / 60.0, 2) - 1.0f));
		float m = z - C;
		float R1, G1, B1;

		if (x >= 0 && x < 60)
		{
			R1 = C;
			G1 = X;
			B1 = 0;
		}
		else if (x >= 60 && x < 120)
		{
			R1 = X;
			G1 = C;
			B1 = 0;
		}
		else if (x >= 120 && x < 180)
		{
			R1 = 0;
			G1 = C;
			B1 = X;
		}
		else if (x >= 180 && x < 240)
		{
			R1 = 0;
			G1 = X;
			B1 = C;
		}
		else if (x >= 240 && x < 300)
		{
			R1 = X;
			G1 = 0;
			B1 = C;
		}
		else
		{
			R1 = C;
			G1 = 0;
			B1 = X;
		}

		rgb.x = R1 + m;
		rgb.y = G1 + m;
		rgb.z = B1 + m;
		rgb.w = w;

		return rgb;
	}
} // namespace Lina
