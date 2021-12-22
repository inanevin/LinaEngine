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
Class: RigidbodyComponent
Timestamp: 9/30/2020 2:46:27 AM
*/

#pragma once

#ifndef PhysicsbodyComponent_HPP
#define PhysicsbodyComponent_HPP

#include "ECS/ECSComponent.hpp"
#include "Math/Vector.hpp"
#include "Core/Common.hpp"

namespace Lina
{
	namespace Editor
	{
		class ComponentDrawer;
	}

	namespace Physics
	{
#ifdef LINA_PHYSICS_BULLET
		class BulletPhysicsEngine;
#elif LINA_PHYSICS_PHYSX
		class PhysXPhysicsEngine;
#endif
	}

	namespace World
	{
		class Level;
	}
};

namespace Lina::ECS
{
	class RigidbodySystem;
	class Registry;

	struct PhysicsComponent : public ECSComponent
	{

		Lina::Vector3 GetVelocity() { return m_velocity; }
		Lina::Vector3 GetAngularVelocity() { return m_angularVelocity; }
		Lina::Vector3 GetHalfExtents() { return m_halfExtents; }
		float GetRadius() { return m_radius; }
		float GetCapsuleHalfHeight() { return m_capsuleHalfHeight; }
		bool GetIsKinematic() { return m_isKinematic; }
		Physics::CollisionShape GetCollisionShape() { return m_collisionShape; }
		Physics::SimulationType GetSimType() { return m_simType; }
		std::string GetMaterialPath() { return m_physicsMaterialPath; }
		StringIDType GetMaterialID() { return m_physicsMaterialID; }
		Lina::Vector3 GetBounds() { return m_bounds; }

	private:

		friend class cereal::access;
		friend class Lina::World::Level;
		friend class Lina::ECS::Registry;
		friend class Lina::Editor::ComponentDrawer;
		friend class Lina::ECS::RigidbodySystem;

#ifdef LINA_PHYSICS_BULLET
		friend class Lina::Physics::BulletPhysicsEngine;
#elif LINA_PHYSICS_PHYSX
		friend class Lina::Physics::PhysXPhysicsEngine;
#endif

		Physics::SimulationType m_simType = Physics::SimulationType::None;
		Physics::CollisionShape m_collisionShape = Physics::CollisionShape::Box;
		Lina::Vector3 m_halfExtents = Lina::Vector3::One; // used for box & cylinder shapes
		float m_mass = 1.0f;
		float m_radius = 1.0f; // used for sphere & capsule shapes.
		float m_capsuleHalfHeight = 1.0f;
		bool m_isKinematic = true;
		std::string m_physicsMaterialPath = "";
		StringIDType m_physicsMaterialID = 0;

		Lina::Vector3 m_bounds = Lina::Vector3::One;
		Lina::Vector3 m_velocity = Lina::Vector3::Zero;
		Lina::Vector3 m_angularVelocity = Lina::Vector3::Zero;

		void ResetRuntimeState()
		{
			m_velocity = Vector3::Zero;
			m_angularVelocity = Vector3::Zero;
			m_simType = Physics::SimulationType::None;
		}

		void Reset()
		{
			m_mass = 1.0f;
			m_radius = 1.0f;
			m_capsuleHalfHeight = 2.0f;
			m_collisionShape = Physics::CollisionShape::Box;
			ResetRuntimeState();
		}

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_collisionShape, m_physicsMaterialPath, m_simType,  m_halfExtents, m_mass, m_radius, m_capsuleHalfHeight, m_isKinematic, m_isEnabled, m_bounds);
		}

	};
}

#endif
