/*
Author: Inan Evin
www.inanevin.com

MIT License

Lina Engine, Copyright (c) 2018 Inan Evin

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

4.0.30319.42000
10/29/2018 11:04:54 PM

*/

#pragma once

#ifndef Lina_Vector_HPP
#define Lina_Vector_HPP

class Lina_Quaternion;


class Lina_Vector4F
{
public:

	Lina_Vector4F() {};
	Lina_Vector4F(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) { };

	float Max() const;
	float MagnitudeSq() const;
	float Magnitude() const;
	float AngleBetween(const Lina_Vector4F& rhs) const;
	float Dot(const Lina_Vector4F& rhs) const;

	Lina_Vector4F Reflect(const Lina_Vector4F& normal) const;
	Lina_Vector4F Max(const Lina_Vector4F& rhs) const;
	Lina_Vector4F Normalized() const;
	Lina_Vector4F Lerp(const Lina_Vector4F& rhs, float lerpFactor) const;
	Lina_Vector4F Project(const Lina_Vector4F& rhs) const;
	void Normalize() const;


	inline Lina_Vector4F operator+(const Lina_Vector4F& rhs) const;
	inline Lina_Vector4F operator-(const Lina_Vector4F& rhs) const;
	inline Lina_Vector4F operator*(const Lina_Vector4F&) const;
	inline Lina_Vector4F operator/(const Lina_Vector4F&) const;
					  
	inline Lina_Vector4F operator+(const float& rhs) const;
	inline Lina_Vector4F operator-(const float& rhs) const;
	inline Lina_Vector4F operator*(const float& rhs) const;
	inline Lina_Vector4F operator/(const float& rhs) const;
					  
	inline Lina_Vector4F& operator+=(const Lina_Vector4F& rhs);
	inline Lina_Vector4F& operator-=(const Lina_Vector4F& rhs);
	inline Lina_Vector4F& operator*=(const Lina_Vector4F& rhs);
	inline Lina_Vector4F& operator/=(const Lina_Vector4F& rhs);
					  
	inline Lina_Vector4F& operator+=(const float& rhs);
	inline Lina_Vector4F& operator-=(const float& rhs);
	inline Lina_Vector4F& operator*=(const float& rhs);
	inline Lina_Vector4F& operator/=(const float& rhs);

	bool operator==(const Lina_Vector4F& rhs) const;
	bool operator!=(const Lina_Vector4F& rhs) const;

	static Lina_Vector4F One();
	static Lina_Vector4F Zero();

private:

	float x, y, z, w;
};

class Lina_Vector3F 
{
public:

	Lina_Vector3F() {};

	float Max() const;
	float MagnitudeSq() const;
	float Magnitude() const;
	float AngleBetween(const Lina_Vector3F& rhs) const;
	float Dot(const Lina_Vector3F& rhs) const;

	Lina_Vector3F Reflect(const Lina_Vector3F& normal) const;
	Lina_Vector3F Max(const Lina_Vector3F& rhs) const;
	Lina_Vector3F Normalized() const;
	Lina_Vector3F Lerp(const Lina_Vector3F& rhs, float lerpFactor) const;
	Lina_Vector3F Project(const Lina_Vector3F& rhs) const;
	void Normalize() const;

	bool operator==(const Lina_Vector3F& rhs) const;
	bool operator!=(const Lina_Vector3F& rhs) const;

	inline Lina_Vector3F operator+(const Lina_Vector3F& rhs) const;
	inline Lina_Vector3F operator-(const Lina_Vector3F& rhs) const;
	inline Lina_Vector3F operator*(const Lina_Vector3F&) const;
	inline Lina_Vector3F operator/(const Lina_Vector3F&) const;
					  
	inline Lina_Vector3F operator+(const float& rhs) const;
	inline Lina_Vector3F operator-(const float& rhs) const;
	inline Lina_Vector3F operator*(const float& rhs) const;
	inline Lina_Vector3F operator/(const float& rhs) const;
					  
	inline Lina_Vector3F& operator+=(const Lina_Vector3F& rhs);
	inline Lina_Vector3F& operator-=(const Lina_Vector3F& rhs);
	inline Lina_Vector3F& operator*=(const Lina_Vector3F& rhs);
	inline Lina_Vector3F& operator/=(const Lina_Vector3F& rhs);
					  
	inline Lina_Vector3F& operator+=(const float& rhs);
	inline Lina_Vector3F& operator-=(const float& rhs);
	inline Lina_Vector3F& operator*=(const float& rhs);
	inline Lina_Vector3F& operator/=(const float& rhs);

	static Lina_Vector3F One();
	static Lina_Vector3F Zero();
};

class Lina_Vector2F 
{
public:

	Lina_Vector2F() {};

	float Max() const;
	float MagnitudeSq() const;
	float Magnitude() const;
	float AngleBetween(const Lina_Vector2F& rhs) const;
	float Dot(const Lina_Vector2F& rhs) const;
	float Cross(const Lina_Vector2F& rhs) const;

	Lina_Vector2F Reflect(const Lina_Vector2F& normal) const;
	Lina_Vector2F Max(const Lina_Vector2F& rhs) const;
	Lina_Vector2F Normalized() const;
	Lina_Vector2F Lerp(const Lina_Vector2F& rhs, float lerpFactor) const;
	Lina_Vector2F Project(const Lina_Vector2F& rhs) const;
	void Normalize() const;

	bool operator==(const Lina_Vector2F& rhs) const;
	bool operator!=(const Lina_Vector2F& rhs) const;

	inline Lina_Vector2F operator+(const Lina_Vector2F& rhs) const;
	inline Lina_Vector2F operator-(const Lina_Vector2F& rhs) const;
	inline Lina_Vector2F operator*(const Lina_Vector2F&) const;
	inline Lina_Vector2F operator/(const Lina_Vector2F&) const;
					  
	inline Lina_Vector2F operator+(const float& rhs) const;
	inline Lina_Vector2F operator-(const float& rhs) const;
	inline Lina_Vector2F operator*(const float& rhs) const;
	inline Lina_Vector2F operator/(const float& rhs) const;
					  
	inline Lina_Vector2F& operator+=(const Lina_Vector2F& rhs);
	inline Lina_Vector2F& operator-=(const Lina_Vector2F& rhs);
	inline Lina_Vector2F& operator*=(const Lina_Vector2F& rhs);
	inline Lina_Vector2F& operator/=(const Lina_Vector2F& rhs);
					  
	inline Lina_Vector2F& operator+=(const float& rhs);
	inline Lina_Vector2F& operator-=(const float& rhs);
	inline Lina_Vector2F& operator*=(const float& rhs);
	inline Lina_Vector2F& operator/=(const float& rhs);


	static Lina_Vector2F One();
	static Lina_Vector2F Zero();
};

#endif