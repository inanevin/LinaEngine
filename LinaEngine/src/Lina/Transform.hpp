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

		Transform(Vector3F pos = Vector3F::Zero(), Vector3F rot = Vector3F::Zero(), Vector3F scl = Vector3F::One()) : position(pos), rotation(rot.x, rot.y, rot.z), scale(scl) {};
		Transform(const Transform& rhs) { this->position = rhs.position; this->rotation = rhs.rotation; this->scale = rhs.scale; }

		inline void SetPosition(float x, float y, float z) { position.x = x; position.y = y; position.z = z; }
		inline void SetPosition(const Vector3F& rhs) { position = rhs; }
		inline void SetPosition(const Vector2F& rhs) { position = Vector3F(rhs.x, rhs.y, 0); }
		inline void SetPositionX(float x) { position.x = x; }
		inline void SetPositionY(float y) { position.y = y; }
		inline void SetPositionZ(float z) { position.z = z; }

		inline void SetRotation(float x, float y, float z) { rotation.x = x; rotation.y = y; rotation.z = z; };
		//inline void SetRotation(const Vector3F& rhs) { rotation = rhs; }
		//inline void SetRotation(const Vector2F& rhs) { rotation = Vector3F(rhs.x, rhs.y, 0.0f); }
		inline void SetRotationX(float x) { rotation.x = x; }
		inline void SetRotationY(float y) { rotation.y = y; }
		inline void SetRotationZ(float z) { rotation.z = z; }

		inline void SetScale(float x, float y, float z) { scale.x = x; scale.y = y; scale.z = z; };
		inline void SetScale(const Vector3F& rhs) { scale = rhs; }
		inline void SetScale(const Vector2F& rhs) { scale = Vector3F(rhs.x, rhs.y, 1.0f); }
		inline void SetScaleX(float x) { scale.x = x; }
		inline void SetScaleY(float y) { scale.y = y; }
		inline void SetScaleZ(float z) { scale.z = z; }

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