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

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/PlaneShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>

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

	void PhysicsWorld::Begin()
	{
		AddAllBodies();
	}

	void PhysicsWorld::End()
	{
		RemoveAllBodies();
	}

	void PhysicsWorld::EnsurePhysicsBodies()
	{
		m_world->ViewEntities([&](Entity* e, uint32 idx) -> bool {
			const EntityPhysicsSettings& phySettings = e->GetPhysicsSettings();
			if (phySettings.bodyType == PhysicsBodyType::None)
			{
				if (e->GetPhysicsBody())
					DestroyBodyForEntity(e);
				return false;
			}

			if (e->GetPhysicsBody() == nullptr)
				CreateBodyForEntity(e);
			return false;
		});
	}

	void PhysicsWorld::AddAllBodies()
	{
		JPH::BodyInterface& bodyInterface = m_physicsSystem.GetBodyInterface();

		Vector<JPH::Body*> bodyPtrs;
		bodyPtrs.reserve(m_world->GetActiveEntityCount());

		m_world->ViewEntities([&](Entity* e, uint32 idx) -> bool {
			if (e->GetPhysicsBody())
				bodyPtrs.push_back(e->GetPhysicsBody());
			return false;
		});

		if (bodyPtrs.empty())
			return;

		const size_t bodySz = bodyPtrs.size();
		m_addedBodyIDs.resize(bodySz);
		for (size_t i = 0; i < bodySz; i++)
			m_addedBodyIDs[i] = bodyPtrs[i]->GetID();

		const JPH::BodyInterface::AddState addState = bodyInterface.AddBodiesPrepare(m_addedBodyIDs.data(), static_cast<int32>(m_addedBodyIDs.size()));
		bodyInterface.AddBodiesFinalize(m_addedBodyIDs.data(), static_cast<int32>(m_addedBodyIDs.size()), addState, JPH::EActivation::DontActivate);

		LINA_TRACE("Added bodies to world! {0}", m_addedBodyIDs.size());
	}

	void PhysicsWorld::RemoveAllBodies()
	{
		if (m_addedBodyIDs.empty())
			return;

		JPH::BodyInterface& bodyInterface = m_physicsSystem.GetBodyInterface();
		bodyInterface.RemoveBodies(m_addedBodyIDs.data(), static_cast<int32>(m_addedBodyIDs.size()));
		LINA_TRACE("Removed bodies from world! {0}", m_addedBodyIDs.size());
	}

	JPH::Body* PhysicsWorld::CreateBodyForEntity(Entity* e)
	{
		LINA_ASSERT(e->GetPhysicsBody() == nullptr, "");

		JPH::BodyInterface&		  bodyInterface = m_physicsSystem.GetBodyInterface();
		JPH::BodyCreationSettings settings		= {};

		const EntityPhysicsSettings& phySettings = e->GetPhysicsSettings();

		JPH::Ref<JPH::Shape> shapeRef;

		if (phySettings.shapeType == PhysicsShapeType::Box)
		{
			JPH::BoxShapeSettings boxShapeSettings(ToJoltVec3(phySettings.shapeExtents * e->GetScale()));
			shapeRef = boxShapeSettings.Create().Get();
		}
		else if (phySettings.shapeType == PhysicsShapeType::Capsule)
		{
			JPH::CapsuleShapeSettings capsuleShapeSettings(phySettings.height * 0.5f, phySettings.radius);
			shapeRef = capsuleShapeSettings.Create().Get();
		}
		else if (phySettings.shapeType == PhysicsShapeType::Cylinder)
		{
			JPH::CylinderShapeSettings cylinderShapeSettings(phySettings.height * 0.5f, phySettings.radius);
			shapeRef = cylinderShapeSettings.Create().Get();
		}
		else if (phySettings.shapeType == PhysicsShapeType::Plane)
		{
			JPH::PlaneShapeSettings plane(JPH::Plane(ToJoltVec3(Vector3::Up), 1.0f));
			shapeRef = plane.Create().Get();
		}
		else if (phySettings.shapeType == PhysicsShapeType::Sphere)
		{
			JPH::SphereShapeSettings sphere(phySettings.radius);
			shapeRef = sphere.Create().Get();
		}

		settings.SetShape(shapeRef);
		settings.mPosition	  = ToJoltVec3(e->GetPosition());
		settings.mRotation	  = ToJoltQuat(e->GetRotation());
		settings.mMotionType  = ToJoltMotionType(phySettings.bodyType);
		settings.mObjectLayer = phySettings.bodyType == PhysicsBodyType::Static ? static_cast<uint16>(PhysicsObjectLayers::NonMoving) : static_cast<uint16>(PhysicsObjectLayers::Moving);

		JPH::Body* body	 = bodyInterface.CreateBody(settings);
		e->m_physicsBody = body;

		LINA_TRACE("Creted physics body for entity {0}", e->GetName());
	}

	void PhysicsWorld::DestroyBodyForEntity(Entity* e)
	{
		JPH::BodyInterface& bodyInterface = m_physicsSystem.GetBodyInterface();
		bodyInterface.DestroyBody(e->GetPhysicsBody()->GetID());
		e->m_physicsBody = nullptr;
		LINA_TRACE("Removed physics body for entity {0}", e->GetName());
	}

	void PhysicsWorld::Simulate(float dt)
	{
		const int cCollisionSteps = 1;
		m_physicsSystem.Update(dt, cCollisionSteps, &m_tempAllocator, &m_jobSystem);
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
