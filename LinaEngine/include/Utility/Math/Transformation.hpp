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

namespace LinaEngine
{
	class Transformation
	{
	public:
		FORCEINLINE Transformation() :
			m_Translation(0.0f, 0.0f, 0.0f),
			m_Rotation(0.0f, 0.0f, 0.0f, 1.0f),
			m_Scale(1.0f, 1.0f, 1.0f) {}

		FORCEINLINE Transformation(const Vector3F& translationIn) :
			m_Translation(translationIn),
			m_Rotation(0.0f, 0.0f, 0.0f, 1.0f),
			m_Scale(1.0f, 1.0f, 1.0f) {}

		FORCEINLINE Transformation(const Quaternion& rotationIn) :
			m_Translation(0.0f, 0.0f, 0.0f),
			m_Rotation(rotationIn),
			m_Scale(1.0f, 1.0f, 1.0f) {}

		FORCEINLINE Transformation(const Vector3F& translationIn, const Quaternion& rotationIn,
			const Vector3F& scaleIn) :
			m_Translation(translationIn),
			m_Rotation(rotationIn),
			m_Scale(scaleIn) {}

		FORCEINLINE Vector Transform(const Vector& vector) const;
		FORCEINLINE Vector Transform(const Vector3F& vector, float w) const;
		FORCEINLINE Vector InverseTransform(const Vector& vector) const;
		FORCEINLINE Vector InverseTransform(const Vector3F& vector, float w) const;
		FORCEINLINE Matrix4F ToMatrix() const;
		Matrix4F Inverse() const;
		FORCEINLINE void NormalizeRotation();
		FORCEINLINE bool IsRotationNormalized();

		FORCEINLINE Transformation operator+(const Transformation& other) const;
		FORCEINLINE Transformation operator+=(const Transformation& other);
		FORCEINLINE Transformation operator*(const Transformation& other) const;
		FORCEINLINE Transformation operator*=(const Transformation& other);
		FORCEINLINE Transformation operator*(float other) const;
		FORCEINLINE Transformation operator*=(float other);

		FORCEINLINE Vector3F GetTranslation() const;
		FORCEINLINE Quaternion GetRotation() const;
		FORCEINLINE Vector3F GetScale() const;
		FORCEINLINE void Set(const Vector3F& translation,
			const Quaternion& rotation, const Vector3F& scale);
		FORCEINLINE void SetTranslation(const Vector3F& translation);
		FORCEINLINE void SetRotation(const Quaternion& rotation);
		FORCEINLINE void SetScale(const Vector3F& scale);

	private:
		Vector3F m_Translation;
		Quaternion m_Rotation;
		Vector3F m_Scale;
	};

	FORCEINLINE Matrix4F Transformation::ToMatrix() const
	{
		return Matrix4F::TransformMatrix(m_Translation, m_Rotation, m_Scale);
	}

	FORCEINLINE void Transformation::NormalizeRotation()
	{
		m_Rotation = m_Rotation.Normalized();
	}

	FORCEINLINE bool Transformation::IsRotationNormalized()
	{
		return m_Rotation.IsNormalized();
	}

	FORCEINLINE Transformation Transformation::operator+(const Transformation& other) const
	{
		return Transformation(m_Translation + other.m_Translation,
			m_Rotation + other.m_Rotation, m_Scale + other.m_Scale);
	}

	FORCEINLINE Transformation Transformation::operator+=(const Transformation& other)
	{
		m_Translation += other.m_Translation;
		m_Rotation += other.m_Rotation;
		m_Scale += other.m_Scale;
		return *this;
	}

	FORCEINLINE Transformation Transformation::operator*(const Transformation& other) const
	{
		return Transformation(m_Translation * other.m_Translation,
			m_Rotation * other.m_Rotation, m_Scale * other.m_Scale);
	}

	FORCEINLINE Transformation Transformation::operator*=(const Transformation& other)
	{
		m_Translation *= other.m_Translation;
		m_Rotation *= other.m_Rotation;
		m_Scale *= other.m_Scale;
		return *this;
	}

	FORCEINLINE Transformation Transformation::operator*(float other) const
	{
		return Transformation(m_Translation * other,
			m_Rotation * other, m_Scale * other);
	}

	FORCEINLINE Transformation Transformation::operator*=(float other)
	{
		m_Translation *= other;
		m_Rotation *= other;
		m_Scale *= other;
		return *this;
	}

	FORCEINLINE Vector Transformation::Transform(const Vector3F& vector, float w) const
	{
		return (m_Rotation.Rotate(m_Scale * vector) + m_Translation * w).ToVector(0.0f);
	}

	FORCEINLINE Vector Transformation::Transform(const Vector& vector) const
	{
		return Transform(Vector3F(vector), vector[3]);
	}

	FORCEINLINE Vector Transformation::InverseTransform(const Vector3F& vector, float w) const
	{
		return (m_Rotation.Inverse().Rotate(vector - m_Translation * w) * m_Scale.Reciprocal()).ToVector(0.0f);
	}

	FORCEINLINE Vector Transformation::InverseTransform(const Vector& vector) const
	{
		return InverseTransform(Vector3F(vector), vector[3]);
	}


	FORCEINLINE Vector3F Transformation::GetTranslation() const
	{
		return m_Translation;
	}

	FORCEINLINE Quaternion Transformation::GetRotation() const
	{
		return m_Rotation;
	}

	FORCEINLINE Vector3F Transformation::GetScale() const
	{
		return m_Scale;
	}

	FORCEINLINE void Transformation::Set(const Vector3F& translationIn,
		const Quaternion& rotationIn, const Vector3F& scaleIn)
	{
		m_Translation = translationIn;
		m_Rotation = rotationIn;
		m_Scale = scaleIn;
	}

	FORCEINLINE void Transformation::SetTranslation(const Vector3F& val)
	{
		m_Translation = val;
	}

	FORCEINLINE void Transformation::SetRotation(const Quaternion& val)
	{
		m_Rotation = val;
	}

	FORCEINLINE void Transformation::SetScale(const Vector3F& val)
	{
		m_Scale = val;
	}

}


#endif