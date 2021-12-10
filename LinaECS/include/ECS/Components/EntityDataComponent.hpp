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
Class: EntityDataComponent



Timestamp: 12/8/2021 5:35:25 PM
*/

#pragma once

#ifndef EntityDataComponent_HPP
#define EntityDataComponent_HPP

// Headers here.
#include "Utility/Math/Transformation.hpp"
#include "ECS/ECSComponent.hpp"
#include "ECS/ECSSystem.hpp"

namespace Lina::ECS
{
	struct EntityDataComponent : public ECSComponent
	{
		EntityDataComponent(bool hidden, bool enabled, bool serialized, std::string name)
		{
			m_isHidden = hidden;
			m_isEnabled = enabled;
			m_serialized = serialized;
			m_name = name;
		}

		EntityDataComponent() {};

		bool m_isHidden = false;
		bool m_isEnabled = true;
		bool m_serialized = true;
		std::string m_name = "";
		std::set<ECSEntity> m_children;
		ECSEntity m_parent = entt::null;

		/* TRANSFORM OPERATIONS */

		Matrix ToMatrix() { return m_transform.ToMatrix(); }

		void AddRotation(const Vector3& angles);
		void AddLocaRotation(const Vector3& angles);
		void AddLocation(const Vector3& loc);
		void AddLocalLocation(const Vector3& loc);

		Transformation GetInterpolated(float interpolation);
		void SetLocalLocation(const Vector3& loc);
		void SetLocation(const Vector3& loc);
		void SetLocalRotation(const Quaternion& rot, bool isThisPivot = true);
		void SetLocalRotationAngles(const Vector3& angles, bool isThisPivot = true);
		void SetRotation(const Quaternion& rot, bool isThisPivot = true);
		void SetRotationAngles(const Vector3& angles, bool isThisPivot = true);
		void SetLocalScale(const Vector3& scale, bool isThisPivot = true);
		void SetScale(const Vector3& scale, bool isThisPivot = true);

		const Vector3& GetLocalRotationAngles() { return m_transform.m_localRotationAngles; }
		const Vector3& GetLocalLocation() { return m_transform.m_localLocation; }
		const Quaternion& GetLocalRotation() { return m_transform.m_localRotation; }
		const Vector3& GetLocalScale() { return m_transform.m_localScale; }
		const Vector3& GetLocation() { return m_transform.m_location; }
		const Quaternion& GetRotation() { return m_transform.m_rotation; }
		const Vector3& GetRotationAngles() { return m_transform.m_rotationAngles; }
		const Vector3& GetScale() { return m_transform.m_scale; }

	private:

		void UpdateGlobalLocation();
		void UpdateLocalLocation();
		void UpdateGlobalRotation();
		void UpdateLocalRotation();
		void UpdateGlobalScale();
		void UpdateLocalScale();


	private:

		friend class cereal::access;
		friend class ECSRegistry;

		ECSRegistry* m_ecs = nullptr;
		Lina::Transformation m_transform;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_isHidden, m_transform, m_isEnabled, m_name, m_parent, m_children);
		}

	
	};
}

#endif
