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
Class: PhysXPhysicsEngine

Responsible for initializing, running and cleaning up the physics world. Also a wrapper for bt3.

Timestamp: 5/1/2019 2:35:28 AM
*/

#pragma once

#ifndef PhysicsEngine_HPP
#define PhysicsEngine_HPP

#include "Core/Common.hpp"
#include "ECS/Systems/RigidbodySystem.hpp"
#include "ECS/Components/AABBComponent.hpp"
#include "ECS/Components/PhysicsComponent.hpp"
#include "Physics/PhysicsMaterial.hpp"

namespace Lina
{
	class Engine;

	namespace Event
	{
		class EventSystem;
	}
}

namespace physx
{
	class PxShape;
	class PxRigidActor;
	class PxActor;
	class PxMaterial;
}

namespace Lina::Physics
{

	class PhysXPhysicsEngine
	{
	public:

		static PhysXPhysicsEngine* Get() { return s_physicsEngine; }

		/// <summary>
		/// Uses the cooking library to create a convex mesh stream using the given vertices.
		/// Stream is placed into the given buffer data for custom serialization.
		/// </summary>
		void CookConvexMesh(std::vector<Vector3>& vertices, std::vector<uint8>& bufferData, StringIDType sid, int nodeID);

		/// <summary>
		/// Given a convex mesh buffer data, creates a convex mesh object and stores it.
		/// </summary>
		void CreateConvexMesh(std::vector<uint8>& data, StringIDType sid, int meshIndex);

		/// <summary>
		/// Returns all moving actor within the Nvidia PhysX scene.
		/// </summary>
		physx::PxActor** GetActiveActors(uint32& size);

		/// <summary>
		/// Given a PxActor, returns the Entity it belongs to.
		/// </summary>
		ECS::Entity GetEntityOfActor(physx::PxActor* actor);

		/// <summary>
		/// Returns all Dynamic bodies, regardless of whether they are static or not.
		/// </summary>
		std::map<ECS::Entity, physx::PxRigidActor*>& GetAllActors();

		/// <summary>
		/// Returns a map of created physics material, the key represents the Lina ID of the PhysicsMaterial object,
		/// value is the PhysX object.
		/// </summary>
		std::map<StringIDType, physx::PxMaterial*>& GetMaterials();


		void SetMaterialStaticFriction(PhysicsMaterial& mat, float friction);
		void SetMaterialDynamicFriction(PhysicsMaterial& mat, float friction);
		void SetMaterialRestitution(PhysicsMaterial& mat, float restitution);
		void SetDebugDraw(bool enabled) { m_debugDrawEnabled = enabled; }
		void SetBodySimulation(ECS::Entity body, SimulationType type);
		void SetBodyCollisionShape(ECS::Entity body, Physics::CollisionShape shape);
		void SetBodyMass(ECS::Entity body, float mass);
		void SetBodyMaterial(ECS::Entity body, const PhysicsMaterial& mat);
		void SetBodyRadius(ECS::Entity body, float radius);
		void SetBodyHeight(ECS::Entity body, float height);
		void SetBodyHalfExtents(ECS::Entity body, const Vector3& extents);
		void SetBodyKinematic(ECS::Entity body, bool kinematic);
		void UpdateBodyShapeParameters(ECS::Entity body);

	private:

		friend class Lina::Engine;
		PhysXPhysicsEngine();
		~PhysXPhysicsEngine();
		void Initialize(Lina::ApplicationMode appMode);
		void Tick(float fixedDelta);
		void Shutdown();

	private:

		void OnPhysicsComponentAdded(entt::registry& reg, entt::entity ent);
		void RecreateBodyShape(ECS::Entity body);
		void OnResourceLoadedFromFile(Event::ELoadResourceFromFile ev);
		void OnResourceLoadedFromMemory(Event::ELoadResourceFromMemory ev);
		void OnLevelInitialized(Event::ELevelInitialized ev);
		void OnPhysicsComponentRemoved(entt::registry& reg, entt::entity ent);
		void OnPostSceneDraw(Event::EPostSceneDraw);
		void OnResourceLoadCompleted(Event::EResourceLoadCompleted ev);
		void RemoveBodyFromWorld(ECS::Entity body);
		void AddBodyToWorld(ECS::Entity body, bool isDynamic);
		physx::PxShape* GetCreateShape(ECS::PhysicsComponent& phy);

	private:

		
		Lina::ECS::Registry* m_ecs = nullptr;
		static PhysXPhysicsEngine* s_physicsEngine;
		Lina::ECS::RigidbodySystem m_rigidbodySystem;
		Lina::ECS::ECSSystemList m_physicsPipeline;
		Lina::Event::EventSystem* m_eventSystem;
		bool m_debugDrawEnabled = false;
		Lina::ApplicationMode m_appMode = Lina::ApplicationMode::Editor;
	};
}


#endif