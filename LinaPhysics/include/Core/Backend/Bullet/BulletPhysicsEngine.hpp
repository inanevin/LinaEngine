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

/*
Class: BulletPhysicsEngine

Responsible for initializing, running and cleaning up the physics world. Also a wrapper for bt3.

Timestamp: 5/1/2019 2:35:28 AM
*/

#pragma once

#ifndef PhysicsEngine_HPP
#define PhysicsEngine_HPP


#define BT_NO_SIMD_OPERATOR_OVERLOADS
#include "ECS/Systems/RigidbodySystem.hpp"
#include "Physics/PhysicsGizmoDrawer.hpp"
#include "ECS/Components/AABBComponent.hpp"
#include "ECS/Components/PhysicsComponent.hpp"
#include "btBulletDynamicsCommon.h"

namespace Lina
{
	class Engine;

	namespace Event
	{
		class EventSystem;
	}
}

namespace Lina::Physics
{

	class BulletPhysicsEngine
	{
	public:

		static BulletPhysicsEngine* Get() { return s_physicsEngine; }

		btRigidBody* GetActiveRigidbody(int id) { return s_bodies[id]; }
		void SetDebugDraw(bool enabled) { m_debugDrawEnabled = enabled; }

	private:
		btCollisionShape* GetCollisionShape(Lina::ECS::PhysicsComponent rb);

	private:
		friend class Lina::Engine;
		BulletPhysicsEngine();
		~BulletPhysicsEngine();
		void Initialize();
		void Tick(float fixedDelta);
		void Shutdown();

	private:

		void OnRigidbodyOrTransformAdded(entt::registry&, entt::entity);
		void OnRigidbodyRemoved(entt::registry&, entt::entity);
		void OnRigidbodyUpdated(entt::registry&, entt::entity);
		void OnPostSceneDraw(Event::EPostSceneDraw);

	private:

		static BulletPhysicsEngine* s_physicsEngine;
		btDefaultCollisionConfiguration* m_collisionConfig = nullptr;
		btCollisionDispatcher* m_collisionDispatcher = nullptr;
		btBroadphaseInterface* m_overlappingPairCache = nullptr;
		btSequentialImpulseConstraintSolver* m_impulseSolver = nullptr;
		btDiscreteDynamicsWorld* m_world = nullptr;
		PhysicsGizmoDrawer m_gizmoDrawer;
		Lina::ECS::RigidbodySystem m_rigidbodySystem;
		Lina::ECS::ECSSystemList m_physicsPipeline;
		Lina::Event::EventSystem* m_eventSystem;
		std::map<int, btRigidBody*> s_bodies;		
		bool m_debugDrawEnabled = false;
	};
}


#endif