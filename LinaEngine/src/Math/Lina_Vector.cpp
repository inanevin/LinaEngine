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
10/29/2018 11:50:07 PM

*/

#include "pch.h"
#include "Math/Lina_Vector.hpp"  
#include "Math/Lina_Quaternion.hpp"

float Lina_Vector4F::Max() const
{

}

float Lina_Vector4F::MagnitudeSq() const
{
	return 0.0f;
}

float Lina_Vector4F::Magnitude() const
{
	return 0.0f;
}

float Lina_Vector4F::AngleBetween(const Lina_Vector4F & rhs) const
{
	return 0.0f;
}

float Lina_Vector4F::Dot(const Lina_Vector4F & rhs) const
{
	return 0.0f;
}



/*
template<typename T, unsigned int D>
T Lina_Vector<T, D>::MagnitudeSq() const
{
	return this->Dot(*this);
}

template<typename T, unsigned int D>
T Lina_Vector<T, D>::Magnitude() const
{
	return sqrt(MagnitudeSq());
}

template<typename T, unsigned int D>
T Lina_Vector<T, D>::AngleBetween(const Lina_Vector<T,D>& rhs) const
{
	T angle = this->Dot( rhs);
	angle /= (this->Magnitude() * rhs.Magnitude());
	return acosf(angle);
}

template<typename T, unsigned int D>
T Lina_Vector<T, D>::Dot(const Lina_Vector<T, D>& r) const
{
	T result = T(0);
	for (unsigned int i = 0; i < D; i++)
		result += (*this)[i] * r[i];

	return result;
}

template<typename T, unsigned int D>
Lina_Vector<T, D> Lina_Vector<T, D>::Reflect(const Lina_Vector<T, D>& normal) const
{
	return *this - (normal * (this->Dot(normal) * 2));
}

template<typename T, unsigned int D>
Lina_Vector<T, D> Lina_Vector<T, D>::Max(const Lina_Vector<T, D>& r) const
{
	Lina_Vector<T, D> result;
	for (unsigned int i = 0; i < D; i++)
	{
		result[i] = values[i] > r[i] ? values[i] : r[i];
	}

	return result;
}

template<typename T, unsigned int D>
T Lina_Vector<T, D>::Max() const
{
	T maxVal = (*this)[0];

	for (int i = 0; i < D; i++)
		if ((*this)[i] > maxVal)
			maxVal = (*this)[i];

	return maxVal;
}

template<typename T, unsigned int D>
Lina_Vector<T, D> Lina_Vector<T, D>::Normalized() const
{
	Lina_Vector<T, D> v = Lina_Vector<T, D>(*this);
	return v / Magnitude();
}

template<typename T, unsigned int D>
void Lina_Vector<T, D>::Normalize() const
{
	*this /= Magnitude();
}


template<typename T, unsigned int D>
Lina_Vector<T, D> Lina_Vector<T, D>::Lerp(const Lina_Vector<T, D>& r, T lerpFactor) const
{
	return (r - *this) * lerpFactor + *this;
}

template<typename T, unsigned int D>
Lina_Vector<T, D> Lina_Vector<T, D>::Project(const Lina_Vector<T, D>& rhs) const
{
	Lina_Vector<T, D> bn = rhs / rhs.Magnitude();
	return bn * this->Dot(rhs);
}

template<typename T, unsigned int D>
Lina_Vector<T, D> Lina_Vector<T, D>::operator+(const Lina_Vector<T, D>& r) const
{
	Lina_Vector<T, D> result;
	for (unsigned int i = 0; i < D; i++)
		result[i] = values[i] + r[i];

	return result;
}

template<typename T, unsigned int D>
Lina_Vector<T, D> Lina_Vector<T, D>::operator-(const Lina_Vector<T, D>& r) const
{
	Lina_Vector<T, D> result;
	for (unsigned int i = 0; i < D; i++)
		result[i] = values[i] - r[i];

	return result;
}

template<typename T, unsigned int D>
Lina_Vector<T, D> Lina_Vector<T, D>::operator*(const Lina_Vector<T, D>& r) const
{
	Lina_Vector<T, D> result;
	for (unsigned int i = 0; i < D; i++)
		result[i] = values[i] * r[i];

	return result;
}

template<typename T, unsigned int D>
Lina_Vector<T, D> Lina_Vector<T, D>::operator/(const Lina_Vector<T, D>& r) const
{
	Lina_Vector<T, D> result;
	for (unsigned int i = 0; i < D; i++)
	{
		if (r[i] == 0.0)
			result[i] = 0.0f;
		else
			result[i] = values[i] / r[i];
	}

	return result;
}

template<typename T, unsigned int D>
Lina_Vector<T, D> Lina_Vector<T, D>::operator+(const T & r) const
{
	Lina_Vector<T, D> result;
	for (unsigned int i = 0; i < D; i++)
		result[i] = values[i] + r;

	return result;
}

template<typename T, unsigned int D>
Lina_Vector<T, D> Lina_Vector<T, D>::operator-(const T & r) const
{
	Lina_Vector<T, D> result;
	for (unsigned int i = 0; i < D; i++)
		result[i] = values[i] - r;

	return result;
}

template<typename T, unsigned int D>
Lina_Vector<T, D> Lina_Vector<T, D>::operator*(const T & r) const
{
	Lina_Vector<T, D> result;
	for (unsigned int i = 0; i < D; i++)
		result[i] = values[i] * r;

	return result;
}

template<typename T, unsigned int D>
Lina_Vector<T, D> Lina_Vector<T, D>::operator/(const T & r) const
{
	if (r == 0.0f)
		return Lina_Vector<T, D>::Zero();

	Lina_Vector<T, D> result;

	for (unsigned int i = 0; i < D; i++)
		result[i] = values[i] / r;

	return result;
}

template<typename T, unsigned int D>
Lina_Vector<T, D>& Lina_Vector<T, D>::operator+=(const Lina_Vector<T, D>& r)
{
	for (unsigned int i = 0; i < D; i++)
		(*this)[i] = values[i] + r[i];

	return *this;
}

template<typename T, unsigned int D>
Lina_Vector<T, D>& Lina_Vector<T, D>::operator-=(const Lina_Vector<T, D>& r)
{
	for (unsigned int i = 0; i < D; i++)
		(*this)[i] = values[i] - r[i];

	return *this;
}

template<typename T, unsigned int D>
Lina_Vector<T, D>& Lina_Vector<T, D>::operator*=(const Lina_Vector<T, D>& r)
{
	for (unsigned int i = 0; i < D; i++)
		(*this)[i] = values[i] * r[i];

	return *this;
}

template<typename T, unsigned int D>
Lina_Vector<T, D>& Lina_Vector<T, D>::operator/=(const Lina_Vector<T, D>& r)
{
	for (unsigned int i = 0; i < D; i++)
	{
		if (r[i] == 0.0f)
			(*this)[i] = 0.0f;
		else
			(*this)[i] = values[i] / r[i];
	}


	return *this;
}

template<typename T, unsigned int D>
Lina_Vector<T, D>& Lina_Vector<T, D>::operator+=(const T & r)
{
	for (unsigned int i = 0; i < D; i++)
		(*this)[i] = values[i] + r;

	return *this;
}

template<typename T, unsigned int D>
Lina_Vector<T, D>& Lina_Vector<T, D>::operator-=(const T & r)
{
	for (unsigned int i = 0; i < D; i++)
		(*this)[i] = values[i] - r;

	return *this;
}

template<typename T, unsigned int D>
Lina_Vector<T, D>& Lina_Vector<T, D>::operator*=(const T & r)
{
	for (unsigned int i = 0; i < D; i++)
		(*this)[i] = values[i] * r;

	return *this;
}

template<typename T, unsigned int D>
Lina_Vector<T, D>& Lina_Vector<T, D>::operator/=(const T & r)
{
	if (r == 0.0f)
		return Lina_Vector<T, D>::Zero();

	for (unsigned int i = 0; i < D; i++)
		(*this)[i] = values[i] / r;

	return *this;
}

template<typename T, unsigned int D>
T & Lina_Vector<T, D>::operator[](unsigned int i)
{
	return values[i];
}

template<typename T, unsigned int D>
T Lina_Vector<T, D>::operator[](unsigned int i) const
{
	return values[i];
}

template<typename T, unsigned int D>
bool Lina_Vector<T, D>::operator==(const Lina_Vector<T, D>& r) const
{
	for (unsigned int i = 0; i < D; i++)
		if ((*this)[i] != r[i])
			return false;
	return true;
}

template<typename T, unsigned int D>
bool Lina_Vector<T, D>::operator!=(const Lina_Vector<T, D>& r) const
{
	return !operator==(r);
}

template<typename T, unsigned int D>
inline Lina_Vector<T, D> Lina_Vector<T, D>::One()
{
	Lina_Vector<T, D> v;

	for (unsigned int i = 0; i < D; i++)
		v[i] = (T)1.0;

	return v;
}

template<typename T, unsigned int D>
inline Lina_Vector<T, D> Lina_Vector<T, D>::Zero()
{
	Lina_Vector<T, D> v;

	for (unsigned int i = 0; i < D; i++)
		v[i] = (T)0.0;

	return v;
}

template<typename T>
Lina_Vector2<T>::Lina_Vector2(const Lina_Vector<T, 2>& r)
{
	(*this)[0] = r[0];
	(*this)[1] = r[1];
}

template<typename T>
Lina_Vector2<T>::Lina_Vector2(T x, T y)
{
	(*this)[0] = x;
	(*this)[1] = y;
}

template<typename T>
T Lina_Vector2<T>::Cross(const Lina_Vector2<T>& r) const
{
	return GetX() * r.GetY() - GetY() * r.GetX();
}

template<typename T>
Lina_Vector3<T>::Lina_Vector3(const Lina_Vector<T, 3>& r)
{
	(*this)[0] = r[0];
	(*this)[1] = r[1];
	(*this)[2] = r[2];
}

template<typename T>
Lina_Vector3<T>::Lina_Vector3(T x, T y, T z)
{
	(*this)[0] = x;
	(*this)[1] = y;
	(*this)[2] = z;
}

template<typename T>
Lina_Vector3<T> Lina_Vector3<T>::Cross(const Lina_Vector3<T>& r) const
{
	T x = (*this)[1] * r[2] - (*this)[2] * r[1];
	T y = (*this)[2] * r[0] - (*this)[0] * r[2];
	T z = (*this)[0] * r[1] - (*this)[1] * r[0];

	return Vector3<T>(x, y, z);
}

template<typename T>
Lina_Vector3<T> Lina_Vector3<T>::Rotate(T angle, const Lina_Vector3<T>& axis) const
{
	const T sinAngle = sin(-angle);
	const T cosAngle = cos(-angle);

	return this->Cross(axis * sinAngle) +        //Rotation on local X
		(*this * cosAngle) +                     //Rotation on local Z
		axis * this->Dot(axis * (1 - cosAngle)); //Rotation on local Y
}

template<typename T>
Lina_Vector3<T> Lina_Vector3<T>::Rotate(const Lina_Quaternion& rotation) const
{
	Quaternion conjugateQ = rotation.Conjugate();
	Quaternion w = rotation * (*this) * conjugateQ;

	Lina_Vector3<T> ret(w.GetX(), w.GetY(), w.GetZ());

	return ret;
}

template<typename T>
Lina_Vector4<T>::Lina_Vector4(const Lina_Vector<T, 4>& r)
{
	(*this)[0] = r[0];
	(*this)[1] = r[1];
	(*this)[2] = r[2];
	(*this)[3] = r[3];
}

template<typename T>
Lina_Vector4<T>::Lina_Vector4(T x, T y, T z, T w)
{
	(*this)[0] = x;
	(*this)[1] = y;
	(*this)[2] = z;
	(*this)[3] = w;
}

*/

Lina_Vector4F Lina_Vector4F::Reflect(const Lina_Vector4F & normal) const
{
	return Lina_Vector4F();
}

Lina_Vector4F Lina_Vector4F::Max(const Lina_Vector4F & rhs) const
{
	return Lina_Vector4F();
}

Lina_Vector4F Lina_Vector4F::Normalized() const
{
	return Lina_Vector4F();
}

Lina_Vector4F Lina_Vector4F::Lerp(const Lina_Vector4F & rhs, float lerpFactor) const
{
	return Lina_Vector4F();
}

Lina_Vector4F Lina_Vector4F::Project(const Lina_Vector4F & rhs) const
{
	return Lina_Vector4F();
}

void Lina_Vector4F::Normalize() const
{
}
