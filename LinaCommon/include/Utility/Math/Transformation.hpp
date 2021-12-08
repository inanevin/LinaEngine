/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

/*
Class: Transformation

m_data structure for representing transformations.

Timestamp: 4/9/2019 12:06:04 PM

*/

#pragma once

#ifndef Transformation_HPP
#define Transformation_HPP

#include "Quaternion.hpp"
#include "Matrix.hpp"
#include <set>

namespace LinaEngine
{
	namespace ECS
	{
		class ECSRegistry;
	}
}

namespace LinaEngine
{
	class Transformation
	{
	public:

		Transformation() : m_location(0.0f, 0.0f, 0.0f), m_rotation(0.0f, 0.0f, 0.0f, 1.0f), m_scale(1.0f, 1.0f, 1.0f) {}
		Transformation(const Vector3& translationIn) : m_location(translationIn), m_rotation(0.0f, 0.0f, 0.0f, 1.0f), m_scale(1.0f, 1.0f, 1.0f) {}
		Transformation(const Quaternion& rotationIn) : m_location(0.0f, 0.0f, 0.0f), m_rotation(rotationIn), m_scale(1.0f, 1.0f, 1.0f) {}
		Transformation(const Vector3& translationIn, const Quaternion& rotationIn, const Vector3& scaleIn) : m_location(translationIn), m_rotation(rotationIn), m_scale(scaleIn) {}

		static Transformation Interpolate(Transformation& from, Transformation& to, float t);

		Matrix ToMatrix() const
		{
			return Matrix::TransformMatrix(m_location, m_rotation, m_scale);
		}

		Matrix ToLocalMatrix() const
		{
			return Matrix::TransformMatrix(m_localLocation, m_localRotation, m_localScale);
		}


		Vector3 m_location = Vector3::Zero;
		Quaternion m_rotation;
		Vector3 m_rotationAngles = Vector3::Zero;
		Vector3 m_scale = Vector3::One;
		Vector3 m_localLocation = Vector3::Zero;
		Quaternion m_localRotation;
		Vector3 m_localScale = Vector3::One;
		Vector3 m_localRotationAngles = Vector3::Zero;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_location, m_rotation, m_scale, m_localLocation, m_localRotation, m_localScale, m_localRotationAngles, m_rotationAngles);
		}

	private:
		friend class ECS::ECSRegistry;

	};


}


#endif