/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: PhysicsEngine
Timestamp: 5/1/2019 2:35:28 AM

*/

#pragma once

#ifndef PhysicsEngine_HPP
#define PhysicsEngine_HPP

#include "ECS/Systems/RigidbodySystem.hpp"
#include "PhysicsGizmoDrawer.hpp"
#include "btBulletDynamicsCommon.h"

namespace LinaEngine
{
	namespace ECS
	{
		struct TransformComponent;
		struct RigidbodyComponent;
	}

	namespace Graphics
	{
		class RenderEngine;
	}
}

namespace LinaEngine::Physics
{

	class PhysicsEngine
	{
	public:

		PhysicsEngine();
		~PhysicsEngine();

		// Initializes the physics engine.
		void Initialize(LinaEngine::ECS::ECSRegistry& ecsReg, std::function<void(Vector3, Vector3, Color, float)>& cb);

		// Update physics systems.
		void Tick(float fixedDelta);

		// Termination.
		void CleanUp();

		// Called when rigidbody components are added/removed from an entity.
		void OnRigidbodyOrTransformAdded(entt::registry&, entt::entity);
		void OnRigidbodyRemoved(entt::registry&, entt::entity);

		// Returns an active rigidbody.
		btRigidBody* GetActiveRigidbody(int id) { return m_bodies[id]; }

	private:


	private:

		btDefaultCollisionConfiguration* m_collisionConfig = nullptr;
		btCollisionDispatcher* m_collisionDispatcher = nullptr;
		btBroadphaseInterface* m_overlappingPairCache = nullptr;
		btSequentialImpulseConstraintSolver* m_impulseSolver = nullptr;
		btDiscreteDynamicsWorld* m_world = nullptr;

		// Gizmo drawer
		PhysicsGizmoDrawer m_gizmoDrawer;

		// ECS system for handling rigidbody physics
		LinaEngine::ECS::RigidbodySystem m_rigidbodySystem;

		// ECS system list for physics operations.
		LinaEngine::ECS::ECSSystemList m_physicsPipeline;

		// Map for keeping track of bodies
		std::map<int, btRigidBody*> m_bodies;
	};
}


#endif