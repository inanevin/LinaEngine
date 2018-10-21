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
10/17/2018 9:15:19 PM

*/

#include "pch.h"
#include "Math/Lina_Matrix4F.h"  
#include "Math/Lina_Math.h"

Lina_Matrix4F::Lina_Matrix4F()
{

};

// returns a copy of an identity matrix.
Lina_Matrix4F Lina_Matrix4F::identity()
{
	Lina_Matrix4F m;
	m.InitIdentityMatrix();
	return m;
}

Lina_Matrix4F Lina_Matrix4F::Multiply(const Lina_Matrix4F& r1, const Lina_Matrix4F& r2)
{
	Lina_Matrix4F result;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result.SetElement(i, j, r1.GetElement(i, 0) * r2.GetElement(0, j) +
				r1.GetElement(i, 1) * r2.GetElement(1, j) +
				r1.GetElement(i, 2) * r2.GetElement(2, j) +
				r1.GetElement(i, 3) * r2.GetElement(3, j)
			);
		}
	}

	return result;
}

Lina_Matrix4F Lina_Matrix4F::Multiply(const Lina_Matrix4F& r)
{
	Lina_Matrix4F result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result.SetElement(i, j, this->GetElement(i, 0) * r.GetElement(0, j) +
				this->GetElement(i, 1) * r.GetElement(1, j) +
				this->GetElement(i, 2) * r.GetElement(2, j) +
				this->GetElement(i, 3) * r.GetElement(3, j)
			);
		}
	}
	return result;
}

void Lina_Matrix4F::InitIdentityMatrix()
{
	m[0][0] = 1;	m[0][1] = 0;	m[0][2] = 0;	m[0][3] = 0;	// Final X Component
	m[1][0] = 0;	m[1][1] = 1;	m[1][2] = 0;	m[1][3] = 0;	// Final Y Component
	m[2][0] = 0;	m[2][1] = 0;	m[2][2] = 1;	m[2][3] = 0;	// Final Z Component
	m[3][0] = 0;	m[3][1] = 0;	m[3][2] = 0;	m[3][3] = 1;	// Final W Component
}

void Lina_Matrix4F::InitPerspectiveProjection(float fov, float aspectRatio, float zNear, float zFar)
{

	// Calculate distance from the edges to the center.
	float tanHFOV = (float)tan(fov / 2);

	// How much space we have in the depth. We will scale z comp in to the range, taking the clipping into account.
	float zRng = zNear - zFar;

	// Override w component for depth division later on, store z for now.
	// Scale all the points into a 1:1 box in X-Y plane, taking aspect ratio into account.
	m[0][0] = 1.0f / (tanHFOV * aspectRatio);	m[0][1] = 0;				m[0][2] = 0;					m[0][3] = 0;
	m[1][0] = 0;								m[1][1] = 1.0f / tanHFOV;	m[1][2] = 0;					m[1][3] = 0;
	m[2][0] = 0;								m[2][1] = 0;				m[2][2] = (-zNear - zFar) / zRng;	m[2][3] = 2 * zFar * zNear / zRng;
	m[3][0] = 0;								m[3][1] = 0;				m[3][2] = 1;					m[3][3] = 0;
}

void Lina_Matrix4F::InitPosition(float x, float y, float z)
{
	m[0][0] = 1;	m[0][1] = 0;	m[0][2] = 0;	m[0][3] = x;
	m[1][0] = 0;	m[1][1] = 1;	m[1][2] = 0;	m[1][3] = y;
	m[2][0] = 0;	m[2][1] = 0;	m[2][2] = 1;	m[2][3] = z;
	m[3][0] = 0;	m[3][1] = 0;	m[3][2] = 0;	m[3][3] = 1;
}

void Lina_Matrix4F::InitRotation(float x, float y, float z)
{
	// 2D rotations for each individual axis.
	Lina_Matrix4F rX, rY, rZ;

	// Get radians out of rotation degrees.
	const float xR = (float)Lina_Math::ToRadians(x);
	const float yR = (float)Lina_Math::ToRadians(y);
	const float zR = (float)Lina_Math::ToRadians(z);

	// Set matrices with rotation.

	// Apply rotation to 1st & 2nd row, meaning we rotate the Z angle in X-Y plane. Remember columns represent x-y-z-w, for the Z rotation, we don't need to apply
	// the rotation in Z itself, since it already will have it, as well as the w.
	rZ.m[0][0] = (float)cos(zR);			rZ.m[0][1] = -(float)sin(zR);		rZ.m[0][2] = 0;		rZ.m[0][3] = 0;
	rZ.m[1][0] = (float)sin(zR);			rZ.m[1][1] = (float)cos(zR);		rZ.m[1][2] = 0;		rZ.m[1][3] = 0;
	rZ.m[2][0] = 0;							rZ.m[2][1] = 0;						rZ.m[2][2] = 1;		rZ.m[2][3] = 0;
	rZ.m[3][0] = 0;							rZ.m[3][1] = 0;						rZ.m[3][2] = 0;		rZ.m[3][3] = 1;

	// Apply rotation to 2&3 row, meaning Y-Z plane, for the X angle. 
	rX.m[0][0] = 1;		rX.m[0][1] = 0;					rX.m[0][2] = 0;						rX.m[0][3] = 0;
	rX.m[1][0] = 0;		rX.m[1][1] = (float)cos(xR);	rX.m[1][2] = -(float)sin(xR);		rX.m[1][3] = 0;
	rX.m[2][0] = 0;		rX.m[2][1] = (float)sin(xR);	rX.m[2][2] = (float)cos(xR);		rX.m[2][3] = 0;
	rX.m[3][0] = 0;		rX.m[3][1] = 0;					rX.m[3][2] = 0;						rX.m[3][3] = 1;

	// Apply rotation X-Z plane, for the Y angle.
	rY.m[0][0] = (float)cos(yR);		rY.m[0][1] = 0;		rY.m[0][2] = -(float)sin(yR);		rY.m[0][3] = 0;
	rY.m[1][0] = 0;						rY.m[1][1] = 1;		rY.m[1][2] = 0;						rY.m[1][3] = 0;
	rY.m[2][0] = (float)sin(yR);		rY.m[2][1] = 0;		rY.m[2][2] = (float)cos(yR);	    rY.m[2][3] = 0;
	rY.m[3][0] = 0;						rY.m[3][1] = 0;		rY.m[3][2] = 0;						rY.m[3][3] = 1;

	Lina_Matrix4F result = rZ.Multiply(rY.Multiply(rX));
	// Set the inner-outer multiplication result as this matrix.
	SetMatrix(result.m);
}

void Lina_Matrix4F::InitScale(float x, float y, float z)
{
	m[0][0] = x;	m[0][1] = 0;	m[0][2] = 0;	m[0][3] = 0;
	m[1][0] = 0;	m[1][1] = y;	m[1][2] = 0;	m[1][3] = 0;
	m[2][0] = 0;	m[2][1] = 0;	m[2][2] = z;	m[2][3] = 0;
	m[3][0] = 0;	m[3][1] = 0;	m[3][2] = 0;	m[3][3] = 1;
}

void Lina_Matrix4F::InitRotation(Vector3 forward, Vector3 up) 
{
	Vector3 f = forward;
	f.Normalize();

	Vector3 r = up;
	r.Normalize();
	r = r.Cross(f);

	Vector3 u = f.Cross(r);

	m[0][0] = r.x;	m[0][1] = r.y;	m[0][2] = r.z;	m[0][3] = 0;	
	m[1][0] = u.x;	m[1][1] = u.y;	m[1][2] = u.z;	m[1][3] = 0;	
	m[2][0] = f.x;	m[2][1] = f.y;	m[2][2] = f.z;	m[2][3] = 0;	
	m[3][0] = 0;	m[3][1] = 0;	m[3][2] = 0;	m[3][3] = 1;	
}

void Lina_Matrix4F::SetMatrix(float(&arr)[4][4])
{
	std::memcpy(this->m, arr, sizeof(float) * 16);
}

float Lina_Matrix4F::GetElement(int x, int y) const
{
	return this->m[x][y];
}

void Lina_Matrix4F::SetElement(int x, int y, float val)
{
	this->m[x][y] = val;
}
