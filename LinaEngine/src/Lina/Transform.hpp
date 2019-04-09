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

namespace LinaEngine
{


	class Transform
	{
	public:

		Transform(Vector3F pos = Vector3F::ZERO(), Vector3F rot = Vector3F::ZERO(), Vector3F scl = Vector3F::ONE()) : position(pos), rotation(Vector3F(rot.GetX(), rot.GetY(), rot.GetZ()).ToVector()), scale(scl) {};
		Transform(const Transform& rhs) { this->position = rhs.position; this->rotation = rhs.rotation; this->scale = rhs.scale; }

		FORCEINLINE void SetPosition(float x, float y, float z) { position.SetX(x); position.SetY(y); position.SetZ(z); }
		FORCEINLINE void SetPosition(const Vector3F& rhs) { position = rhs; }
		FORCEINLINE void SetPosition(const Vector2F& rhs) { position = Vector3F(rhs.GetX(), rhs.GetY(), 0); }
		FORCEINLINE void SetPositionX(float x) { position.SetX(x); }
		FORCEINLINE void SetPositionY(float y) { position.SetY(y); }
		FORCEINLINE void SetPositionZ(float z) { position.SetZ(z); }
		FORCEINLINE void SetRotation(float x, float y, float z) { rotation = Quaternion(Vector3F(x, y, z).ToVector()); };
		//inline void SetRotation(const Vector3F& rhs) { rotation = rhs; }
		//inline void SetRotation(const Vector2F& rhs) { rotation = Vector3F(rhs.x, rhs.y, 0.0f); }
		//inline void SetRotationX(float x) { rotation.x = x; }
		//inline void SetRotationY(float y) { rotation.y = y; }
		//inline void SetRotationZ(float z) { rotation.z = z; }

		FORCEINLINE void SetScale(float x, float y, float z) { scale.SetX(x);  scale.SetY(y); scale.SetZ(z); };
		FORCEINLINE void SetScale(const Vector3F& rhs) { scale = rhs; }
		FORCEINLINE void SetScale(const Vector2F& rhs) { scale = Vector3F(rhs.GetX(), rhs.GetY(), 1.0f); }
		FORCEINLINE void SetScaleX(float x) { scale.SetX(x); }
		FORCEINLINE void SetScaleY(float y) { scale.SetY(y); }
		FORCEINLINE void SetScaleZ(float z) { scale.SetZ(z); }

		void Rotate(const Vector3F& axis, float angle);
		void Rotate(const Quaternion& rotation);
		void LookAt(const Vector3F& point, const Vector3F& up);

		inline Quaternion GetLookAtRotation(const Vector3F& point, const Vector3F& up)
		{
			return Quaternion(Matrix4F().InitRotationFromDirection((point - position).Normalized(), up));
		}

		const Matrix4F& GetWorldTransformation();
		
		
		Vector3F position;
		Quaternion rotation;
		Vector3F scale;

	private:

		Matrix4F m_WorldTransformation;


	};
}


#endif