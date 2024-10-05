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

// Headers here.
#include "Common/Math/Vector.hpp"
#include "Common/Data/Vector.hpp"

namespace Lina
{
	class Plane;
	class AABB
	{
	public:
		AABB() = default;
		AABB(Vector3 min, Vector3 max)
		{
			boundsMin		  = min;
			boundsMax		  = max;
			boundsHalfExtents = (max - min) / 2.0f;
		}
		~AABB() = default;

		/// <summary>
		/// Returns true if this box is inside the given plane, given an arbitrary position.
		/// </summary>
		/// <param name="aabb"></param>
		/// <param name="plane"></param>
		bool IsInsidePlane(const Vector3& center, const Plane& plane);

		Vector3 GetPositive(const Vector3& normal) const;
		Vector3 GetNegative(const Vector3& normal) const;

		void SaveToStream(OStream& stream) const;
		void LoadFromStream(IStream& stream);

		inline void UpdateHalfExtents()
		{
			boundsHalfExtents = (boundsMax - boundsMin) / 2.0f;
		}

		Vector3			boundsHalfExtents = Vector3::Zero;
		Vector3			boundsMin		  = Vector3::Zero;
		Vector3			boundsMax		  = Vector3::Zero;
		Vector<Vector3> positions;

	private:
	};
} // namespace Lina
