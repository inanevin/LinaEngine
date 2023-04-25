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

#include "Math/Vector.hpp"

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
		bool	IsPointInside(const Vector2& p) const;
		bool	IsInBorder(const Vector2& p, float borderThickness, int& border) const;
		Vector2 GetCenter() const;

		Vector2 pos	 = Vector2(0, 0);
		Vector2 size = Vector2(0, 0);
	};

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

		Recti	 Shrink(float percentage) const;
		Recti	 ShrinkByAmount(uint32 amt) const;
		bool	 IsPointInside(const Vector2i& p) const;
		bool	 IsInBorder(const Vector2& p, int borderThickness, int& border) const;
		Vector2i GetCenter() const;

		Vector2i pos  = Vector2i(0, 0);
		Vector2i size = Vector2i(0, 0);
	};

} // namespace Lina

#endif
