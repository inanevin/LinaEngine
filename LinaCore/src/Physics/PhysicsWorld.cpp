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
#include "Core/Physics/PhysicsContactListener.hpp"
#include "Core/Physics/PhysicsMaterial.hpp"
#include "Core/Resources/ResourceManager.hpp"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CollisionCollector.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/PlaneShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>

namespace Lina
{
	PhysicsWorld::PhysicsWorld(EntityWorld* ew) : m_world(ew)
	{
		const uint32 cMaxBodies				= 1024;
		const uint32 cNumBodyMutexes		= 0;
		const uint32 cMaxBodyPairs			= 1024;
		const uint32 cMaxContactConstraints = 1024;
		m_physicsSystem.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, m_bpLayerInterface, m_objectBPLayerFilter, m_layerFilter);
		m_physicsSystem.SetContactListener(this);
	};

	PhysicsWorld::~PhysicsWorld()
	{
	}

	void PhysicsWorld::SetPhysicsSettings(const WorldPhysicsSettings& opts)
	{
		m_physicsSystem.SetGravity(ToJoltVec3(opts.gravity));
	}

	void PhysicsWorld::Begin()
	{
		EnsurePhysicsBodies();
		m_physicsSystem.OptimizeBroadPhase();
	}

	void PhysicsWorld::End()
	{
		RemoveAllBodies();

		for (Entity* e : m_addedBodies)
			DestroyBodyForEntity(e);
	}

	void PhysicsWorld::EnsurePhysicsBodies()
	{
		Vector<Entity*> newBodies;

		m_world->ViewEntities([&](Entity* e, uint32 idx) -> bool {
			const EntityPhysicsSettings& phySettings = e->GetPhysicsSettings();
			if (phySettings.bodyType == PhysicsBodyType::None)
			{
				if (e->GetPhysicsBody())
				{
					auto it = linatl::find_if(m_addedBodies.begin(), m_addedBodies.end(), [e](Entity* ent) -> bool { return e == ent; });
					if (it != m_addedBodies.end())
					{
						m_physicsSystem.GetBodyInterface().RemoveBody(e->GetPhysicsBody()->GetID());
						m_addedBodies.erase(it);
					}
					DestroyBodyForEntity(e);
				}
				return false;
			}
			if (e->GetPhysicsBody() == nullptr)
			{
				CreateBodyForEntity(e);
				newBodies.push_back(e);
			}

			return false;
		});

		AddBodies(newBodies);
	}

	void PhysicsWorld::AddBodies(const Vector<Entity*>& entities)
	{
		if (entities.empty())
			return;

		JPH::BodyInterface& bodyInterface = m_physicsSystem.GetBodyInterface();

		m_addedBodies.insert(m_addedBodies.end(), entities.begin(), entities.end());

		const size_t bodySz = entities.size();
		if (bodySz == 0)
			return;

		Vector<JPH::BodyID> bodyIDs;
		bodyIDs.resize(bodySz);

		for (size_t i = 0; i < bodySz; i++)
		{
			JPH::Body* body = entities.at(i)->GetPhysicsBody();
			bodyIDs[i]		= body->GetID();
		}

		const JPH::BodyInterface::AddState addState = bodyInterface.AddBodiesPrepare(bodyIDs.data(), static_cast<int32>(bodySz));
		bodyInterface.AddBodiesFinalize(bodyIDs.data(), static_cast<int32>(bodySz), addState, JPH::EActivation::Activate);
		LINA_TRACE("Added bodies to world! {0}", bodyIDs.size());
	}

	void PhysicsWorld::RemoveAllBodies()
	{
		if (m_addedBodies.empty())
			return;

		const size_t		bodySz = m_addedBodies.size();
		Vector<JPH::BodyID> bodyIDs;
		bodyIDs.resize(bodySz);

		for (size_t i = 0; i < bodySz; i++)
			bodyIDs[i] = m_addedBodies.at(i)->GetPhysicsBody()->GetID();

		JPH::BodyInterface& bodyInterface = m_physicsSystem.GetBodyInterface();
		bodyInterface.RemoveBodies(bodyIDs.data(), static_cast<int32>(bodyIDs.size()));
		m_addedBodies.clear();
		LINA_TRACE("Removed bodies from world! {0}", bodySz);
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
			JPH::PlaneShapeSettings plane(JPH::Plane(ToJoltVec3(e->GetRotation().GetUp()), 0.0f));
			shapeRef = plane.Create().Get();
		}
		else if (phySettings.shapeType == PhysicsShapeType::Sphere)
		{
			JPH::SphereShapeSettings sphere(phySettings.radius);
			shapeRef = sphere.Create().Get();
		}

		PhysicsMaterial* mat	 = m_world->GetResourceManager()->GetResource<PhysicsMaterial>(phySettings.material);
		settings.mRestitution	 = mat->GetRestitution();
		settings.mFriction		 = mat->GetFriction();
		settings.mAngularDamping = mat->GetAngularDamping();
		settings.mLinearDamping	 = mat->GetLinearDamping();

		settings.SetShape(shapeRef);
		settings.mGravityFactor				   = phySettings.gravityMultiplier;
		settings.mMassPropertiesOverride.mMass = phySettings.mass;
		// settings.mOverrideMassProperties = JPH::EOverrideMassProperties::MassAndInertiaProvided;
		settings.mPosition	  = ToJoltVec3(e->GetPosition() + phySettings.offset);
		settings.mRotation	  = ToJoltQuat(e->GetRotation());
		settings.mMotionType  = ToJoltMotionType(phySettings.bodyType);
		settings.mObjectLayer = phySettings.bodyType == PhysicsBodyType::Static ? static_cast<uint16>(PhysicsObjectLayers::NonMoving) : static_cast<uint16>(PhysicsObjectLayers::Moving);

		JPH::Body* body	 = bodyInterface.CreateBody(settings);
		e->m_physicsBody = body;

		LINA_TRACE("Created physics body for entity {0}", e->GetName());

		return body;
	}

	void PhysicsWorld::DestroyBodyForEntity(Entity* e)
	{
		JPH::BodyInterface& bodyInterface = m_physicsSystem.GetBodyInterface();

		auto it = linatl::find_if(m_addedBodies.begin(), m_addedBodies.end(), [e](Entity* ent) -> bool { return ent == e; });
		if (it != m_addedBodies.end())
		{
			bodyInterface.RemoveBody((*it)->GetPhysicsBody()->GetID());
			m_addedBodies.erase(it);
		}

		bodyInterface.DestroyBody(e->GetPhysicsBody()->GetID());
		e->m_physicsBody = nullptr;
		LINA_TRACE("Removed physics body for entity {0}", e->GetName());
	}

	void PhysicsWorld::Simulate(float dt)
	{
		const int cCollisionSteps = 1;
		m_physicsSystem.Update(dt, cCollisionSteps, &m_tempAllocator, &m_jobSystem);

		for (Entity* e : m_addedBodies)
		{
			JPH::Body*		 body = e->GetPhysicsBody();
			const Vector3	 p	  = FromJoltVec3(body->GetCenterOfMassPosition());
			const Quaternion q	  = FromJoltQuat(body->GetRotation());
			e->SetPosition(p - e->GetPhysicsSettings().offset);
			e->SetRotation(q);
		}
	}

	bool PhysicsWorld::CastRay(const Vector3& position, const Vector3& normDirection, float maxDistance, RayResult& outRayResult)
	{
		return m_rayCollector.CastRay(this, position, normDirection, maxDistance, outRayResult);
	}

	bool PhysicsWorld::CastRayFast(const Vector3& position, const Vector3& normDirection, float maxDistance, RayResult& outRayResult)
	{
		return m_broadphaseCollector.CastRay(this, position, normDirection, maxDistance, outRayResult);
	}

	bool PhysicsWorld::CastRayAll(const Vector3& position, const Vector3& normDirection, float maxDistance, RayResult& outRayResult)
	{
		return m_rayCollector.CastRayAll(this, position, normDirection, maxDistance, outRayResult);
	}

	JPH::ValidateResult PhysicsWorld::OnContactValidate(const JPH::Body& inBody1, const JPH::Body& inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult)
	{
		return {};
	}

	void PhysicsWorld::OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
	{
		if (m_contactListeners.empty())
			return;

		Entity*		   e1 = GetEntityFromBodyID(inBody1.GetID());
		Entity*		   e2 = GetEntityFromBodyID(inBody2.GetID());
		const Vector3& p1 = FromJoltVec3(inManifold.GetWorldSpaceContactPointOn1(inBody1.GetID().GetIndex()));
		const Vector3& p2 = FromJoltVec3(inManifold.GetWorldSpaceContactPointOn2(inBody2.GetID().GetIndex()));

		for (PhysicsContactListener* list : m_contactListeners)
			list->OnContactBegin(e1, e2, p1, p2);
	}

	void PhysicsWorld::OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
	{
		if (m_contactListeners.empty())
			return;

		Entity*		   e1 = GetEntityFromBodyID(inBody1.GetID());
		Entity*		   e2 = GetEntityFromBodyID(inBody2.GetID());
		const Vector3& p1 = FromJoltVec3(inManifold.GetWorldSpaceContactPointOn1(inBody1.GetID().GetIndex()));
		const Vector3& p2 = FromJoltVec3(inManifold.GetWorldSpaceContactPointOn2(inBody2.GetID().GetIndex()));

		for (PhysicsContactListener* list : m_contactListeners)
			list->OnContact(e1, e2, p1, p2);
	}

	void PhysicsWorld::OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair)
	{
		if (m_contactListeners.empty())
			return;
		Entity* e1 = GetEntityFromBodyID(inSubShapePair.GetBody1ID());
		Entity* e2 = GetEntityFromBodyID(inSubShapePair.GetBody2ID());

		for (PhysicsContactListener* list : m_contactListeners)
			list->OnContactEnd(e1, e2);
	}

	void PhysicsWorld::OnBodyActivated(const JPH::BodyID& inBodyID, uint64 inBodyUserData)
	{
	}

	void PhysicsWorld::OnBodyDeactivated(const JPH::BodyID& inBodyID, uint64 inBodyUserData)
	{
	}

	void PhysicsWorld::AddContactListener(PhysicsContactListener* list)
	{
		m_contactListeners.push_back(list);
	}

	void PhysicsWorld::RemoveContactListener(PhysicsContactListener* list)
	{
		auto it = linatl::find_if(m_contactListeners.begin(), m_contactListeners.end(), [list](PhysicsContactListener* l) -> bool { return l == list; });
		m_contactListeners.erase(it);
	}

	Entity* PhysicsWorld::GetEntityFromBodyID(const JPH::BodyID& id)
	{
		auto it = linatl::find_if(m_addedBodies.begin(), m_addedBodies.end(), [&](Entity* e) -> bool { return e->GetPhysicsBody()->GetID() == id; });
		return *it;
	}

} // namespace Lina
