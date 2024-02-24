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
		Color		K = Color(0.0f, -1.0f / 3.0f, 2.0f / 3.0f, -1.0f);
		Color		p = Math::Lerp(Color(z, y, K.w, K.z), Color(y, z, K.x, K.y), Math::Step(z, y));
		Color		q = Math::Lerp(Color(p.x, p.y, p.w, x), Color(x, p.y, p.z, p.x), Math::Step(p.x, x));
		const float d = q.x - Math::Min(q.w, q.y);
		const float e = 1.0e-10;
		return Color(Math::Abs(q.z + (q.w - q.y) / (6.0f * d + e)), d / (q.x + e), q.x, z);
	}

	Color Color::HSVToRGB()
	{
		const float h = x;
		const float s = y;
		const float v = z;
		const int	i = int(h * 6.0);
		const float f = h * 6.0 - float(i);
		const float p = v * (1.0 - s);
		const float q = v * (1.0 - f * s);
		const float t = v * (1.0 - (1.0 - f) * s);
		switch (i % 6)
		{
		case 0:
			return Color(v, t, p, w);
		case 1:
			return Color(q, v, p, w);
		case 2:
			return Color(p, v, t, w);
		case 3:
			return Color(p, q, v, w);
		case 4:
			return Color(t, p, v, w);
		case 5:
			return Color(v, p, q, w);
		}
	}
} // namespace Lina
