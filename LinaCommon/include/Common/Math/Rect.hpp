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

#ifndef RRect_HPP
#define RRect_HPP

#include "Common/Math/Vector.hpp"

namespace Lina
{
	class Recti;

	class Rect
	{
	public:
		Rect(){};
		Rect(const Vector2& p, const Vector2& s) : pos(p), size(s){};
		Rect(const Rect& r) : pos(r.pos), size(r.size){};
		Rect(const Recti& r);
		Rect(float x, float y, float w, float h) : pos(x, y), size(w, h){};
		Rect	Shrink(float percentage) const;
		Rect	ShrinkByAmount(float amt) const;
		Vector2 GetCenter() const;
		Vector2 GetEnd() const;
		bool	IsClippingHorizontal(const Rect& other) const;
		bool	IsClippingVertical(const Rect& other) const;
		bool	IsClipping(const Rect& other) const;
		bool	IsPointInside(const Vector2& p, bool equalOK = false) const;
		bool	IsInBorder(const Vector2& p, float borderThickness, int& border) const;
		bool	IsRectInside(const Rect& other, float margin = 0.0f) const;
		bool	IsRectCompletelyOutside(const Rect& other, float margin = 0.0f) const;

		inline void SaveToStream(OStream& stream) const
		{
			stream << pos << size;
		}

		inline void LoadFromStream(IStream& stream)
		{
			stream >> pos >> size;
		}

		Vector2 pos	 = Vector2(0, 0);
		Vector2 size = Vector2(0, 0);
	};

	inline Rect operator+(Rect const& a, Rect const& b)
	{
		return Rect(a.pos.x + b.pos.x, a.pos.y + b.pos.y, a.size.x + b.size.x, a.size.y + b.size.y);
	}

	class Recti
	{
	public:
		Recti(){};
		Recti(const Rect& r)
		{
			pos	 = r.pos;
			size = r.size;
		};
		Recti(const Recti& r)
		{
			pos	 = r.pos;
			size = r.size;
		}
		Recti(const Vector2i pos, const Vector2i& size)
		{
			this->pos  = pos;
			this->size = size;
		}
		Recti(int x, int y, int w, int h) : pos(x, y), size(w, h){};

		/// <summary>
		/// Shrink by percentage [0.0f, 1.0f]
		/// </summary>
		/// <param name="percentage"></param>
		/// <returns></returns>
		Recti Shrink(float percentage) const;

		Recti	 ShrinkByAmount(uint32 amt) const;
		bool	 IsPointInside(const Vector2i& p) const;
		bool	 IsInBorder(const Vector2& p, int borderThickness, int& border) const;
		Vector2i GetCenter() const;

		inline void SaveToStream(OStream& stream) const
		{
			pos.SaveToStream(stream);
			size.SaveToStream(stream);
		}

		inline void LoadFromStream(IStream& stream)
		{
			pos.LoadFromStream(stream);
			size.LoadFromStream(stream);
		}

		Vector2i pos  = Vector2i(0, 0);
		Vector2i size = Vector2i(0, 0);
	};

	class Rectui
	{
	public:
		Rectui(){};
		Rectui(const Vector2ui& p, const Vector2ui& s) : pos(p), size(s){};
		Rectui(uint32 x, uint32 y, uint32 w, uint32 h) : pos(x, y), size(w, h){};
		Vector2ui pos  = Vector2ui(0, 0);
		Vector2ui size = Vector2ui(0, 0);

		bool IsInside(uint32 x, uint32 y) const
		{
			return x > pos.x && x < pos.x + size.x && y > pos.y && y < pos.y + size.y;
		}

		bool Equals(const Rectui& other)
		{
			return pos.Equals(other.pos) && size.Equals(other.size);
		}
	};

} // namespace Lina

#endif
