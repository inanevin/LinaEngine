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

#include "Common/Math/AABB.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Math/Plane.hpp"

namespace Lina
{

	bool AABB::IsInsidePlane(const Vector3& center, const Plane& plane)
	{
		const float r = boundsHalfExtents.x * Math::Abs(plane.normal.x) + boundsHalfExtents.y * Math::Abs(plane.normal.y) + boundsHalfExtents.z * Math::Abs(plane.normal.z);
		return -r <= plane.GetSignedDistance(center);
	}
	Vector3 AABB::GetPositive(const Vector3& normal) const
	{
		Vector3 positive = boundsMin;
		if (normal.x >= 0.0f)
			positive.x = boundsMax.x;
		if (normal.y >= 0.0f)
			positive.y = boundsMax.y;
		if (normal.z >= 0.0f)
			positive.z = boundsMax.z;

		return positive;
	}
	Vector3 AABB::GetNegative(const Vector3& normal) const
	{
		Vector3 negative = boundsMax;
		if (normal.x >= 0.0f)
			negative.x = boundsMin.x;
		if (normal.y >= 0.0f)
			negative.y = boundsMin.y;
		if (normal.z >= 0.0f)
			negative.z = boundsMin.z;

		return negative;
	}

	void AABB::SaveToStream(OStream& stream) const
	{
		stream << boundsHalfExtents;
		stream << boundsMin;
		stream << boundsMax;
		stream << positions;
	}

	void AABB::LoadFromStream(IStream& stream)
	{
		stream >> boundsHalfExtents;
		stream >> boundsMin;
		stream >> boundsMax;
		stream >> positions;
	}
} // namespace Lina
