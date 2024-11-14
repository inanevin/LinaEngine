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

#include "Common/Math/Rect.hpp"

namespace Lina
{
	Rect::Rect(const Recti& r)
	{
		pos	 = r.pos;
		size = r.size;
	}

	Rect Rect::Shrink(float percentage) const
	{
		Rect		r	 = *this;
		const float amtX = r.size.x * percentage;
		const float amtY = r.size.y * percentage;
		r.size -= Vector2(amtX, amtY);
		r.pos += Vector2(amtX, amtY) * 0.5f;
		return r;
	}

	Rect Rect::ShrinkByAmount(float amt) const
	{
		Rect r = *this;
		r.pos += amt;
		r.size -= amt * 2.0f;
		return r;
	}

	bool Rect::IsPointInside(const Vector2& p, bool equalOK) const
	{
		if (equalOK)
			return p.x >= pos.x && p.x <= pos.x + size.x && p.y >= pos.y && p.y <= pos.y + size.y;
		else
			return p.x > pos.x && p.x < pos.x + size.x && p.y > pos.y && p.y < pos.y + size.y;
	}

	bool Rect::IsClippingHorizontal(const Rect& other) const
	{
		if (pos.x + size.x <= other.pos.x || other.pos.x + other.size.x <= pos.x)
			return false;

		return true;
	}

	bool Rect::IsClippingVertical(const Rect& other) const
	{
		if (pos.y + size.y <= other.pos.y || other.pos.y + other.size.y <= pos.y)
			return false;

		return true;
	}

	bool Rect::IsClipping(const Rect& other) const
	{
		return IsClippingHorizontal(other) && IsClippingVertical(other);
	}

	bool Rect::IsRectInside(const Rect& other, float margin) const
	{
		return other.pos.x > pos.x - margin && other.GetEnd().x < GetEnd().x + margin && other.pos.y > pos.y - margin && other.GetEnd().y < GetEnd().y + margin;
	}

	bool Rect::IsRectCompletelyOutside(const Rect& other, float margin) const
	{
		const float left		= pos.x - margin;
		const float right		= pos.x + size.x + margin;
		const float top			= pos.y - margin;
		const float bottom		= pos.y + size.y + margin;
		const float otherLeft	= other.pos.x;
		const float otherRight	= other.pos.x + other.size.x;
		const float otherTop	= other.pos.y;
		const float otherBottom = other.pos.y + other.size.y;

		return (otherRight < left || otherLeft > right || otherBottom < top || otherTop > bottom);
	}

	bool Rect::IsInBorder(const Vector2& p, float borderThickness, int& borderPosition) const
	{
		if (p.x >= pos.x - borderThickness && p.x <= pos.x + borderThickness && p.y >= pos.y - borderThickness && p.y <= pos.y + borderThickness)
		{
			borderPosition = 0; // TopLeft
			return true;
		}
		else if (p.x >= pos.x + size.x - borderThickness && p.x <= pos.x + size.x + borderThickness && p.y >= pos.y - borderThickness && p.y <= pos.y + borderThickness)
		{
			borderPosition = 1; // TopRight
			return true;
		}
		else if (p.y >= pos.y - borderThickness && p.y <= pos.y + borderThickness && p.x > pos.x + borderThickness && p.x < pos.x + size.x - borderThickness)
		{
			borderPosition = 2; // Top
			return true;
		}
		else if (p.x >= pos.x + size.x - borderThickness && p.x <= pos.x + size.x + borderThickness && p.y > pos.y + borderThickness && p.y < pos.y + size.y - borderThickness)
		{
			borderPosition = 3; // Right
			return true;
		}
		else if (p.x >= pos.x + size.x - borderThickness && p.x <= pos.x + size.x + borderThickness && p.y >= pos.y + size.y - borderThickness && p.y <= pos.y + size.y + borderThickness)
		{
			borderPosition = 4; // BottomRight
			return true;
		}
		else if (p.y >= pos.y + size.y - borderThickness && p.y <= pos.y + size.y + borderThickness && p.x > pos.x + borderThickness && p.x < pos.x + size.x - borderThickness)
		{
			borderPosition = 5; // Bottom
			return true;
		}
		else if (p.x >= pos.x - borderThickness && p.x <= pos.x + borderThickness && p.y > pos.y + borderThickness && p.y < pos.y + size.y - borderThickness)
		{
			borderPosition = 6; // Left
			return true;
		}
		else if (p.x >= pos.x - borderThickness && p.x <= pos.x + borderThickness && p.y >= pos.y + size.y - borderThickness && p.y <= pos.y + size.y + borderThickness)
		{
			borderPosition = 7; // BottomLeft
			return true;
		}

		borderPosition = -1;
		return false;
	}

	Vector2 Rect::GetCenter() const
	{
		return Vector2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f);
	}

	Vector2 Rect::GetEnd() const
	{
		return pos + size;
	}

	Recti Recti::Shrink(float percentage) const
	{
		Recti	  r	   = *this;
		const int amtX = static_cast<int>(static_cast<float>(r.size.x) * percentage);
		const int amtY = static_cast<int>(static_cast<float>(r.size.y) * percentage);
		r.size -= Vector2i(amtX, amtY);
		r.pos += Vector2i(amtX / 2, amtY / 2);
		return r;
	}

	Recti Recti::ShrinkByAmount(uint32 amt) const
	{
		Recti r = *this;
		r.pos += amt;
		r.size -= amt * 2;
		return r;
	}

	bool Recti::IsPointInside(const Vector2i& p) const
	{
		return p.x > pos.x && p.x < pos.x + size.x && p.y > pos.y && p.y < pos.y + size.y;
	}

	bool Recti::IsInBorder(const Vector2& p, int borderThickness, int& borderPosition) const
	{
		if (p.x >= pos.x - borderThickness && p.x <= pos.x + borderThickness && p.y >= pos.y - borderThickness && p.y <= pos.y + borderThickness)
		{
			borderPosition = 0; // TopLeft
			return true;
		}
		else if (p.x >= pos.x + size.x - borderThickness && p.x <= pos.x + size.x + borderThickness && p.y >= pos.y - borderThickness && p.y <= pos.y + borderThickness)
		{
			borderPosition = 1; // TopRight
			return true;
		}
		else if (p.y >= pos.y - borderThickness && p.y <= pos.y + borderThickness && p.x > pos.x + borderThickness && p.x < pos.x + size.x - borderThickness)
		{
			borderPosition = 2; // Top
			return true;
		}
		else if (p.x >= pos.x + size.x - borderThickness && p.x <= pos.x + size.x + borderThickness && p.y > pos.y + borderThickness && p.y < pos.y + size.y - borderThickness)
		{
			borderPosition = 3; // Right
			return true;
		}
		else if (p.x >= pos.x + size.x - borderThickness && p.x <= pos.x + size.x + borderThickness && p.y >= pos.y + size.y - borderThickness && p.y <= pos.y + size.y + borderThickness)
		{
			borderPosition = 4; // BottomRight
			return true;
		}
		else if (p.y >= pos.y + size.y - borderThickness && p.y <= pos.y + size.y + borderThickness && p.x > pos.x + borderThickness && p.x < pos.x + size.x - borderThickness)
		{
			borderPosition = 5; // Bottom
			return true;
		}
		else if (p.x >= pos.x - borderThickness && p.x <= pos.x + borderThickness && p.y > pos.y + borderThickness && p.y < pos.y + size.y - borderThickness)
		{
			borderPosition = 6; // Left
			return true;
		}
		else if (p.x >= pos.x - borderThickness && p.x <= pos.x + borderThickness && p.y >= pos.y + size.y - borderThickness && p.y <= pos.y + size.y + borderThickness)
		{
			borderPosition = 7; // BottomLeft
			return true;
		}

		borderPosition = -1;
		return false;
	}

	Vector2i Recti::GetCenter() const
	{
		return Vector2i(pos.x + size.x / 2, pos.y + size.x / 2);
	}

} // namespace Lina
