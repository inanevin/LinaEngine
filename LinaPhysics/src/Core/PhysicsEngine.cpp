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

#include "Core/PhysicsEngine.hpp"  
#include "Log/Log.hpp"
#include "ECS/Components/RigidbodyComponent.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Math/Color.hpp"


namespace Lina::Physics
{
	PhysicsEngine::PhysicsEngine()
	{
		LINA_TRACE("[Constructor] -> Physics Engine ({0})", typeid(*this).name());
	}

	PhysicsEngine::~PhysicsEngine()
	{
		LINA_TRACE("[Destructor] -> Physics Engine ({0})", typeid(*this).name());

		// Remove collision shapes.
		for (std::map<int, btRigidBody*>::iterator it = s_bodies.begin(); it != s_bodies.end(); ++it)
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

	void PhysicsEngine::Initialize(Lina::ECS::ECSRegistry& ecsReg, std::function<void(Vector3,Vector3,Color,float)>& cb)
	{
		LINA_TRACE("[Initialization] -> Physics Engine ({0})", typeid(*this).name());

		// Register components.
		ecsReg.RegisterComponentToClone<Lina::ECS::RigidbodyComponent>();

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
		m_world->setGravity(btVector3(0, -10, 0));

		// Initialize the debug drawer.
		m_gizmoDrawer.Setup(cb);
		m_world->setDebugDrawer(&m_gizmoDrawer);
		m_world->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);

		// Setup rigidbody system and listen to events so that we can refresh bodies when new rigidbodies are created, destroyed etc.
		m_rigidbodySystem.Construct(ecsReg, this);
		m_physicsPipeline.AddSystem(m_rigidbodySystem);
		ecsReg.on_construct<Lina::ECS::RigidbodyComponent>().connect<&PhysicsEngine::OnRigidbodyOrTransformAdded>(this);
		ecsReg.on_destroy<Lina::ECS::RigidbodyComponent>().connect<&PhysicsEngine::OnRigidbodyOrTransformAdded>(this);
		ecsReg.on_construct<Lina::ECS::EntityDataComponent>().connect<&PhysicsEngine::OnRigidbodyOrTransformAdded>(this);
		ecsReg.on_update<Lina::ECS::RigidbodyComponent>().connect<&PhysicsEngine::OnRigidbodyUpdated>(this);
		
	}


	void PhysicsEngine::Tick(float fixedDelta)
	{
		// Update physics.
		m_world->stepSimulation(fixedDelta, 10);
		m_physicsPipeline.UpdateSystems(fixedDelta);
	}

	void PhysicsEngine::CleanUp()
	{

	}

	void PhysicsEngine::OnRigidbodyOrTransformAdded(entt::registry& reg, entt::entity ent)
	{
		// If the object doesn't have a transform & rigidbody yet, return.
		if (!reg.all_of<Lina::ECS::EntityDataComponent>(ent) || !reg.all_of<Lina::ECS::RigidbodyComponent>(ent)) return;
		Lina::ECS::RigidbodyComponent& rb = reg.get<Lina::ECS::RigidbodyComponent>(ent);

		// Return if rigidbody is already alive.
		if (rb.m_alive) return;

		Lina::ECS::EntityDataComponent& data = reg.get<Lina::ECS::EntityDataComponent>(ent);
		btCollisionShape* colShape = GetCollisionShape(rb);

		btTransform transform;
		transform.setIdentity();
		Vector3 location = data.GetLocation();
		transform.setOrigin(btVector3(location.x, location.y, location.z));

		btScalar mass(rb.m_mass);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);

		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);

		rb.m_localInertia = Vector3(localInertia.getX(), localInertia.getY(), localInertia.getZ());

		// using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		// Register the body to the map and assign it to the world
		// so that its simulated.
		int id = Lina::Utility::GetUniqueID();
		s_bodies[id] = body;
		rb.m_bodyID = id;

		rb.m_alive = true;
		m_world->addRigidBody(body);
	}

	void PhysicsEngine::OnRigidbodyRemoved(entt::registry& reg, entt::entity ent)
	{
		Lina::ECS::RigidbodyComponent& rbComp = reg.get<Lina::ECS::RigidbodyComponent>(ent);
		btRigidBody* rb = s_bodies[rbComp.m_bodyID];
		
		if (rb->getMotionState())
			delete rb->getMotionState();

		m_world->removeRigidBody(rb);

		s_bodies.erase(rbComp.m_bodyID);
	}

	void PhysicsEngine::OnRigidbodyUpdated(entt::registry& reg, entt::entity ent)
	{
		Lina::ECS::RigidbodyComponent& rbComp = reg.get<Lina::ECS::RigidbodyComponent>(ent);
		btRigidBody* rb = s_bodies[rbComp.m_bodyID];

		// We remove the body, replace the collision shape
		// Recalculate it and add it to the world again.
		rb->activate(true);
		m_world->removeRigidBody(rb);

		btCollisionShape* previousShape = rb->getCollisionShape();
		btCollisionShape* newShape = GetCollisionShape(rbComp);
		rb->setCollisionShape(newShape);

		btVector3 inertia(0, 0, 0);
		newShape->calculateLocalInertia(btScalar(rbComp.m_mass), inertia);

		rb->setMassProps(btScalar(rbComp.m_mass), inertia);
		rb->updateInertiaTensor();

		delete previousShape;

		m_world->addRigidBody(rb);
	}

	void PhysicsEngine::OnPostSceneDraw()
	{
		if (m_debugDrawEnabled)
			m_world->debugDrawWorld();
	}

	btCollisionShape* PhysicsEngine::GetCollisionShape(Lina::ECS::RigidbodyComponent rb)
	{
		btCollisionShape* colShape = nullptr;

		// Build collision shape depending on the type
		if (rb.m_collisionShape == Lina::ECS::CollisionShape::BOX)
			colShape = new btBoxShape(btVector3(rb.m_halfExtents.x, rb.m_halfExtents.y, rb.m_halfExtents.z));
		else if (rb.m_collisionShape == Lina::ECS::CollisionShape::Sphere)
			colShape = new btSphereShape(btScalar(rb.m_radius));
		else if (rb.m_collisionShape == Lina::ECS::CollisionShape::CAPSULE)
			colShape = new btCapsuleShape(btScalar(rb.m_radius), btScalar(rb.m_capsuleHeight));
		else if (rb.m_collisionShape == Lina::ECS::CollisionShape::Cylinder)
			colShape = new btCylinderShape(btVector3(rb.m_halfExtents.x, rb.m_halfExtents.y, rb.m_halfExtents.z));

		return colShape;
	}

}

