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

template<typename T, unsigned int D>
class Lina_Vector
{

public:

	Lina_Vector() {};

	T Max() const;
	T MagnitudeSq() const;
	T Magnitude() const;
	T AngleBetween(const Lina_Vector<T, D>& rhs) const;
	T Dot(const Lina_Vector<T, D>& r) const;


	Lina_Vector<T, D> Reflect(const Lina_Vector<T, D>& normal) const;
	Lina_Vector<T, D> Max(const Lina_Vector<T, D>& rrhs) const;
	Lina_Vector<T, D> Normalized() const;
	Lina_Vector<T, D> Lerp(const Lina_Vector<T, D>& rhs, T lerpFactor) const;
	Lina_Vector<T, D> Project(const Lina_Vector<T, D>& rhs) const;
	void Normalize() const;

	Lina_Vector<T, D> operator+(const Lina_Vector<T, D>& rhs) const;
	Lina_Vector<T, D> operator-(const Lina_Vector<T, D>& rhs) const;
	Lina_Vector<T, D> operator*(const Lina_Vector<T, D>&) const;
	Lina_Vector<T, D> operator/(const Lina_Vector<T, D>&) const;

	Lina_Vector<T, D> operator+(const T& rhs) const;
	Lina_Vector<T, D> operator-(const T& rhs) const;
	Lina_Vector<T, D> operator*(const T& rhs) const;
	Lina_Vector<T, D> operator/(const T& rhs) const;

	Lina_Vector<T, D>& operator+=(const Lina_Vector<T, D>& rhs);
	Lina_Vector<T, D>& operator-=(const Lina_Vector<T, D>& rhs);
	Lina_Vector<T, D>& operator*=(const Lina_Vector<T, D>& rhs);
	Lina_Vector<T, D>& operator/=(const Lina_Vector<T, D>& rhs);

	Lina_Vector<T, D>& operator+=(const T& rhs);
	Lina_Vector<T, D>& operator-=(const T& rhs);
	Lina_Vector<T, D>& operator*=(const T& rhs);
	Lina_Vector<T, D>& operator/=(const T& rhs);

	T& operator [] (unsigned int i);
	T operator [] (unsigned int i) const;
	inline bool operator==(const Lina_Vector<T, D>& rhs) const;
	inline bool operator!=(const Lina_Vector<T, D>& rhs) const;

	inline static Lina_Vector<T, D> One();
	inline static Lina_Vector<T, D> Zero();

private:

	T values[D];
};


template<typename T>
class Lina_Vector2 : public Lina_Vector<T, 2>
{
public:

	Lina_Vector2() { };
	Lina_Vector2(const Lina_Vector<T, 2>& rhs);
	Lina_Vector2(T x, T y);

	T Cross(const Lina_Vector2<T>& rhs) const;

	inline T GetX() const { return (*this)[0]; }
	inline T GetY() const { return (*this)[1]; }

	inline void SetX(const T& x) { (*this)[0] = x; }
	inline void SetY(const T& y) { (*this)[1] = y; }

	inline void Set(const T& x, const T& y) { SetX(x); SetY(y); }

protected:
private:
};

template<typename T>
class Lina_Vector3 : public Lina_Vector<T, 3>
{

public:
	Lina_Vector3() {};

	Lina_Vector3(const Lina_Vector<T, 3>& rhs);
	Lina_Vector3(T x, T y, T z);
	
	Lina_Vector3<T> Cross(const Lina_Vector3<T>& rhs) const;
	Lina_Vector3<T> Rotate(T angle, const Lina_Vector3<T>& axis) const;
	Lina_Vector3<T> Rotate(const Lina_Quaternion& rotation) const;

	inline Lina_Vector2<T> GetXY() const { return Lina_Vector2<T>(GetX(), GetY()); }
	inline Lina_Vector2<T> GetYZ() const { return Lina_Vector2<T>(GetY(), GetZ()); }
	inline Lina_Vector2<T> GetZX() const { return Lina_Vector2<T>(GetZ(), GetX()); }
	inline Lina_Vector2<T> GetYX() const { return Lina_Vector2<T>(GetY(), GetX()); }
	inline Lina_Vector2<T> GetZY() const { return Lina_Vector2<T>(GetZ(), GetY()); }
	inline Lina_Vector2<T> GetXZ() const { return Lina_Vector2<T>(GetX(), GetZ()); }

	inline T GetX() const { return (*this)[0]; }
	inline T GetY() const { return (*this)[1]; }
	inline T GetZ() const { return (*this)[2]; }

	inline void SetX(const T& x) { (*this)[0] = x; }
	inline void SetY(const T& y) { (*this)[1] = y; }
	inline void SetZ(const T& z) { (*this)[2] = z; }

	inline void Set(const T& x, const T& y, const T& z) { SetX(x); SetY(y); SetZ(z); }

protected:
private:

};

template<typename T>
class Lina_Vector4 : public Lina_Vector<T, 4>
{
public:

	Lina_Vector4() { };
	Lina_Vector4(const Lina_Vector<T, 4>& rhs);
	Lina_Vector4(T x, T y, T z, T w);

	inline T GetX() const { return (*this)[0]; }
	inline T GetY() const { return (*this)[1]; }
	inline T GetZ() const { return (*this)[2]; }
	inline T GetW() const { return (*this)[3]; }

	inline void SetX(const T& x) { (*this)[0] = x; }
	inline void SetY(const T& y) { (*this)[1] = y; }
	inline void SetZ(const T& z) { (*this)[2] = z; }
	inline void SetW(const T& w) { (*this)[3] = w; }

	inline void Set(const T& x, const T& y, const T& z, const T& w) { SetX(x); SetY(y); SetZ(z); SetW(w); }

protected:
private:
};

typedef Lina_Vector2<int> Vector2I;
typedef Lina_Vector3<int> Vector3I;
typedef Lina_Vector4<int> Vector4I;

typedef Lina_Vector2<float> Vector2F;
typedef Lina_Vector3<float> Vector3F;
typedef Lina_Vector4<float> Vector4F;

typedef Lina_Vector2<double> Vector2do;
typedef Lina_Vector3<double> Vector3do;
typedef Lina_Vector4<double> Vector4do;


#endif

