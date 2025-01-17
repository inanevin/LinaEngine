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

#include "Core/Physics/PhysicsRayCollector.hpp"
#include "Core/Physics/PhysicsWorld.hpp"
#include "Core/World/Entity.hpp"
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhase.h>

namespace Lina
{
	void PhysicsRayCollector::AddHit(const JPH::RayCastResult& inResult)
	{
		m_results.push_back(inResult);
	}

	bool PhysicsRayCollector::CastRay(PhysicsWorld* world, const Vector3& position, const Vector3& normDirection, float maxDistance, RayResult& outRayResult)
	{
		m_results.clear();

		const Vector3	   direction = normDirection * maxDistance;
		JPH::RRayCast	   ray(ToJoltVec3(position), ToJoltVec3(direction));
		JPH::RayCastResult ioHit;
		const bool		   hit = world->GetPhysicsSystem().GetNarrowPhaseQuery().CastRay(ray, ioHit);

		if (hit)
		{
			const Vector3 hitPoint = position + normDirection * ioHit.mFraction;
			outRayResult.hitPoints.push_back(hitPoint);
			outRayResult.hitEntities.push_back(world->GetEntityFromBodyID(ioHit.mBodyID));
			outRayResult.hitDistances.push_back(ioHit.mFraction * direction.Magnitude());
		}

		return hit;
	}

	bool PhysicsRayCollector::CastRayAll(PhysicsWorld* world, const Vector3& position, const Vector3& normDirection, float maxDistance, RayResult& outRayResult)
	{
		m_results.clear();

		const Vector3 direction = normDirection * maxDistance;
		JPH::RRayCast ray(ToJoltVec3(position), ToJoltVec3(direction));

		JPH::RayCastSettings settings;
		world->GetPhysicsSystem().GetNarrowPhaseQuery().CastRay(ray, settings, *this);

		for (const JPH::RayCastResult& res : m_results)
		{
			const Vector3 hitPoint = position + normDirection * res.mFraction;
			outRayResult.hitPoints.push_back(hitPoint);
			outRayResult.hitEntities.push_back(world->GetEntityFromBodyID(res.mBodyID));
			outRayResult.hitDistances.push_back(res.mFraction * direction.Magnitude());
		}

		return !m_results.empty();
	}

	void PhysicsBroadphaseCollector::AddHit(const JPH::BroadPhaseCastResult& inResult)
	{
		m_results.push_back(inResult);
	}

	bool PhysicsBroadphaseCollector::CastRay(PhysicsWorld* world, const Vector3& position, const Vector3& normDirection, float maxDistance, RayResult& outRayResult)
	{
		m_results.clear();

		const Vector3 direction = normDirection * maxDistance;
		JPH::RayCast  ray(ToJoltVec3(position), ToJoltVec3(direction));

		JPH::RayCastSettings settings;
		world->GetPhysicsSystem().GetBroadPhaseQuery().CastRay(ray, *this);

		for (const JPH::BroadPhaseCastResult& res : m_results)
		{
			const Vector3 hitPoint = position + normDirection * res.mFraction;
			outRayResult.hitPoints.push_back(hitPoint);
			outRayResult.hitEntities.push_back(world->GetEntityFromBodyID(res.mBodyID));
			outRayResult.hitDistances.push_back(res.mFraction * direction.Magnitude());
		}

		return !m_results.empty();
	}

} // namespace Lina
