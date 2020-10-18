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

#ifndef RigidbodyComponent_HPP
#define RigidbodyComponent_HPP

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
		bool m_alive = false;
		int m_bodyID = 0;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_collisionShape, m_localInertia, m_halfExtents, m_mass, m_radius, m_capsuleHeight, m_bodyID, m_alive, m_isEnabled); 
		}

#ifdef LINA_EDITOR
		COMPONENT_DRAWFUNC_SIG;
		COMPONENT_ADDFUNC_SIG{ ecs.emplace<RigidbodyComponent>(entity, RigidbodyComponent()); }
#endif

	};
}

#endif
