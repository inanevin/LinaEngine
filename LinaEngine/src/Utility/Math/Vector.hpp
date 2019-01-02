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


class Vector4F
{
public:

	float x, y, z, w;

	Vector4F() {};
	Vector4F(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) { };
	Vector4F(const Vector4F& rhs) :x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {};


	float Max() const;
	float MagnitudeSq() const;
	float Magnitude() const;
	float AngleBetween(const Vector4F& rhs) const;
	float Dot(const Vector4F& rhs) const;

	Vector4F Reflect(const Vector4F& normal) const;
	Vector4F Max(const Vector4F& rhs) const;
	Vector4F Normalized() const;
	Vector4F Lerp(const Vector4F& rhs, float lerpFactor) const;
	Vector4F Project(const Vector4F& rhs) const;
	void Normalize();


	static Vector4F One() { return Vector4F(1, 1, 1, 1); }
	static Vector4F Zero() { return Vector4F(0, 0, 0, 0); }

#pragma region Operator Overloads

	inline Vector4F operator+(const Vector4F& rhs) const
	{
		return Vector4F(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z, this->w + rhs.w);
	};

	inline Vector4F operator-(const Vector4F& rhs) const
	{
		return Vector4F(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z, this->w - rhs.w);
	};

	inline Vector4F operator*(const Vector4F& rhs) const
	{

		return Vector4F(this->x * rhs.x, this->y * rhs.y, this->z * rhs.z, this->w * rhs.w);

	}
	inline Vector4F operator/(const Vector4F& rhs) const
	{
		float xR = rhs.x == 0.0f ? 0.0f : this->x / rhs.x;
		float yR = rhs.y == 0.0f ? 0.0f : this->y / rhs.y;
		float zR = rhs.z == 0.0f ? 0.0f : this->z / rhs.z;
		float wR = rhs.w == 0.0f ? 0.0f : this->w / rhs.w;

		return Vector4F(xR, yR, zR, wR);
	};


	inline Vector4F operator+(const float& rhs) const
	{
		return Vector4F(this->x + rhs, this->y + rhs, this->z + rhs, this->w + rhs);
	}

	inline Vector4F operator-(const float& rhs) const
	{
		return Vector4F(this->x - rhs, this->y - rhs, this->z - rhs, this->w - rhs);
	}

	inline Vector4F operator*(const float& rhs) const
	{
		return Vector4F(this->x * rhs, this->y * rhs, this->z * rhs, this->w * rhs);
	}

	inline Vector4F operator/(const float& rhs) const
	{
		if (rhs == 0.0f)
			return Vector4F(0, 0, 0, 0);

		return Vector4F(this->x + rhs, this->y + rhs, this->z + rhs, this->w + rhs);
	}

	inline Vector4F& operator+=(const Vector4F& rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;
		this->w += rhs.w;

		return *this;
	};

	inline Vector4F& operator-=(const Vector4F& rhs)
	{
		this->x -= rhs.x;
		this->y -= rhs.y;
		this->z -= rhs.z;
		this->w -= rhs.w;

		return *this;
	};

	inline Vector4F& operator*=(const Vector4F& rhs)
	{
		this->x *= rhs.x;
		this->y *= rhs.y;
		this->z *= rhs.z;
		this->w *= rhs.w;

		return *this;
	};

	inline Vector4F& operator/=(const Vector4F& rhs)
	{
		this->x = rhs.x == 0.0f ? 0.0f : this->x / rhs.x;
		this->y = rhs.y == 0.0f ? 0.0f : this->y / rhs.y;
		this->z = rhs.z == 0.0f ? 0.0f : this->z / rhs.z;
		this->w = rhs.w == 0.0f ? 0.0f : this->w / rhs.w;

		return *this;
	};


	inline Vector4F& operator+=(const float& rhs)
	{
		this->x += rhs;
		this->y += rhs;
		this->z += rhs;
		this->w += rhs;

		return *this;
	};


	inline Vector4F& operator-=(const float& rhs)
	{
		this->x -= rhs;
		this->y -= rhs;
		this->z -= rhs;
		this->w -= rhs;

		return *this;
	};

	inline Vector4F& operator*=(const float& rhs)
	{
		this->x *= rhs;
		this->y *= rhs;
		this->z *= rhs;
		this->w *= rhs;

		return *this;
	};

	inline Vector4F& operator/=(const float& rhs)
	{
		if (rhs == 0.0f)
		{
			this->x = this->y = this->z = this->w = 0.0f;
			return *this;
		}

		this->x /= rhs;
		this->y /= rhs;
		this->z /= rhs;
		this->w /= rhs;

		return *this;
	};


	inline bool operator==(const Vector4F& rhs) const
	{
		return (this->x == rhs.x && this->y == rhs.y && this->z == rhs.z && this->w == rhs.w);
	}

	inline bool operator!=(const Vector4F& rhs) const
	{
		return !(this->x == rhs.x && this->y == rhs.y && this->z == rhs.z && this->w == rhs.w);
	}


	inline bool operator>(const Vector4F& rhs) const
	{
		return this->Magnitude() > rhs.Magnitude();
	}

	inline bool operator<(const Vector4F& rhs) const
	{
		return this->Magnitude() < rhs.Magnitude();
	}

	inline float& operator[] (unsigned int i) { 
		if (i == 0)
			return x;
		else if (i == 1)
			return y;
		else if (i == 2)
			return z;
		else
			return w;
	}

	inline float operator[] (unsigned int i) const
	{
		if (i == 0)
			return x;
		else if (i == 1)
			return y;
		else if (i == 2)
			return z;
		else
			return w;
	}

#pragma endregion

	
};


class Vector3F
{
public:

	float x, y, z;

	Vector3F() {};
	Vector3F(float X, float Y, float Z) : x(X), y(Y), z(Z) {};
	Vector3F(const Vector3F& rhs) :x(rhs.x), y(rhs.y), z(rhs.z) {};

	float Max() const;
	float MagnitudeSq() const;
	float Magnitude() const;
	float AngleBetween(const Vector3F& rhs) const;
	float Dot(const Vector3F& rhs) const;
	Vector3F Cross(const Vector3F& rhs) const;
	Vector3F Rotate(float angle, const Vector3F& axis);
	Vector3F Rotate(const Lina_Quaternion& rotation) const;

	Vector3F Reflect(const Vector3F& normal) const;
	Vector3F Max(const Vector3F& rhs) const;
	Vector3F Normalized() const;
	Vector3F Lerp(const Vector3F& rhs, float lerpFactor) const;
	Vector3F Project(const Vector3F& rhs) const;
	void Normalize();

	inline static Vector3F One() { return Vector3F(1, 1, 1); }
	inline static Vector3F Zero() { return Vector3F(0, 0, 0); }
	inline static Vector3F Up() { return Vector3F(0, 1, 0); }

#pragma region OperatorOverloads

	inline Vector3F operator+(const Vector3F& rhs) const
	{
		return Vector3F(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z);
	};

	inline Vector3F operator-(const Vector3F& rhs) const
	{
		return Vector3F(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z);
	};

	inline Vector3F operator*(const Vector3F& rhs) const
	{

		return Vector3F(this->x * rhs.x, this->y * rhs.y, this->z * rhs.z);

	}
	inline Vector3F operator/(const Vector3F& rhs) const
	{
		float xR = rhs.x == 0.0f ? 0.0f : this->x / rhs.x;
		float yR = rhs.y == 0.0f ? 0.0f : this->y / rhs.y;
		float zR = rhs.z == 0.0f ? 0.0f : this->z / rhs.z;

		return Vector3F(xR, yR, zR);
	};

	inline Vector3F operator+(const float& rhs) const
	{
		return Vector3F(this->x + rhs, this->y + rhs, this->z + rhs);
	}

	inline Vector3F operator-(const float& rhs) const
	{
		return Vector3F(this->x - rhs, this->y - rhs, this->z - rhs);
	}

	inline Vector3F operator*(const float& rhs) const
	{
		return Vector3F(this->x * rhs, this->y * rhs, this->z * rhs);
	}

	inline Vector3F operator/(const float& rhs) const
	{
		if (rhs == 0.0f)
			return Vector3F(0, 0, 0);

		return Vector3F(this->x + rhs, this->y + rhs, this->z + rhs);
	}

	inline Vector3F& operator+=(const Vector3F& rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;

		return *this;
	};

	inline Vector3F& operator-=(const Vector3F& rhs)
	{
		this->x -= rhs.x;
		this->y -= rhs.y;
		this->z -= rhs.z;

		return *this;
	};

	inline Vector3F& operator*=(const Vector3F& rhs)
	{
		this->x *= rhs.x;
		this->y *= rhs.y;
		this->z *= rhs.z;

		return *this;
	};

	inline Vector3F& operator/=(const Vector3F& rhs)
	{
		this->x = rhs.x == 0.0f ? 0.0f : this->x / rhs.x;
		this->y = rhs.y == 0.0f ? 0.0f : this->y / rhs.y;
		this->z = rhs.z == 0.0f ? 0.0f : this->z / rhs.z;

		return *this;
	};


	inline Vector3F& operator+=(const float& rhs)
	{
		this->x += rhs;
		this->y += rhs;
		this->z += rhs;

		return *this;
	};


	inline Vector3F& operator-=(const float& rhs)
	{
		this->x -= rhs;
		this->y -= rhs;
		this->z -= rhs;

		return *this;
	};

	inline Vector3F& operator*=(const float& rhs)
	{
		this->x *= rhs;
		this->y *= rhs;
		this->z *= rhs;

		return *this;
	};

	inline Vector3F& operator/=(const float& rhs)
	{
		if (rhs == 0.0f)
		{
			this->x = this->y = this->z = 0.0f;
			return *this;
		}

		this->x /= rhs;
		this->y /= rhs;
		this->z /= rhs;

		return *this;
	};

	inline bool operator==(const Vector3F& rhs) const
	{
		return (this->x == rhs.x && this->y == rhs.y && this->z == rhs.z);
	}

	inline bool operator!=(const Vector3F& rhs) const
	{
		return !(this->x == rhs.x && this->y == rhs.y && this->z == rhs.z);
	}

	inline bool operator>(const Vector3F& rhs) const
	{
		return this->Magnitude() > rhs.Magnitude();
	}

	inline bool operator<(const Vector3F& rhs) const
	{
		return this->Magnitude() < rhs.Magnitude();
	}

	inline float& operator[] (unsigned int i) {
		if (i == 0)
			return x;
		else if (i == 1)
			return y;
		else
			return z;
	}

	inline float operator[] (unsigned int i) const
	{
		if (i == 0)
			return x;
		else if (i == 1)
			return y;
		else
			return z;
	}

#pragma endregion

};



class Vector2F
{
public:

	float x, y;

	Vector2F() {};
	Vector2F(float X, float Y) : x(X), y(Y) {};
	Vector2F(const Vector3F& rhs) :x(rhs.x), y(rhs.y) {};


	float Max() const;
	float MagnitudeSq() const;
	float Magnitude() const;
	float AngleBetween(const Vector2F& rhs) const;
	float Dot(const Vector2F& rhs) const;
	float Cross(const Vector2F& rhs) const;

	Vector2F Reflect(const Vector2F& normal) const;
	Vector2F Max(const Vector2F& rhs) const;
	Vector2F Normalized() const;
	Vector2F Lerp(const Vector2F& rhs, float lerpFactor) const;
	Vector2F Project(const Vector2F& rhs) const;
	void Normalize();

	inline static Vector2F One() { return Vector2F(0, 0); };
	inline static Vector2F Zero() { return Vector2F(1, 1); };

#pragma region OperatorOverloads

	inline Vector2F operator+(const Vector2F& rhs) const
	{
		return Vector2F(this->x + rhs.x, this->y + rhs.y);
	};

	inline Vector2F operator-(const Vector2F& rhs) const
	{
		return Vector2F(this->x - rhs.x, this->y - rhs.y);
	};

	inline Vector2F operator*(const Vector2F& rhs) const
	{

		return Vector2F(this->x * rhs.x, this->y * rhs.y);

	}
	inline Vector2F operator/(const Vector2F& rhs) const
	{
		float xR = rhs.x == 0.0f ? 0.0f : this->x / rhs.x;
		float yR = rhs.y == 0.0f ? 0.0f : this->y / rhs.y;

		return Vector2F(xR, yR);
	};

	inline Vector2F operator+(const float& rhs) const
	{
		return Vector2F(this->x + rhs, this->y + rhs);
	}

	inline Vector2F operator-(const float& rhs) const
	{
		return Vector2F(this->x - rhs, this->y - rhs);
	}

	inline Vector2F operator*(const float& rhs) const
	{
		return Vector2F(this->x * rhs, this->y * rhs);
	}

	inline Vector2F operator/(const float& rhs) const
	{
		if (rhs == 0.0f)
			return Vector2F(0, 0);

		return Vector2F(this->x + rhs, this->y + rhs);
	}

	inline Vector2F& operator+=(const Vector2F& rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;

		return *this;
	};

	inline Vector2F& operator-=(const Vector2F& rhs)
	{
		this->x -= rhs.x;
		this->y -= rhs.y;

		return *this;
	};

	inline Vector2F& operator*=(const Vector2F& rhs)
	{
		this->x *= rhs.x;
		this->y *= rhs.y;

		return *this;
	};

	inline Vector2F& operator/=(const Vector2F& rhs)
	{
		this->x = rhs.x == 0.0f ? 0.0f : this->x / rhs.x;
		this->y = rhs.y == 0.0f ? 0.0f : this->y / rhs.y;

		return *this;
	};


	inline Vector2F& operator+=(const float& rhs)
	{
		this->x += rhs;
		this->y += rhs;

		return *this;
	};


	inline Vector2F& operator-=(const float& rhs)
	{
		this->x -= rhs;
		this->y -= rhs;

		return *this;
	};

	inline Vector2F& operator*=(const float& rhs)
	{
		this->x *= rhs;
		this->y *= rhs;

		return *this;
	};

	inline Vector2F& operator/=(const float& rhs)
	{
		if (rhs == 0.0f)
		{
			this->x = this->y = 0.0f;
			return *this;
		}

		this->x /= rhs;
		this->y /= rhs;

		return *this;
	};

	inline bool operator==(const Vector2F& rhs) const
	{
		return (this->x == rhs.x && this->y == rhs.y);
	}

	inline bool operator!=(const Vector2F& rhs) const
	{
		return !(this->x == rhs.x && this->y == rhs.y);
	}

	inline bool operator>(const Vector2F& rhs) const
	{
		return this->Magnitude() > rhs.Magnitude();
	}

	inline bool operator<(const Vector2F& rhs) const
	{
		return this->Magnitude() < rhs.Magnitude();
	}

	inline float& operator[] (unsigned int i) {
		if (i == 0)
			return x;
		else
			return y;
	}

	inline float operator[] (unsigned int i) const
	{
		if (i == 0)
			return x;
		else
			return y;
	}

#pragma endregion

};


#endif