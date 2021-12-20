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

#include "Core/PhysicsCommon.hpp"
#include "Core/Backend/Bullet/BulletPhysicsEngine.hpp"  
#include "Log/Log.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Math/Color.hpp"

namespace Lina::Physics
{
	BulletPhysicsEngine* BulletPhysicsEngine::s_physicsEngine;

	BulletPhysicsEngine::BulletPhysicsEngine()
	{
		LINA_TRACE("[Constructor] -> Physics Engine ({0})", typeid(*this).name());
	}

	BulletPhysicsEngine::~BulletPhysicsEngine()
	{
		LINA_TRACE("[Destructor] -> Physics Engine ({0})", typeid(*this).name());

		// Remove collision shapes.
		for (std::map<ECS::Entity, btRigidBody*>::iterator it = s_bodies.begin(); it != s_bodies.end(); ++it)
		{
			if (it->second->getMotionState())
			{
				delete it->second->getCollisionShape();
			}
		}

		//remove the rigidbodies from the dynamics world and delete them
		for (int i = m_world->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			btCollisionObject* obj = m_world->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);

			if (body && body->getMotionState())
				delete body->getMotionState();

			m_world->removeCollisionObject(obj);
			delete obj;
		}

		delete m_world;
		delete m_impulseSolver;
		delete m_overlappingPairCache;
		delete m_collisionDispatcher;
		delete m_collisionConfig;
	}

	void BulletPhysicsEngine::Initialize(Lina::ApplicationMode appMode)
	{
		LINA_TRACE("[Initialization] -> Physics Engine ({0})", typeid(*this).name());
		m_appMode = appMode;
		m_ecs = Lina::ECS::Registry::Get();

		if (m_appMode == Lina::ApplicationMode::Editor)
			SetDebugDraw(true);

		// collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
		m_collisionConfig = new btDefaultCollisionConfiguration();

		// use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
		m_collisionDispatcher = new btCollisionDispatcher(m_collisionConfig);

		// btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
		m_overlappingPairCache = new btDbvtBroadphase();

		// the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
		m_impulseSolver = new btSequentialImpulseConstraintSolver;

		// Build dynamics world
		m_world = new btDiscreteDynamicsWorld(m_collisionDispatcher, m_overlappingPairCache, m_impulseSolver, m_collisionConfig);
		m_world->setGravity(btVector3(0, -0.2f, 0));

		// Initialize the debug drawer.
		m_world->setDebugDrawer(&m_gizmoDrawer);
		m_world->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
		m_ecs->on_destroy<ECS::PhysicsComponent>().connect<&BulletPhysicsEngine::OnPhysicsComponentRemoved>(this);

		// Setup rigidbody system and listen to events so that we can refresh bodies when new rigidbodies are created, destroyed etc.
		m_rigidbodySystem.Initialize();
		m_physicsPipeline.AddSystem(m_rigidbodySystem);

		// Engine events.
		m_eventSystem = Event::EventSystem::Get();
		m_eventSystem->Connect<Event::EPostSceneDraw, &BulletPhysicsEngine::OnPostSceneDraw>(this);
	}


	void BulletPhysicsEngine::Tick(float fixedDelta)
	{
		// Update phy.
		m_world->stepSimulation(fixedDelta, 10);
		m_physicsPipeline.UpdateSystems(fixedDelta);
	}

	void BulletPhysicsEngine::Shutdown()
	{
		LINA_TRACE("[Shutdown] -> Physics Engine ({0})", typeid(*this).name());
	}

	void BulletPhysicsEngine::OnPostSceneDraw(Event::EPostSceneDraw)
	{
		if (m_debugDrawEnabled)
			m_world->debugDrawWorld();
	}

	void BulletPhysicsEngine::SetBodySimulation(ECS::Entity body, bool simulate)
	{
		auto& phy = m_ecs->get<ECS::PhysicsComponent>(body);

		// If started simulating.
		if (simulate && !phy.m_wasSimulated)
		{
			AddBodyToWorld(body);
			phy.m_wasSimulated = true;
		}
		else if (!phy.m_isSimulated && phy.m_wasSimulated)
		{
			RemoveBodyFromWorld(body);
			phy.m_wasSimulated = false;
		}

		phy.m_isSimulated = simulate;
	}

	void BulletPhysicsEngine::SetBodyCollisionShape(ECS::Entity body, Physics::CollisionShape shape)
	{
		auto& phy = m_ecs->get<ECS::PhysicsComponent>(body);
		phy.m_collisionShape = shape;

		if (phy.m_isSimulated)
		{
			btRigidBody* rb = s_bodies[body];
			btCollisionShape* previousShape = rb->getCollisionShape();
			rb->setCollisionShape(GetCreateCollisionShape(phy));
			delete previousShape;
		}
	}

	void BulletPhysicsEngine::SetBodyMass(ECS::Entity body, float mass)
	{
		auto& phy = m_ecs->get<ECS::PhysicsComponent>(body);
		phy.m_mass = mass;
		
		if (phy.m_isSimulated)
		{
			btRigidBody* rb = s_bodies[body];
			btVector3 localInertia(0, 0, 0);
			rb->getCollisionShape()->calculateLocalInertia(mass, localInertia);
			rb->setMassProps(btScalar(mass), localInertia);
		}

	}

	void BulletPhysicsEngine::SetBodyRadius(ECS::Entity body, float radius)
	{
		auto& phy = m_ecs->get<ECS::PhysicsComponent>(body);
		phy.m_radius = radius;

		if (phy.m_isSimulated)
		{
			SetBodyCollisionShape(body, phy.m_collisionShape);
		}
	}

	void BulletPhysicsEngine::SetBodyHeight(ECS::Entity body, float height)
	{
		auto& phy = m_ecs->get<ECS::PhysicsComponent>(body);
		phy.m_capsuleHeight = height;

		if (phy.m_isSimulated)
		{
			SetBodyCollisionShape(body, phy.m_collisionShape);
		}
	}

	void BulletPhysicsEngine::SetBodyHalfExtents(ECS::Entity body, const Vector3& extents)
	{
		auto& phy = m_ecs->get<ECS::PhysicsComponent>(body);
		phy.m_halfExtents = extents;

		if (phy.m_isSimulated)
		{
			SetBodyCollisionShape(body, phy.m_collisionShape);
		}
	}


	void BulletPhysicsEngine::OnPhysicsComponentRemoved(entt::registry& reg, entt::entity ent)
	{
		if (s_bodies.find(ent) != s_bodies.end())
			RemoveBodyFromWorld(ent);
	}

	void BulletPhysicsEngine::RemoveBodyFromWorld(ECS::Entity body)
	{
		btRigidBody* rb = s_bodies[body];
		m_world->removeRigidBody(rb);
		delete rb->getMotionState();
		delete rb->getCollisionShape();
		delete rb;
		s_bodies.erase(body);
	}

	void BulletPhysicsEngine::AddBodyToWorld(ECS::Entity body)
	{
		LINA_TRACE("Rigidbody");
		auto& phy = m_ecs->get<ECS::PhysicsComponent>(body);
		auto& data = m_ecs->get<ECS::EntityDataComponent>(body);
		Vector3 location = data.GetLocation();
		Quaternion rotation = data.GetRotation();

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(btVector3(location.x, location.y, location.z));
		transform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
		btScalar mass(phy.m_mass);
		btVector3 localInertia(0, 0, 0);

		// rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.0f);
		btCollisionShape* colShape = GetCreateCollisionShape(phy);
		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);

		btDefaultMotionState* motionState = new btDefaultMotionState(transform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, colShape, localInertia);
		btRigidBody* rb = new btRigidBody(rbInfo);
		rb->setLinearVelocity(ToBtVector(phy.m_velocity));
		rb->setAngularVelocity(ToBtVector(phy.m_angularVelocity));
		rb->setPushVelocity(ToBtVector(phy.m_pushVelocity));
		rb->setTurnVelocity(ToBtVector(phy.m_turnVelocity));

		s_bodies[body] = rb;
		m_world->addRigidBody(rb);
	}

	btCollisionShape* BulletPhysicsEngine::GetCreateCollisionShape(Lina::ECS::PhysicsComponent rb)
	{
		btCollisionShape* colShape = nullptr;

		// Build collision shape depending on the type
		if (rb.m_collisionShape == CollisionShape::Box)
			colShape = new btBoxShape(btVector3(rb.m_halfExtents.x, rb.m_halfExtents.y, rb.m_halfExtents.z));
		else if (rb.m_collisionShape == CollisionShape::Sphere)
			colShape = new btSphereShape(btScalar(rb.m_radius));
		else if (rb.m_collisionShape == CollisionShape::Capsule)
			colShape = new btCapsuleShape(btScalar(rb.m_radius), btScalar(rb.m_capsuleHeight));
		else if (rb.m_collisionShape == CollisionShape::Cylinder)
			colShape = new btCylinderShape(btVector3(rb.m_halfExtents.x, rb.m_halfExtents.y, rb.m_halfExtents.z));

		return colShape;
	}

}

