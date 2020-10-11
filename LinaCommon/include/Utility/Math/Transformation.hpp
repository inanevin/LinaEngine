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
		Transformation() : m_location(0.0f, 0.0f, 0.0f),m_rotation(0.0f, 0.0f, 0.0f, 1.0f), m_scale(1.0f, 1.0f, 1.0f) {}
		Transformation(const Vector3& translationIn) : m_location(translationIn), m_rotation(0.0f, 0.0f, 0.0f, 1.0f), m_scale(1.0f, 1.0f, 1.0f) {}
		Transformation(const Quaternion& rotationIn) : m_location(0.0f, 0.0f, 0.0f), m_rotation(rotationIn), m_scale(1.0f, 1.0f, 1.0f) {}
		Transformation(const Vector3& translationIn, const Quaternion& rotationIn, const Vector3& scaleIn) : m_location(translationIn), m_rotation(rotationIn), m_scale(scaleIn) {}
		
		// Returns interpolated transformation
		static Transformation Interpolate(Transformation& from, Transformation& to, float t);

		// Returns the transformation matrix.
		FORCEINLINE Matrix ToMatrix() const
		{
			return Matrix::TransformMatrix(m_location, m_rotation, m_scale);
		}

		// Normalizes rotation Quaternion.
		FORCEINLINE void NormalizeRotation()
		{
			m_rotation = m_rotation.Normalized();
		}

		// Returns whether the rotation is normalized.
		FORCEINLINE bool IsRotationNormalized()
		{
			return m_rotation.IsNormalized();
		}

		/* ------------------- OPERATOR OVERLOADS ------------------- */

		FORCEINLINE Transformation operator+(const Transformation& other) const
		{
			return Transformation(m_location + other.m_location, m_rotation + other.m_rotation, m_scale + other.m_scale);
		}

		FORCEINLINE Transformation operator+=(const Transformation& other)
		{
			m_location += other.m_location;
			m_rotation += other.m_rotation;
			m_scale += other.m_scale;
			return *this;
		}

		FORCEINLINE Transformation operator*(const Transformation& other) const
		{
			return Transformation(m_location * other.m_location, m_rotation * other.m_rotation, m_scale * other.m_scale);
		}

		FORCEINLINE Transformation operator*=(const Transformation& other)
		{
			m_location *= other.m_location;
			m_rotation *= other.m_rotation;
			m_scale *= other.m_scale;
			return *this;
		}

		FORCEINLINE Transformation operator*(float other) const
		{
			return Transformation(m_location * other, m_rotation * other, m_scale * other);
		}

		FORCEINLINE Transformation operator*=(float other)
		{
			m_location *= other;
			m_rotation *= other;
			m_scale *= other;
			return *this;
		}
	
		// Sets the location, rotation & scale.
		FORCEINLINE void Set(const Vector3& translationIn, const Quaternion& rotationIn, const Vector3& scaleIn)
		{
			m_location = translationIn;
			m_rotation = rotationIn;
			m_scale = scaleIn;
		}

		// Sets the rotation of this transformation based on an angle & axis.
		FORCEINLINE void Rotate(const Vector3& axis, float angle)
		{
			m_rotation = Quaternion(axis, angle);
		}

		// Sets the rotation of this transformation based on an euler vector.
		FORCEINLINE void Rotate(const Vector3& euler)
		{
			m_rotation = Quaternion::Euler(euler.x, euler.y, euler.z);
		}

		// Sets the rotation of this transformation based on an euler vector.
		FORCEINLINE void Rotate(float x, float y, float z)
		{
			m_rotation = Quaternion::Euler(x,y,z);
		}

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_location, m_rotation, m_scale); // serialize things by passing them to the archive
		}

		Vector3 m_location = Vector3::Zero;
		Quaternion m_rotation;
		Vector3 m_scale = Vector3::One;

	private:

		
	};


}


#endif