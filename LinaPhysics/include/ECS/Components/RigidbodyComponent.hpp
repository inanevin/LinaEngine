/*
Author: Inan Evin
www.inanevin.com
https://github.com/inanevin/LinaEngine

Copyright 2020~ Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: RigidbodyComponent
Timestamp: 9/30/2020 2:46:27 AM

*/
#pragma once

#ifndef RigidbodyComponent_HPP
#define RigidbodyComponent_HPP

// Headers here.
#include "ECS/ECSComponent.hpp"
#include "Utility/Math/Vector.hpp"

namespace LinaEngine::ECS
{
	enum class CollisionShape
	{
		SPHERE,
		BOX,
		CYLINDER,
		CAPSULE
	};

	struct RigidbodyComponent : public ECSComponent
	{
		CollisionShape m_collisionShape = CollisionShape::SPHERE;
		LinaEngine::Vector3 m_localInertia = LinaEngine::Vector3::Zero;
		LinaEngine::Vector3 m_halfExtents = LinaEngine::Vector3::Zero; // used for box & cylinder shapes
		float m_mass = 0.0f;
		float m_radius = 0.0f; // used for sphere & capsule shapes.
		float m_capsuleHeight = 0.0f; 
		int m_bodyID = 0;
		bool m_alive = false;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_collisionShape, m_localInertia, m_halfExtents, m_mass, m_radius, m_capsuleHeight, m_bodyID, m_alive, m_isEnabled); // serialize things by passing them to the archive
		}

#ifdef LINA_EDITOR
		COMPONENT_DRAWFUNC_SIG
#endif

	};
}

#endif
