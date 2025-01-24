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

#include "Matrix.hpp"
#include "Quaternion.hpp"
#include "Vector.hpp"
#include "Common/Data/Set.hpp"

namespace Lina
{
	namespace ECS
	{
		class Registry;
	}
} // namespace Lina

namespace Lina
{
	class Transformation
	{
	public:
		Transformation() : m_position(0.0f, 0.0f, 0.0f), m_rotation(0.0f, 0.0f, 0.0f, 1.0f), m_scale(1.0f, 1.0f, 1.0f)
		{
		}
		Transformation(const Vector3& translationIn) : m_position(translationIn), m_rotation(0.0f, 0.0f, 0.0f, 1.0f), m_scale(1.0f, 1.0f, 1.0f)
		{
		}
		Transformation(const Quaternion& rotationIn) : m_position(0.0f, 0.0f, 0.0f), m_rotation(rotationIn), m_scale(1.0f, 1.0f, 1.0f)
		{
		}
		Transformation(const Vector3& translationIn, const Quaternion& rotationIn, const Vector3& scaleIn) : m_position(translationIn), m_rotation(rotationIn), m_scale(scaleIn)
		{
		}

		static Transformation Interpolate(const Transformation& from, const Transformation& to, float t);

		void SetLocalMatrix(const Matrix4& mat);
		void SetGlobalMatrix(const Matrix4& mat);
		void UpdateLocalMatrix();
		void UpdateGlobalMatrix();
		void SaveToStream(OStream& stream) const;
		void LoadFromStream(IStream& stream);

		inline Matrix4 ToMatrix() const
		{
			return Matrix4::TransformMatrix(m_position, m_rotation, m_scale);
		}

		inline Matrix4 ToLocalMatrix() const
		{
			return Matrix4::TransformMatrix(m_localPosition, m_localRotation, m_localScale);
		}

		inline const Matrix4& GetMatrix() const
		{
			return m_matrix;
		}

		inline const Matrix4& GetLocalMatrix() const
		{
			return m_localMatrix;
		}

		inline void SetPosition(const Vector3& v)
		{
			m_position = v;
			UpdateGlobalMatrix();
		}

		inline void SetLocalPosition(const Vector3& v)
		{
			m_localPosition = v;
			UpdateLocalMatrix();
		}

		inline void SetScale(const Vector3& v)
		{
			m_scale = v;
			UpdateGlobalMatrix();
		}

		inline void SetLocalScale(const Vector3& v)
		{
			m_localScale = v;
			UpdateLocalMatrix();
		}

		inline void SetRotation(const Quaternion& q)
		{
			m_rotation = q;
			UpdateGlobalMatrix();
		}

		inline void SetLocalRotation(const Quaternion& q)
		{
			m_localRotation = q;
			UpdateLocalMatrix();
		}

		inline void SetRotationAngles(const Vector3& v)
		{
			m_rotationAngles = v;
		}

		inline void SetLocalRotationAngles(const Vector3& v)
		{
			m_localRotationAngles = v;
		}

		inline const Vector3& GetPosition() const
		{
			return m_position;
		}

		inline const Vector3& GetLocalPosition() const
		{
			return m_localPosition;
		}

		inline const Vector3& GetScale() const
		{
			return m_scale;
		}

		inline const Vector3& GetLocalScale() const
		{
			return m_localScale;
		}

		inline const Quaternion& GetRotation() const
		{
			return m_rotation;
		}

		inline const Quaternion& GetLocalRotation() const
		{
			return m_localRotation;
		}

		inline const Vector3& GetRotationAngles() const
		{
			return m_rotationAngles;
		}

		inline const Vector3& GetLocalRotationAngles() const
		{
			return m_localRotationAngles;
		}

	private:
		Vector3	   m_position = Vector3::Zero;
		Quaternion m_rotation;
		Vector3	   m_rotationAngles = Vector3::Zero;
		Vector3	   m_scale			= Vector3::One;
		Vector3	   m_localPosition	= Vector3::Zero;
		Quaternion m_localRotation;
		Vector3	   m_localScale			 = Vector3::One;
		Vector3	   m_localRotationAngles = Vector3::Zero;
		Matrix4	   m_matrix				 = Matrix4::Identity();
		Matrix4	   m_localMatrix		 = Matrix4::Identity();
	};

} // namespace Lina
