/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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

#pragma once

#include "Core/World/Component.hpp"

namespace Lina
{
	class CameraSystem;

	class CameraComponent : public Component
	{
	public:
		virtual void PostTick(float delta) override;

		virtual TypeID GetComponentType() override
		{
			return GetTypeID<CameraComponent>();
		}

		virtual void SaveToStream(OStream& stream) const override
		{
			stream << m_fieldOfView << m_zNear << m_zFar;
		};

		virtual void LoadFromStream(IStream& stream) override
		{
			stream >> m_fieldOfView >> m_zNear >> m_zFar;
		}

		inline const Matrix4& GetProjection()
		{
			return m_projection;
		}

		inline const Matrix4& GetView()
		{
			return m_view;
		}

		inline float GetNear() const
		{
			return m_zNear;
		}

		inline float GetFar() const
		{
			return m_zFar;
		}

	private:
		friend class CameraSystem;
		LINA_REFLECTION_ACCESS(CameraComponent);
		Matrix4	   m_projection	 = Matrix4::Identity();
		Matrix4	   m_view		 = Matrix4::Identity();
		float	   m_fieldOfView = 90.0f;
		float	   m_zNear		 = 0.01f;
		float	   m_zFar		 = 500.0f;
		float	   m_targetX	 = 0.0f;
		float	   m_targetY	 = 0.0f;
		float	   m_targetXPos	 = 0.0f;
		float	   m_targetZPos	 = 0.0f;
		float	   m_angleX		 = 0.0f;
		float	   m_angleY		 = 0.0f;
		Quaternion m_rotation	 = Quaternion();
	};

	LINA_REFLECTCOMPONENT_BEGIN(CameraComponent, "Camera", "Graphics")
	LINA_REFLECT_FIELD(CameraComponent, m_fieldOfView, "FOV", "FLOAT", "", "Field of View Angles", "");
	LINA_REFLECT_FIELD(CameraComponent, m_zNear, "Near Plane", "FLOAT", "", "Near Plane", "");
	LINA_REFLECT_FIELD(CameraComponent, m_zFar, "Far Plane", "FLOAT", "", "Far Plane", "");
	LINA_REFLECTCOMPONENT_END(CameraComponent);
} // namespace Lina
