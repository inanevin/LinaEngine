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
	class PxRigidDynamic;
	class PxActor;
	class PxRigidStatic;
	class PxMaterial;
}

namespace Lina::Physics
{

	class PhysXPhysicsEngine
	{
	public:

		static PhysXPhysicsEngine* Get() { return s_physicsEngine; }

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
		physx::PxActor** GetActiveActors(uint32& size);
		ECS::Entity GetEntityOfActor(physx::PxActor* actor);
		std::map<ECS::Entity, physx::PxRigidDynamic*>& GetAllDynamicActors();
		std::map<ECS::Entity, physx::PxRigidStatic*>& GetAllStaticActors();
		std::map<StringIDType, physx::PxMaterial*>& GetMaterials();

	private:
		friend class Lina::Engine;
		PhysXPhysicsEngine();
		~PhysXPhysicsEngine();
		void Initialize(Lina::ApplicationMode appMode);
		void Tick(float fixedDelta);
		void Shutdown();

	private:

		void OnPhysicsComponentAdded(entt::registry& reg, entt::entity ent);
		void UpdateShapeMaterials(ECS::Entity body, physx::PxShape* shape);
		void UpdateBodyShape(ECS::Entity body);
		void OnResourceLoadedFromFile(Event::ELoadResourceFromFile ev);
		void OnResourceLoadedFromMemory(Event::ELoadResourceFromMemory ev);
		void OnLevelInitialized(Event::ELevelInitialized ev);
		void OnPhysicsComponentRemoved(entt::registry& reg, entt::entity ent);
		void OnPostSceneDraw(Event::EPostSceneDraw);
		void RemoveBodyFromWorld(ECS::Entity body, bool isDynamic);
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