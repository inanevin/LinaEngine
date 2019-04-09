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

#include "LinaPch.hpp"
#include "Transform.hpp"  
#include "Utility/Math/Matrix.hpp"

namespace LinaEngine
{
	const Matrix4F & Transform::GetWorldTransformation()
	{
		Matrix4F translationM, rotationM, scaleM;

		// Init transform matrices.
		scaleM.InitScaleTransform(scale.GetX(), scale.GetY(), scale.GetZ());
		rotationM.InitRotationTransform(rotation.GetX(), rotation.GetY(), rotation.GetZ());
		translationM.InitTranslationTransform(position.GetY(), position.GetY(), position.GetZ());

		// Multiply, order is hardcoded.
		m_WorldTransformation = translationM * rotationM* scaleM;

		return m_WorldTransformation;
	}

	void Transform::Rotate(const Vector3F& axis, float angle)
	{
		Rotate(Quaternion(axis, angle));
	}

	void Transform::Rotate(const Quaternion& r)
	{
		rotation = Quaternion((r * rotation).Normalized());
	}

	void Transform::LookAt(const Vector3F& point, const Vector3F& up)
	{
		rotation = GetLookAtRotation(point, up);
	}

}

