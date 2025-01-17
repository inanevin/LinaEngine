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

#include "Core/Physics/CommonPhysics.hpp"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>

namespace JPH
{
	class BroadPhaseQuery;
}

namespace Lina
{
	class PhysicsWorld;

	class PhysicsRayCollector : public JPH::CollisionCollector<JPH::RayCastResult, JPH::CollisionCollectorTraitsCastRay>
	{
	public:
		virtual void AddHit(const JPH::RayCastResult& inResult) override;

		bool CastRay(PhysicsWorld* world, const Vector3& position, const Vector3& normDirection, float maxDistance, RayResult& outRayResult);
		bool CastRayAll(PhysicsWorld* world, const Vector3& position, const Vector3& normDirection, float maxDistance, RayResult& outRayResult);

	private:
		Vector<JPH::RayCastResult> m_results;
	};

	class PhysicsBroadphaseCollector : public JPH::CollisionCollector<JPH::BroadPhaseCastResult, JPH::CollisionCollectorTraitsCastRay>
	{
	public:
		virtual void AddHit(const JPH::BroadPhaseCastResult& inResult) override;
		bool		 CastRay(PhysicsWorld* world, const Vector3& position, const Vector3& normDirection, float maxDistance, RayResult& outRayResult);

	private:
		Vector<JPH::BroadPhaseCastResult> m_results;
	};

} // namespace Lina
