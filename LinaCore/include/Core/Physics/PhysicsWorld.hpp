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

#include "Core/Physics/PhysicsLayerFilter.hpp"
#include "Core/Physics/PhysicsBPLayerInterface.hpp"
#include "Core/Physics/PhysicsObjectBPLayerFilter.hpp"
#include "Core/Physics/PhysicsRayCollector.hpp"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>

namespace Lina
{
	class EntityWorld;
	class Entity;
	class PhysicsContactListener;

	class PhysicsWorld : public JPH::ContactListener, public JPH::BodyActivationListener
	{
	public:
		PhysicsWorld(EntityWorld* world);
		virtual ~PhysicsWorld();

		void SetPhysicsSettings(const WorldPhysicsSettings& opts);
		void Begin();
		void End();
		void EnsurePhysicsBodies();

		void AddAllBodies();
		void RemoveAllBodies();
		void Simulate(float dt);

		Entity* GetEntityFromBodyID(const JPH::BodyID& id);

		bool CastRayFast(const Vector3& position, const Vector3& normDirection, float maxDistance, RayResult& outRayResult);
		bool CastRay(const Vector3& position, const Vector3& normDirection, float maxDistance, RayResult& outRayResult);
		bool CastRayAll(const Vector3& position, const Vector3& normDirection, float maxDistance, RayResult& outRayResult);

		// Contact
		void AddContactListener(PhysicsContactListener* list);
		void RemoveContactListener(PhysicsContactListener* list);

		JPH::Body* CreateBodyForEntity(Entity* e);
		void	   DestroyBodyForEntity(Entity* e);

		// Contacts
		virtual JPH::ValidateResult OnContactValidate(const JPH::Body& inBody1, const JPH::Body& inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult) override;
		virtual void				OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override;
		virtual void				OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override;
		virtual void				OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override;

		// Body
		virtual void OnBodyActivated(const JPH::BodyID& inBodyID, uint64 inBodyUserData) override;
		virtual void OnBodyDeactivated(const JPH::BodyID& inBodyID, uint64 inBodyUserData) override;

		inline JPH::PhysicsSystem& GetPhysicsSystem()
		{
			return m_physicsSystem;
		}

		inline const Vector<Entity*>& GetAddedBodies() const
		{
			return m_addedBodies;
		}

	private:
		JPH::PhysicsSystem		 m_physicsSystem;
		JPH::TempAllocatorImpl	 m_tempAllocator = JPH::TempAllocatorImpl(10 * 1024 * 1024);
		JPH::JobSystemThreadPool m_jobSystem	 = JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, JPH::thread::hardware_concurrency() - 1);

		PhysicsLayerFilter				m_layerFilter;
		PhysicsObjectBPLayerFilter		m_objectBPLayerFilter;
		PhysicsBPLayerInterface			m_bpLayerInterface;
		EntityWorld*					m_world = nullptr;
		Vector<Entity*>					m_addedBodies;
		PhysicsRayCollector				m_rayCollector;
		PhysicsBroadphaseCollector		m_broadphaseCollector;
		Vector<PhysicsContactListener*> m_contactListeners;
	};
} // namespace Lina
