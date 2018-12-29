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
10/16/2018 12:38:18 AM

*/

#pragma once

#ifndef Lina_Transform_HPP
#define Lina_Transform_HPP

#include "Math/Lina_Matrix.hpp"
#include "Math/Lina_Vector.hpp"
#include "Scene/Lina_Camera.hpp"
#include "Math/Lina_Quaternion.hpp"


class Lina_Transform
{

public:

	Lina_Transform(const Lina_Vector3F& pos = Lina_Vector3F(0, 0, 0), const Lina_Quaternion& rot = Lina_Quaternion(0, 0, 0, 1),
		const Lina_Vector3F& s = Lina_Vector3F(1, 1, 1)) : m_Position(pos), m_Rotation(rot), m_Scale(s) {};

	inline Vector3 GetPosition() { return m_Position; };
	inline Quaternion GetRotation() { return m_Rotation; };
	inline Vector3 GetScale() { return m_Scale; }
	inline void SetPosition(Vector3 v) { m_Position = v; };
	inline void SetPosition(float x, float y, float z) { m_Position = Vector3(x, y, z); };
	inline void SetRotation(Quaternion q) { m_Rotation = q; };
	inline void SetRotation(float x, float y, float z) { m_Rotation = Quaternion(x, y, z, m_Rotation.w); };
	inline void SetScale(Vector3 v) { m_Scale = v; };
	inline void SetScale(float x, float y, float z) { m_Scale = Vector3(x, y, z); };
	inline void SetParent(Lina_Transform* p) { m_Parent = p; }

	Matrix4 GetTransformation();



private:

	Lina_Transform* m_Parent;
	Vector3 m_Position;
	Quaternion m_Rotation;
	Vector3 m_Scale;
};


#endif