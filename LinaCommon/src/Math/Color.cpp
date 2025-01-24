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
#include "Common/Platform/LinaVGIncl.hpp"

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

	Color::Color(const Vector4& col) : x(col.x), y(col.y), z(col.z), w(col.w)
	{
	}

	Color Color::Color255(float r, float g, float b, float a)
	{
		return Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
	}

	Color Color::Color255Linear(float r, float g, float b, float a)
	{
		Color color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
		return color.SRGB2Linear();
	}

	Color Color::ColorLinear(float r, float g, float b, float a)
	{
		Color color(r, g, b, a);
		return color.SRGB2Linear();
	}

	void Color::SaveToStream(OStream& stream) const
	{
		stream << x << y << z << w;
	}
	void Color::LoadFromStream(IStream& stream)
	{
		stream >> x >> y >> z >> w;
	}

	Color Color::Brighten(float amt) const
	{
		return Math::Lerp(*this, Color::White, amt);
	}

	Color Color::Darken(float amt) const
	{
		return Math::Lerp(*this, Color::Black, amt);
	}

	Color Color::HS2SRGB() const
	{
		const float hue		   = x;
		const float saturation = y;
		const float angle	   = hue * 6.0f;
		const float r		   = Math::Clamp(Math::Abs(angle - 3.0f) - 1.0f, 0.0f, 1.0f);
		const float g		   = Math::Clamp(2.0f - Math::Abs(angle - 2.0f), 0.0f, 1.0f);
		const float b		   = Math::Clamp(2.0f - Math::Abs(angle - 4.0f), 0.0f, 1.0f);
		return Math::Lerp(Color::White, Color(r, g, b, 1.0f), saturation);
	}

	Color Color::SRGB2HSV() const
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

	Color Color::HSV2SRGB() const
	{
		Color rgb;
		float C = z * y;
		float X = C * (1 - Math::Abs(Math::Fmod(x / 60.0f, 2.0f) - 1.0f));
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

	Color Color::SRGB2Linear() const
	{
		auto convert = [](float value) {
			if (value <= 0.04045f)
			{
				return value / 12.92f;
			}
			else
			{
				return Math::Pow((value + 0.055f) / 1.055f, 2.4f);
			}
		};

		return Color(convert(x), convert(y), convert(z), convert(w));
	}

	Color Color::Linear2SRGB() const
	{
		auto convert = [](float value) {
			if (value <= 0.0031308f)
			{
				return value * 12.92f;
			}
			else
			{
				return 1.055f * Math::Pow(value, 1.0f / 2.4f) - 0.055f;
			}
		};

		return Color(convert(x), convert(y), convert(z), convert(w));
	}

	void Color::Round()
	{
		x = Math::RoundToFloat(x);
		y = Math::RoundToFloat(y);
		z = Math::RoundToFloat(z);
		w = Math::RoundToFloat(w);
	}

	void Color::FromHex(const String& hex)
	{

		if (hex.size() != 7)
			return;

		if (hex[0] != '#')
			return;

		uint32 r, g, b;

#ifdef LINA_PLATFORM_WINDOWS
		sscanf_s(hex.c_str(), "#%02x%02x%02x", &r, &g, &b);
#else
		const int ret = std::sscanf(hex.c_str(), "#%02x%02x%02x", &r, &g, &b);
#endif
		*this = Color{static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(b) / 255.0f, 1.0f};
	}

	String Color::GetHex() const
	{
		const int32		  r = static_cast<int32>(x * 255);
		const int32		  g = static_cast<int32>(y * 255);
		const int32		  b = static_cast<int32>(z * 255);
		std::stringstream ss;
		ss << "#" << std::hex << std::setfill('0') << std::setw(2) << r << std::setw(2) << g << std::setw(2) << b;
		return ss.str();
	}

	Vector4 Color::ToVector() const
	{
		return Vector4(x, y, z, w);
	}

	LinaVG::Vec4Grad ColorGrad::AsLVG() const
	{
		LinaVG::Vec4Grad grad;
		grad.start = start.AsLVG4();
		grad.end   = end.AsLVG4();
		return grad;
	}
} // namespace Lina
