/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: Transformation
Timestamp: 4/9/2019 12:06:04 PM

*/

#pragma once

#ifndef Transformation_HPP
#define Transformation_HPP

#include "Quaternion.hpp"
#include "Matrix.hpp"

namespace LinaEngine
{
	class Transformation
	{
	public:

		// Param constructors w/ various options.
		Transformation() : location(0.0f, 0.0f, 0.0f),rotation(0.0f, 0.0f, 0.0f, 1.0f), scale(1.0f, 1.0f, 1.0f) {}
		Transformation(const Vector3& translationIn) : location(translationIn), rotation(0.0f, 0.0f, 0.0f, 1.0f), scale(1.0f, 1.0f, 1.0f) {}
		Transformation(const Quaternion& rotationIn) : location(0.0f, 0.0f, 0.0f), rotation(rotationIn), scale(1.0f, 1.0f, 1.0f) {}
		Transformation(const Vector3& translationIn, const Quaternion& rotationIn, const Vector3& scaleIn) : location(translationIn), rotation(rotationIn), scale(scaleIn) {}

		// Returns the transformation matrix.
		FORCEINLINE Matrix ToMatrix() const
		{
			return Matrix::TransformMatrix(location, rotation, scale);
		}

		// Normalizes rotation Quaternion.
		FORCEINLINE void NormalizeRotation()
		{
			rotation = rotation.Normalized();
		}

		// Returns whether the rotation is normalized.
		FORCEINLINE bool IsRotationNormalized()
		{
			return rotation.IsNormalized();
		}

		/* ------------------- OPERATOR OVERLOADS ------------------- */

		FORCEINLINE Transformation operator+(const Transformation& other) const
		{
			return Transformation(location + other.location, rotation + other.rotation, scale + other.scale);
		}

		FORCEINLINE Transformation operator+=(const Transformation& other)
		{
			location += other.location;
			rotation += other.rotation;
			scale += other.scale;
			return *this;
		}

		FORCEINLINE Transformation operator*(const Transformation& other) const
		{
			return Transformation(location * other.location, rotation * other.rotation, scale * other.scale);
		}

		FORCEINLINE Transformation operator*=(const Transformation& other)
		{
			location *= other.location;
			rotation *= other.rotation;
			scale *= other.scale;
			return *this;
		}

		FORCEINLINE Transformation operator*(float other) const
		{
			return Transformation(location * other, rotation * other, scale * other);
		}

		FORCEINLINE Transformation operator*=(float other)
		{
			location *= other;
			rotation *= other;
			scale *= other;
			return *this;
		}
	
		// Sets the location, rotation & scale.
		FORCEINLINE void Set(const Vector3& translationIn, const Quaternion& rotationIn, const Vector3& scaleIn)
		{
			location = translationIn;
			rotation = rotationIn;
			scale = scaleIn;
		}

		// Sets the rotation of this transformation based on an angle & axis.
		FORCEINLINE void Rotate(const Vector3& axis, float angle)
		{
			rotation = Quaternion(axis, angle);
		}

		// Sets the rotation of this transformation based on an euler vector.
		FORCEINLINE void Rotate(const Vector3& euler)
		{
			rotation = Quaternion::Euler(euler.x, euler.y, euler.z);
		}

		// Sets the rotation of this transformation based on an euler vector.
		FORCEINLINE void Rotate(float x, float y, float z)
		{
			rotation = Quaternion::Euler(x,y,z);
		}



		Vector3 location = Vector3::Zero;
		Quaternion rotation;
		Vector3 scale = Vector3::One;

	private:

		
	};


}


#endif