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
Timestamp: 4/25/2019 11:47:35 PM

*/

#pragma once

#ifndef AABB_HPP
#define AABB_HPP

#include "Vector.hpp"
#include "Matrix.hpp"
#include "Core/LinaAPI.hpp"

namespace LinaEngine
{
	class AABB
	{
	public:

		FORCEINLINE AABB() {}

		FORCEINLINE AABB(const Vector3& minExtents, const Vector3& maxExtents)
		{
			m_Extents[0] = minExtents;
			m_Extents[1] = maxExtents;
		}

		AABB(Vector3* points, uint32 amt);
		AABB(float* points, uint32 amt, uint32 stride = 0);

		bool IntersectRay(const Vector3& start, const Vector3& rayDir, float& point1, float& point2) const;
		bool IntersectLine(const Vector3& start, const Vector3& end) const;
		AABB Transform(const Matrix& transform) const;


		FORCEINLINE bool Intersects(const AABB& other) const
		{
			return
				((m_Extents[0].ToVector() >= other.m_Extents[1].ToVector()) |
				(m_Extents[1].ToVector() <= other.m_Extents[0].ToVector())).IsZero3f();
		}

		FORCEINLINE AABB Expand(float distance) const 
		{ 
			return Expand(Vector3(distance));
		}

		FORCEINLINE AABB Expand(const Vector3& amt) const
		{ 
			return AABB(m_Extents[0] - amt, m_Extents[1] + amt);
		}

		FORCEINLINE AABB MoveTo(const Vector3& destination) const
		{ 
			return Translate(destination - GetCenter());
		}

		FORCEINLINE Vector3 GetCenter() const 
		{ 
			return (m_Extents[1] + m_Extents[0]) * 0.5f;
		}

		FORCEINLINE Vector3 GetExtents() const
		{ 
			return (m_Extents[1] - m_Extents[0]) * 0.5f;	
		}

		FORCEINLINE Vector3 GetMinExtents() const  
		{ 
			return m_Extents[0];
		}

		FORCEINLINE Vector3 GetMaxExtents() const 
		{ 	
			return m_Extents[1]; 
		}

		FORCEINLINE void GetCenterAndExtents(Vector3& center, Vector3& m_Extents) const
		{
			m_Extents = (this->m_Extents[1] - this->m_Extents[0]) * 0.5f;
			center = this->m_Extents[0] + m_Extents;
		}

		FORCEINLINE float GetVolume() const
		{
			Vector3 lengths = m_Extents[1] - m_Extents[0];
			return lengths[0] * lengths[1] * lengths[2];
		}

		FORCEINLINE AABB Overlap(const AABB& other) const
		{
			return AABB(m_Extents[0].Max(other.m_Extents[0]),
				m_Extents[1].Min(other.m_Extents[1]));
		}

		FORCEINLINE bool Contains(const Vector3& point) const
		{
			return ((point.ToVector() <= m_Extents[0].ToVector()) |
				(point.ToVector() >= m_Extents[1].ToVector())).IsZero3f();		
		}

		FORCEINLINE bool Contains(const AABB& other) const
		{
			return ((other.m_Extents[0].ToVector() <= m_Extents[0].ToVector()) | (other.m_Extents[0].ToVector() >= m_Extents[1].ToVector()) |
					(other.m_Extents[1].ToVector() <= m_Extents[0].ToVector()) | (other.m_Extents[1].ToVector() >= m_Extents[1].ToVector())).IsZero3f();
		}

		FORCEINLINE AABB Translate(const Vector3& amt) const
		{
			return AABB(m_Extents[0] + amt, m_Extents[1] + amt);
		}

		FORCEINLINE AABB ScaleFromCenter(const Vector3& amt) const
		{
			Vector3 m_Extents, center;
			GetCenterAndExtents(center, m_Extents);
			m_Extents = m_Extents * amt;
			return AABB(center - m_Extents, center + m_Extents);
		}

		FORCEINLINE AABB ScaleFromOrigin(const Vector3& amt) const
		{
			return AABB(m_Extents[0] * amt, m_Extents[1] * amt);
		}

		FORCEINLINE AABB AddPoint(const Vector3& other) const
		{
			return AABB(m_Extents[0].Min(other), m_Extents[1].Max(other));
		}

		FORCEINLINE AABB AddAABB(const AABB& other) const
		{
			return AABB(m_Extents[0].Min(other.m_Extents[0]), m_Extents[1].Max(other.m_Extents[1]));
		}

		FORCEINLINE bool operator==(const AABB& other) const
		{
			return (m_Extents[0] == other.m_Extents[0]) && (m_Extents[1] == other.m_Extents[1]);
		}

		FORCEINLINE bool operator!=(const AABB& other) const
		{
			return (m_Extents[0] != other.m_Extents[0]) && (m_Extents[1] != other.m_Extents[1]);
		}

		FORCEINLINE bool equals(const AABB& other, float errorMargin = 1.e-4f) const
		{
			return m_Extents[0].equals(other.m_Extents[0], errorMargin) && m_Extents[1].equals(other.m_Extents[1], errorMargin);
		}

	private:
		Vector3 m_Extents[2];
	};



}


#endif