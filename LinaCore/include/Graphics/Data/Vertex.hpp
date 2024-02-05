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

#ifndef Vertex_HPP
#define Vertex_HPP

#include "Common/Math/Vector.hpp"
#include "Common/Math/Color.hpp"

namespace Lina
{
	class Vertex
	{
	public:
		Vertex()  = default;
		~Vertex() = default;

		Vertex(const Vector3& p, const Vector3& n, const Color& c, const Vector2 txCoord) : pos(p), normal(n), color(c), uv(txCoord){};
		Vector3 pos	   = Vector3::Zero;
		Vector3 normal = Vector3::Zero;
		Color	color  = Color::White;
		Vector2 uv	   = Vector2::Zero;

		void SaveToStream(OStream& stream);
		void LoadFromStream(IStream& stream);
	};

} // namespace Lina

#endif
