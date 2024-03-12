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

#include "Common/Common.hpp"

namespace Lina
{

	Direction DirectionOpposite(const Direction& dir)
	{
		if (dir == Direction::Right)
			return Direction::Left;

		if (dir == Direction::Left)
			return Direction::Right;

		if (dir == Direction::Bottom)
			return Direction::Top;

		if (dir == Direction::Top)
			return Direction::Bottom;

		return Direction::Center;
	}

	Direction DirectionFromVector(const Vector2& dir)
	{
		if (dir.x < -0.1f)
			return Direction::Left;

		if (dir.x > 0.1f)
			return Direction::Right;

		if (dir.y < -0.1f)
			return Direction::Top;

		if (dir.y > 0.1f)
			return Direction::Bottom;

		return Direction::Center;
	}

	Vector2 DirectionToVector(const Direction& dir)
	{
		if (dir == Direction::Left)
			return Vector2(-1.0f, 0.0f);

		if (dir == Direction::Right)
			return Vector2(1.0f, 0.0f);

		if (dir == Direction::Top)
			return Vector2(0.0f, -1.0f);

		if (dir == Direction::Bottom)
			return Vector2(0.0f, 1.0f);

		return Vector2::Zero;
	}

} // namespace Lina
