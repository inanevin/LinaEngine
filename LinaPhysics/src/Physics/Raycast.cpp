/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

#include "Physics/Raycast.hpp"
#include "Core/PhysicsCommon.hpp"

#ifdef LINA_PHYSICS_PHYSX
#include "PxPhysicsAPI.h"
using namespace physx;
#endif
namespace Lina::Physics
{
	HitInfo RaycastPose(const Lina::Vector3& from, const Lina::Vector3& unitDir, const Lina::Vector3& targetPosition, const Lina::Vector3& targetHalfExtents, float dist)
	{
#ifdef LINA_PHYSICS_PHYSX
		HitInfo hitInfo;
		PxRaycastHit pxHit;
		PxU32 maxHits = 1;
		PxTransform pose;
		pose.p = ToPxVector3(targetPosition);
		pose.q = ToPxQuat(Quaternion(Vector3(0, 0, 1), 0));
		PxBoxGeometry boxGeometry = PxBoxGeometry(ToPxVector3(targetHalfExtents));

		PxHitFlags hitFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL | PxHitFlag::eUV;
		PxU32 hitCount = PxGeometryQuery::raycast(ToPxVector3(from), ToPxVector3(unitDir),
			boxGeometry, pose,
			dist,
			hitFlags,
			maxHits, &pxHit);

		hitInfo.m_hitCount = hitCount;
		hitInfo.m_position = ToLinaVector3(pxHit.position);
		hitInfo.m_distance = pxHit.distance;
		hitInfo.m_normal = ToLinaVector3(pxHit.normal);
		return hitInfo;
#endif
		return HitInfo();
	}
}