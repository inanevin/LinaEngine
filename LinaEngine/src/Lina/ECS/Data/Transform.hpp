/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: Transform
Timestamp: 1/5/2019 7:52:35 PM

*/

#pragma once

#ifndef Transform_HPP
#define Transform_HPP

#include "Lina/Utility/Math/Vector.hpp"
#include "Lina/Utility/Math/Matrix.hpp"
namespace LinaEngine
{


	class Transform
	{
	public:

		Transform(Vector3F pos = Vector3F::ZERO(), Vector3F rot = Vector3F::ZERO(), Vector3F scl = Vector3F::ONE()) : m_Position(pos), m_Rotation(Vector3F(rot.GetX(), rot.GetY(), rot.GetZ()).ToVector()), m_Scale(scl) {};
		Transform(const Transform& rhs) { this->m_Position = rhs.m_Position; this->m_Rotation = rhs.m_Rotation; this->m_Scale = rhs.m_Scale; }

		FORCEINLINE void SetPosition(float x, float y, float z) { m_Position.SetX(x); m_Position.SetY(y); m_Position.SetZ(z); }
		FORCEINLINE void SetPosition(const Vector3F& rhs) { m_Position = rhs; }
		FORCEINLINE void SetPosition(const Vector2F& rhs) { m_Position = Vector3F(rhs.GetX(), rhs.GetY(), 0); }
		FORCEINLINE void SetPositionX(float x) { m_Position.SetX(x); }
		FORCEINLINE void SetPositionY(float y) { m_Position.SetY(y); }
		FORCEINLINE void SetPositionZ(float z) { m_Position.SetZ(z); }
		FORCEINLINE void SetRotation(float x, float y, float z) { m_Rotation = Quaternion(Vector3F(x, y, z).ToVector()); };
		FORCEINLINE void SetRotation(const Vector3F rhs) { m_Rotation = Quaternion(rhs.ToVector()); }
		FORCEINLINE void SetRotation(const Vector2F rhs) { m_Rotation = Quaternion(rhs.ToVector()); }
		FORCEINLINE void SetRotationX(float x) { m_Rotation = Quaternion(Vector3F(x, 0.0f, 0.0f).ToVector()); }
		FORCEINLINE void SetRotationY(float y) { m_Rotation = Quaternion(Vector3F(0.0f, y, 0.0f).ToVector()); }
		FORCEINLINE void SetRotationz(float z) { m_Rotation = Quaternion(Vector3F(0.0f, 0.0f, z).ToVector()); }
		FORCEINLINE void SetScale(float x, float y, float z) { m_Scale.SetX(x);  m_Scale.SetY(y); m_Scale.SetZ(z); };
		FORCEINLINE void SetScale(const Vector3F& rhs) { m_Scale = rhs; }
		FORCEINLINE void SetScale(const Vector2F& rhs) { m_Scale = Vector3F(rhs.GetX(), rhs.GetY(), 1.0f); }
		FORCEINLINE void SetScaleX(float x) { m_Scale.SetX(x); }
		FORCEINLINE void SetScaleY(float y) { m_Scale.SetY(y); }
		FORCEINLINE void SetScaleZ(float z) { m_Scale.SetZ(z); }
		FORCEINLINE Vector3F GetPosition() const { return m_Position; }
		FORCEINLINE Vector3F GetScale() const { return m_Scale; }
		FORCEINLINE Quaternion GetRotation() const { return m_Rotation; }
		FORCEINLINE float GetPositionX() const { return m_Position.GetX(); }
		FORCEINLINE float GetPositionY() const { return m_Position.GetY(); }
		FORCEINLINE float GetPositionZ() const { return m_Position.GetZ(); }
		FORCEINLINE float GetScaleX() const { return m_Scale.GetX(); }
		FORCEINLINE float GetScaleY() const { return m_Scale.GetY(); }
		FORCEINLINE float GetScaleZ() const { return m_Scale.GetZ(); }
		FORCEINLINE float GetQuaternionX() const { return m_Rotation.GetX(); }
		FORCEINLINE float GetQuaternionY() const { return m_Rotation.GetY(); }
		FORCEINLINE float GetQuaternionZ() const { return m_Rotation.GetZ(); }

		void Rotate(const Vector3F& axis, float angle);
		void Rotate(const Quaternion& rotation);
		void LookAt(const Vector3F& point, const Vector3F& up);

		FORCEINLINE Quaternion GetLookAtRotation(const Vector3F& point, const Vector3F& up)
		{
			return Quaternion(Matrix4F().InitRotationFromDirection((point - m_Position).Normalized(), up));
		}

		const Matrix4F& GetWorldTransformation();
	private:
		Vector3F m_Position;
		Quaternion m_Rotation;
		Vector3F m_Scale;
		Matrix4F m_WorldTransformation;
	};
}


#endif