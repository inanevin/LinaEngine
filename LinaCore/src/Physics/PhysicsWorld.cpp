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

#include "Core/Physics/PhysicsWorld.hpp"

#include "Common/System/SystemInfo.hpp"
#include "Core/World/EntityWorld.hpp"

namespace Lina
{
	PhysicsWorld::PhysicsWorld(EntityWorld* ew) : m_world(ew)
	{
		const uint cMaxBodies			  = 1024;
		const uint cNumBodyMutexes		  = 0;
		const uint cMaxBodyPairs		  = 1024;
		const uint cMaxContactConstraints = 1024;
		m_physicsSystem.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, m_bpLayerInterface, m_objectBPLayerFilter, m_layerFilter);
	};

	PhysicsWorld::~PhysicsWorld()
	{
	}

	void PhysicsWorld::Simulate()
	{
	}

	JPH::ValidateResult PhysicsWorld::OnContactValidate(const JPH::Body& inBody1, const JPH::Body& inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult)
	{
	}

	void PhysicsWorld::OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
	{
	}

	void PhysicsWorld::OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
	{
	}

	void PhysicsWorld::OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair)
	{
	}

	void PhysicsWorld::OnBodyActivated(const JPH::BodyID& inBodyID, uint64 inBodyUserData)
	{
	}

	void PhysicsWorld::OnBodyDeactivated(const JPH::BodyID& inBodyID, uint64 inBodyUserData)
	{
	}

} // namespace Lina
