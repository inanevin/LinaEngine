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
		Transformation() : m_Translation(0.0f, 0.0f, 0.0f),m_Rotation(0.0f, 0.0f, 0.0f, 1.0f), m_Scale(1.0f, 1.0f, 1.0f) {}
		Transformation(const Vector3F& translationIn) : m_Translation(translationIn), m_Rotation(0.0f, 0.0f, 0.0f, 1.0f), m_Scale(1.0f, 1.0f, 1.0f) {}
		Transformation(const Quaternion& rotationIn) : m_Translation(0.0f, 0.0f, 0.0f), m_Rotation(rotationIn), m_Scale(1.0f, 1.0f, 1.0f) {}
		Transformation(const Vector3F& translationIn, const Quaternion& rotationIn, const Vector3F& scaleIn) : m_Translation(translationIn), m_Rotation(rotationIn), m_Scale(scaleIn) {}

		Matrix Inverse() const;
		// Returns the transform vector based on vector and it's w component.
		FORCEINLINE Vector Transform(const Vector& vector) const
		{
			return Transform(Vector3F(vector), vector[3]);
		}

		// Returns the transform vector based on a vector and a specified w component.
		FORCEINLINE Vector Transform(const Vector3F& vector, float w) const
		{
			return (m_Rotation.Rotate(m_Scale * vector) + m_Translation * w).ToVector(0.0f);
		}

		// Returns inversed transform vector based on a vector and it's w component.
		FORCEINLINE Vector InverseTransform(const Vector& vector) const
		{
			return InverseTransform(Vector3F(vector), vector[3]);
		}

		// Returns the inversed transform vector based on a vector and a specified w component.
		FORCEINLINE Vector InverseTransform(const Vector3F& vector, float w) const
		{
			return (m_Rotation.Inverse().Rotate(vector - m_Translation * w) * m_Scale.Reciprocal()).ToVector(0.0f);
		}

		// Returns the transformation matrix.
		FORCEINLINE Matrix ToMatrix() const
		{
			return Matrix::TransformMatrix(m_Translation, m_Rotation, m_Scale);
		}

		// Normalizes rotation Quaternion.
		FORCEINLINE void NormalizeRotation()
		{
			m_Rotation = m_Rotation.Normalized();
		}

		// Returns whether the rotation is normalized.
		FORCEINLINE bool IsRotationNormalized()
		{
			return m_Rotation.IsNormalized();
		}

		/* ------------------- OPERATOR OVERLOADS ------------------- */

		FORCEINLINE Transformation operator+(const Transformation& other) const
		{
			return Transformation(m_Translation + other.m_Translation, m_Rotation + other.m_Rotation, m_Scale + other.m_Scale);
		}
		FORCEINLINE Transformation operator+=(const Transformation& other)
		{
			m_Translation += other.m_Translation;
			m_Rotation += other.m_Rotation;
			m_Scale += other.m_Scale;
			return *this;
		}
		FORCEINLINE Transformation operator*(const Transformation& other) const
		{
			return Transformation(m_Translation * other.m_Translation, m_Rotation * other.m_Rotation, m_Scale * other.m_Scale);
		}
		FORCEINLINE Transformation operator*=(const Transformation& other)
		{
			m_Translation *= other.m_Translation;
			m_Rotation *= other.m_Rotation;
			m_Scale *= other.m_Scale;
			return *this;
		}
		FORCEINLINE Transformation operator*(float other) const
		{
			return Transformation(m_Translation * other, m_Rotation * other, m_Scale * other);
		}
		FORCEINLINE Transformation operator*=(float other)
		{
			m_Translation *= other;
			m_Rotation *= other;
			m_Scale *= other;
			return *this;
		}

		// Returns the location of this transformation.
		FORCEINLINE Vector3F GetLocation() const 
		{ 
			return m_Translation;  
		}

		// Returns the rotation of this transformation.
		FORCEINLINE Quaternion GetRotation() const
		{
			return m_Rotation;
		}

		// Returns the scale of this transformation.
		FORCEINLINE Vector3F GetScale() const
		{
			return m_Scale;
		}

		// Sets the location, rotation & scale.
		FORCEINLINE void Set(const Vector3F& translationIn, const Quaternion& rotationIn, const Vector3F& scaleIn)
		{
			m_Translation = translationIn;
			m_Rotation = rotationIn;
			m_Scale = scaleIn;
		}

		// Sets the location of this transformation.
		FORCEINLINE void SetLocation(const Vector3F& translation)
		{
			m_Translation = translation;
		}

		// Sets the rotation of this transformation.
		FORCEINLINE void SetRotation(const Quaternion& rotation)
		{
			m_Rotation = rotation;
		}

		// Sets the scale of this transformation.
		FORCEINLINE void SetScale(const Vector3F& scale)
		{
			m_Scale = scale;
		}

		// Sets the rotation of this transformation based on an angle & axis.
		FORCEINLINE void Rotate(const Vector3F& axis, float angle)
		{
			SetRotation(Quaternion(axis, angle));
		}

		// Sets the rotation of this transformation based on an euler vector.
		FORCEINLINE void Rotate(const Vector3F& euler)
		{
			SetRotation(Quaternion::Euler(euler));
		}

	private:

		Vector3F m_Translation = Vector3F::Zero;
		Quaternion m_Rotation;
		Vector3F m_Scale = Vector3F::One;
	};


}


#endif