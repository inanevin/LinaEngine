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
};

namespace Lina::ECS
{
	class RigidbodySystem;

	struct PhysicsComponent : public ECSComponent
	{

		Lina::Vector3 GetVelocity() { return m_velocity; }
		Lina::Vector3 GetAngularVelocity() { return m_angularVelocity; }
		bool GetIsSimulated() { return m_isSimulated; }

	private:

		friend class cereal::access;
		friend class Lina::Editor::ComponentDrawer;
		friend class Lina::ECS::RigidbodySystem;

#ifdef LINA_PHYSICS_BULLET
		friend class Lina::Physics::BulletPhysicsEngine;
#elif LINA_PHYSICS_PHYSX
		friend class Lina::Physics::PhysXPhysicsEngine;
#endif

		Physics::CollisionShape m_collisionShape = Physics::CollisionShape::Box;
		Lina::Vector3 m_halfExtents = Lina::Vector3::One; // used for box & cylinder shapes
		float m_mass = 1.0f;
		float m_radius = 1.0f; // used for sphere & capsule shapes.
		float m_capsuleHeight = 0.0f;
		bool m_isSimulated = false;
		bool m_isKinematic = true;
		bool m_wasSimulated = false;
		std::string m_physicsMaterialPath = "";
		StringIDType m_physicsMaterialID = 0;

		Physics::CollisionShape m_boundingBox = Physics::CollisionShape::Box;
		Lina::Vector3 m_bbHalfExtents = Lina::Vector3::One;

		Lina::Vector3 m_velocity = Lina::Vector3::Zero;
		Lina::Vector3 m_angularVelocity = Lina::Vector3::Zero;
		Lina::Vector3 m_pushVelocity = Lina::Vector3::Zero;
		Lina::Vector3 m_turnVelocity = Lina::Vector3::Zero;

		void ResetRuntimeState()
		{
			m_velocity = Vector3::Zero;
			m_angularVelocity = Vector3::Zero;
			m_pushVelocity = Vector3::Zero;
			m_turnVelocity = Vector3::Zero;
			m_isSimulated = false;
			m_wasSimulated = false;
		}

		void Reset()
		{
			m_mass = 1.0f;
			m_radius = 1.0f;
			m_capsuleHeight = 2.0f;
			m_collisionShape = Physics::CollisionShape::Box;
			ResetRuntimeState();
		}

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_collisionShape, m_physicsMaterialPath, m_isSimulated, m_halfExtents, m_mass, m_radius, m_capsuleHeight, m_isSimulated, m_isKinematic, m_isEnabled);
		}

	};
}

#endif
