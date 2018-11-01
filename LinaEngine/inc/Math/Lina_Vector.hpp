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
/*
#pragma once

#ifndef Lina_Vector_HPP
#define Lina_Vector_HPP

class Lina_Quaternion;


class Lina_Vector4F
{
public:

	Lina_Vector4F() {};

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

/*
template<typename T, unsigned int D>
class Lina_Vector
{

public:

	Lina_Vector() {};

	T Max() const;
	T MagnitudeSq() const;
	T Magnitude() const;
	T AngleBetween(const Lina_Vector<T, D>& rhs) const;
	T Dot(const Lina_Vector<T, D>& rhs) const;

	Lina_Vector<T, D> Reflect(const Lina_Vector<T, D>& normal) const;
	Lina_Vector<T, D> Max(const Lina_Vector<T, D>& rhs) const;
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
	bool operator==(const Lina_Vector<T, D>& rhs) const;
	bool operator!=(const Lina_Vector<T, D>& rhs) const;

	static Lina_Vector<T, D> One();
	static Lina_Vector<T, D> Zero();

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
*/

/*
#endif

*/