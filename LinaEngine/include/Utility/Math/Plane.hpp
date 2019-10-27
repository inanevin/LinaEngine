/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: Plane
Timestamp: 4/28/2019 2:02:56 AM

*/

#pragma once

#ifndef Plane_HPP
#define Plane_HPP

#include "Vector.hpp"


namespace LinaEngine
{
	class Matrix;
	class Plane
	{
	public:
		FORCEINLINE Plane();
		FORCEINLINE Plane(const Vector& plane);
		FORCEINLINE Plane(float x, float y, float z, float w);
		FORCEINLINE Plane(const Vector3F& normal, float w);
		Plane transform(const Matrix& transform) const;

		FORCEINLINE float dot(const Vector3F& other) const;
		FORCEINLINE float dotVector(const Vector& other) const;
		FORCEINLINE float dotPlane(const Plane& other) const;
		FORCEINLINE Plane normalized(float errorMargin = 1.e-8f) const;
		FORCEINLINE bool isNormalized(float errorMargin = 1.e-4f) const;

		FORCEINLINE Vector3F getNormal() const;
		FORCEINLINE Vector3F reflect(const Vector3F& point) const;

		bool intersectPlanes(Vector3F& intersectionPoint, const Plane& other1, const Plane& other2, float errormargin = 1.e-8f) const;
		FORCEINLINE float intersectLine(const Vector3F& lineStart, const Vector3F& lineEnd) const;
		FORCEINLINE float intersectRay(const Vector3F& start, const Vector3F& rayDir) const;

		FORCEINLINE Plane abs() const;

		FORCEINLINE Plane operator-() const;
		FORCEINLINE Plane operator+(const Plane& other) const;
		FORCEINLINE Plane operator-(const Plane& other) const;
		FORCEINLINE Plane operator*(const Plane& other) const;
		FORCEINLINE Plane operator/(const Plane& other) const;

		FORCEINLINE Plane operator+=(const Plane& other);
		FORCEINLINE Plane operator-=(const Plane& other);
		FORCEINLINE Plane operator*=(const Plane& other);
		FORCEINLINE Plane operator/=(const Plane& other);

		FORCEINLINE Plane operator*(float amt) const;
		FORCEINLINE Plane operator/(float amt) const;
		FORCEINLINE Plane operator*=(float val);
		FORCEINLINE Plane operator/=(float val);

		FORCEINLINE bool operator==(const Plane& other) const;
		FORCEINLINE bool operator!=(const Plane& other) const;
		FORCEINLINE bool equals(const Plane& other, float errorMargin = 1.e-4f) const;
		FORCEINLINE Vector toVector() const;
	private:
		Vector data;
	};

	FORCEINLINE Plane::Plane() {}

	FORCEINLINE Plane::Plane(const Vector& plane) :
		data(plane) {}

	FORCEINLINE Plane::Plane(float x, float y, float z, float w) :
		data(Vector::Make(x, y, z, w)) {}

	FORCEINLINE Plane::Plane(const Vector3F& normal, float w) :
		data(normal.ToVector(w)) {}

	FORCEINLINE float Plane::dot(const Vector3F& other) const
	{
		return other.ToVector(1.0f).Dot4(data)[0];
	}

	FORCEINLINE float Plane::dotVector(const Vector& other) const
	{
		return other.Dot4(data)[0];
	}

	FORCEINLINE float Plane::dotPlane(const Plane& other) const
	{
		return other.data.Dot4(data)[0];
	}

	FORCEINLINE Plane Plane::normalized(float errorMargin) const
	{
		(void)errorMargin;
		return Plane(data.Normalize3());
	}

	FORCEINLINE bool Plane::isNormalized(float errorMargin) const
	{
		return Math::Abs(1.0f - (data.Dot3(data)[0])) < errorMargin;
	}

	FORCEINLINE Vector3F Plane::getNormal() const
	{
		return Vector3F(data.Normalize3());
	}

	FORCEINLINE float Plane::intersectLine(const Vector3F& lineStart, const Vector3F& lineEnd) const
	{
		return intersectRay(lineStart, lineEnd - lineStart);
	}

	FORCEINLINE float Plane::intersectRay(const Vector3F& start, const Vector3F& rayDir) const
	{
		return -dot(start) / rayDir.Dot(getNormal());
	}

	FORCEINLINE Vector3F Plane::reflect(const Vector3F& point) const
	{
		Vector pointAsVector(point.ToVector(1.0f));
		Vector dotAmt = VectorConstants::TWO * data.Dot4(pointAsVector);
		return Vector3F(pointAsVector - (data * dotAmt));
	}

	FORCEINLINE Plane Plane::abs() const
	{
		return Plane(data.Abs());
	}

	FORCEINLINE Plane Plane::operator-() const
	{
		return Plane(-data);
	}

	FORCEINLINE Plane Plane::operator+(const Plane& other) const
	{
		return Plane(data + other.data);
	}

	FORCEINLINE Plane Plane::operator-(const Plane& other) const
	{
		return Plane(data - other.data);
	}

	FORCEINLINE Plane Plane::operator*(const Plane& other) const
	{
		return Plane(data * other.data);
	}

	FORCEINLINE Plane Plane::operator/(const Plane& other) const
	{
		return Plane(data / other.data);
	}


	FORCEINLINE Plane Plane::operator+=(const Plane& other)
	{
		data = data + other.data;
		return *this;
	}

	FORCEINLINE Plane Plane::operator-=(const Plane& other)
	{
		data = data - other.data;
		return *this;
	}

	FORCEINLINE Plane Plane::operator*=(const Plane& other)
	{
		data = data * other.data;
		return *this;
	}

	FORCEINLINE Plane Plane::operator/=(const Plane& other)
	{
		data = data / other.data;
		return *this;
	}


	FORCEINLINE Plane Plane::operator*(float amt) const
	{
		return Plane(data * Vector::Load1F(amt));
	}

	FORCEINLINE Plane Plane::operator/(float amt) const
	{
		return Plane(data * Vector::Load1F(Math::Reciprocal(amt)));
	}

	FORCEINLINE Plane Plane::operator*=(float val)
	{
		data = data * Vector::Load1F(val);
		return *this;
	}

	FORCEINLINE Plane Plane::operator/=(float val)
	{
		data = data * Vector::Load1F(Math::Reciprocal(val));
		return *this;
	}


	FORCEINLINE bool Plane::operator==(const Plane& other) const
	{
		return (data != other.data).IsZero4f();
	}

	FORCEINLINE bool Plane::operator!=(const Plane& other) const
	{
		return (data == other.data).IsZero4f();
	}

	FORCEINLINE bool Plane::equals(const Plane& other, float errorMargin) const
	{
		return data.NotEquals(other.data, errorMargin).IsZero4f();
	}

	FORCEINLINE Vector Plane::toVector() const
	{
		return data;
	}
}


#endif