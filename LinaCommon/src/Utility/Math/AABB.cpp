/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: AABB
Timestamp: 4/25/2019 11:47:53 PM

*/

#include "Utility/Math/AABB.hpp"

namespace LinaEngine
{
	AABB::AABB(Vector3* points, uint32 amt)
	{
		if (amt == 0) {
			m_Extents[0] = Vector3(0.0f, 0.0f, 0.0f);
			m_Extents[1] = Vector3(0.0f, 0.0f, 0.0f);
			return;
		}
		m_Extents[0] = points[0];
		m_Extents[1] = points[0];
		for (uint32 i = 1; i < amt; i++) {
			m_Extents[0] = m_Extents[0].Min(points[i]);
			m_Extents[1] = m_Extents[1].Max(points[i]);
		}
	}

	AABB::AABB(float* points, uint32 amt, uint32 stride)
	{
		if (amt == 0) {
			m_Extents[0] = Vector3(0.0f, 0.0f, 0.0f);
			m_Extents[1] = Vector3(0.0f, 0.0f, 0.0f);
			return;
		}
		Vector3 initialPoint(points[0], points[1], points[2]);
		m_Extents[0] = initialPoint;
		m_Extents[1] = initialPoint;
		uintptr index = 3;
		stride += 3;
		for (uint32 i = 1; i < amt; i++) {
			Vector3 point(points[index], points[index + 1], points[index + 2]);
			m_Extents[0] = m_Extents[0].Min(point);
			m_Extents[1] = m_Extents[1].Max(point);
			index += stride;
		}
	}

	AABB AABB::Transform(const Matrix& transform) const
	{
		Vector p000 = m_Extents[0].ToVector(1.0f);
		Vector p111 = m_Extents[1].ToVector(1.0f);

		Vector p100 = p000.select(VectorConstants::MASK_X, p111);
		Vector p010 = p000.select(VectorConstants::MASK_Y, p111);
		Vector p001 = p000.select(VectorConstants::MASK_Z, p111);
		Vector p011 = p111.select(VectorConstants::MASK_X, p000);
		Vector p101 = p111.select(VectorConstants::MASK_Y, p000);
		Vector p110 = p111.select(VectorConstants::MASK_Z, p000);

		p000 = transform.Transform(p000);
		p001 = transform.Transform(p001);
		p010 = transform.Transform(p010);
		p011 = transform.Transform(p011);
		p100 = transform.Transform(p100);
		p101 = transform.Transform(p101);
		p110 = transform.Transform(p110);
		p111 = transform.Transform(p111);

		Vector newMin = (p000.Min(p001)).Min(p010.Min(p011)).Min((p100.Min(p101)).Min(p110.Min(p111)));
		Vector newMax = (p000.Max(p001)).Max(p010.Max(p011)).Max((p100.Max(p101)).Max(p110.Max(p111)));
		return AABB(newMin, newMax);
	}

	bool AABB::IntersectRay(const Vector3& start, const Vector3& rayDir, float& point1, float& point2) const
	{
		Vector startVec = start.ToVector();
		Vector dirVec = rayDir.ToVector();
		Vector minVec = m_Extents[0].ToVector();
		Vector maxVec = m_Extents[1].ToVector();
		Vector rdirVec = dirVec.Reciprocal();
		Vector intersects1 = (minVec - startVec)*rdirVec;
		Vector intersects2 = (maxVec - startVec)*rdirVec;

		float mins[4];
		float maxs[4];
		intersects1.Min(intersects2).Store4F(mins);
		intersects1.Max(intersects2).Store4F(maxs);

		if ((mins[0] > maxs[1]) || (mins[1] > maxs[0])) {
			return false;
		}
		if (mins[1] > mins[0]) {
			mins[0] = mins[1];
		}
		if (maxs[1] < maxs[0]) {
			maxs[0] = maxs[1];
		}
		if ((mins[0] > maxs[2]) || (mins[2] > maxs[0])) {
			return false;
		}
		if (mins[2] > mins[0]) {
			mins[0] = mins[2];
		}
		if (maxs[2] < maxs[0]) {
			maxs[0] = maxs[2];
		}
		point1 = mins[0];
		point2 = maxs[0];
		return true;
	}

	bool AABB::IntersectLine(const Vector3& start, const Vector3& end) const
	{
		float p1, p2;
		Vector3 dir = (end - start);
		bool intersect = IntersectRay(start, dir.Normalized(), p1, p2);
		return intersect && p1*p1 < dir.MagnitudeSqrt();
	}
}

