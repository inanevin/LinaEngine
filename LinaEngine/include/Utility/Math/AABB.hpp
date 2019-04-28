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

namespace LinaEngine
{
	class AABB
	{
	public:

		FORCEINLINE AABB() {}

		FORCEINLINE AABB(const Vector3F& minExtents, const Vector3F& maxExtents)
		{
			extents[0] = minExtents;
			extents[1] = maxExtents;
		}

		AABB(Vector3F* points, uint32 amt);
		AABB(float* points, uint32 amt, uint32 stride = 0);

		bool IntersectRay(const Vector3F& start, const Vector3F& rayDir, float& point1, float& point2) const;
		bool IntersectLine(const Vector3F& start, const Vector3F& end) const;
		AABB Transform(const Matrix& transform) const;


		FORCEINLINE bool intersects(const AABB& other) const
		{
			return
				((extents[0].ToVector() >= other.extents[1].ToVector()) |
				(extents[1].ToVector() <= other.extents[0].ToVector())).IsZero3f();
		}

		FORCEINLINE AABB expand(float distance) const 
		{ 
			return expand(Vector3F(distance));
		}

		FORCEINLINE AABB expand(const Vector3F& amt) const
		{ 
			return AABB(extents[0] - amt, extents[1] + amt);
		}

		FORCEINLINE AABB moveTo(const Vector3F& destination) const
		{ 
			return translate(destination - getCenter());
		}

		FORCEINLINE Vector3F getCenter() const 
		{ 
			return (extents[1] + extents[0]) * 0.5f;
		}

		FORCEINLINE Vector3F getExtents() const
		{ 
			return (extents[1] - extents[0]) * 0.5f;	
		}

		FORCEINLINE Vector3F getMinExtents() const  
		{ 
			return extents[0];
		}

		FORCEINLINE Vector3F getMaxExtents() const 
		{ 	
			return extents[1]; 
		}

		FORCEINLINE void getCenterAndExtents(Vector3F& center, Vector3F& extents) const
		{
			extents = (this->extents[1] - this->extents[0]) * 0.5f;
			center = this->extents[0] + extents;
		}

		FORCEINLINE float getVolume() const
		{
			Vector3F lengths = extents[1] - extents[0];
			return lengths[0] * lengths[1] * lengths[2];
		
		}

		FORCEINLINE AABB overlap(const AABB& other) const
		{
			return AABB(extents[0].Max(other.extents[0]),
				extents[1].Min(other.extents[1]));
		}

		FORCEINLINE bool contains(const Vector3F& point) const
		{
			return ((point.ToVector() <= extents[0].ToVector()) |
				(point.ToVector() >= extents[1].ToVector())).IsZero3f();		
		}

		FORCEINLINE bool contains(const AABB& other) const
		{
			return ((other.extents[0].ToVector() <= extents[0].ToVector()) | (other.extents[0].ToVector() >= extents[1].ToVector()) |
					(other.extents[1].ToVector() <= extents[0].ToVector()) | (other.extents[1].ToVector() >= extents[1].ToVector())).IsZero3f();
		}

		FORCEINLINE AABB translate(const Vector3F& amt) const
		{
			return AABB(extents[0] + amt, extents[1] + amt);
		}

		FORCEINLINE AABB scaleFromCenter(const Vector3F& amt) const
		{
			Vector3F extents, center;
			getCenterAndExtents(center, extents);
			extents = extents * amt;
			return AABB(center - extents, center + extents);
		}

		FORCEINLINE AABB scaleFromOrigin(const Vector3F& amt) const
		{
			return AABB(extents[0] * amt, extents[1] * amt);
		}

		FORCEINLINE AABB addPoint(const Vector3F& other) const
		{
			return AABB(extents[0].Min(other), extents[1].Max(other));
		}

		FORCEINLINE AABB addAABB(const AABB& other) const
		{
			return AABB(extents[0].Min(other.extents[0]), extents[1].Max(other.extents[1]));
		}

		FORCEINLINE bool operator==(const AABB& other) const
		{
			return (extents[0] == other.extents[0]) && (extents[1] == other.extents[1]);
		}

		FORCEINLINE bool operator!=(const AABB& other) const
		{
			return (extents[0] != other.extents[0]) && (extents[1] != other.extents[1]);
		}

		FORCEINLINE bool equals(const AABB& other, float errorMargin = 1.e-4f) const
		{
			return extents[0].equals(other.extents[0], errorMargin) && extents[1].equals(other.extents[1], errorMargin);
		}

	private:
		Vector3F extents[2];
	};



}


#endif