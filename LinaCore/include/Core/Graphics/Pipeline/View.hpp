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

#include "Common/Math/Matrix.hpp"
#include "Common/Data/HashSet.hpp"

namespace Lina
{
	class AABB;

	class View
	{
	public:
		bool CalculateVisibility(const AABB& aabb);

		inline void SetView(const Matrix4& view)
		{
			m_view = view;
		}

		inline const Vector3& GetPosition() const
		{
			return m_position;
		}

		inline void SetPosition(const Vector3& v)
		{
			m_position = v;
		}

	private:
		Vector3 m_position = Vector3::Zero;
		Matrix4 m_view	   = Matrix4::Identity();
		// HashSet<EntityID> m_visibleEntities;
	};
} // namespace Lina
