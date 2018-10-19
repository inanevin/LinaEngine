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
10/17/2018 10:57:07 PM

*/

#include "pch.h"
#include "Lina_Transform.h"  
#include "Scene/Lina_Scene.h"

Vector3 Lina_Transform::GetPosition() { return position; }
Vector3 Lina_Transform::GetRotation() { return rotation; }
Vector3 Lina_Transform::GetScale() { return scale; }
void Lina_Transform::SetPosition(Vector3 t) { position = t; }
void Lina_Transform::SetPosition(float x, float y, float z) { position = Vector3(x, y, z); }
void Lina_Transform::SetRotation(Vector3 r) { rotation = r; }
void Lina_Transform::SetRotation(float x, float y, float z) { rotation = Vector3(x, y, z); }
void Lina_Transform::SetScale(Vector3 s) { scale = s; }
void Lina_Transform::SetScale(float x, float y, float z) { scale = Vector3(x, y, z); }

Lina_Transform::Lina_Transform()
{
	position = Vector3::zero();
	rotation = Vector3::zero();
	scale = Vector3::one();
}

// Get transformation matrix composed of position & rotation & scale.
Matrix4 Lina_Transform::GetTransformation()
{
	Matrix4 t;	// Translation
	Matrix4 r;	// Rotation
	Matrix4 s;	// Scale

	// Init matrix with the desired translation.
	t.InitPosition(position.x, position.y, position.z);
	r.InitRotation(rotation.x, rotation.y, rotation.z);
	s.InitScale(scale.x, scale.y, scale.z);

	// Return the multiplied scale with rotation with translation, inner-outer order.
	return t.Multiply(r.Multiply(s));
}

// Perspective transformation projections.
Matrix4 Lina_Transform::GetProjectedTransformation()
{
	Matrix4 transformationMatrix = GetTransformation();
	Matrix4 projectionMatrix;
	Matrix4 cameraRotation;
	Matrix4 cameraTranslation;

	projectionMatrix.InitProjection(fov, width, height, zNear, zFar);
	cameraRotation.InitCamera(sceneCamera->GetForward(), sceneCamera->GetUp());
	cameraTranslation.InitPosition(-sceneCamera->GetPosition().x, -sceneCamera->GetPosition().y, -sceneCamera->GetPosition().z);
	return projectionMatrix.Multiply(cameraRotation.Multiply(cameraTranslation.Multiply(transformationMatrix)));
}

// Sets projection
void Lina_Transform::SetProjection(float f, float w, float h, float zN, float zF)
{
	fov = f;
	width = w;
	height = h;
	zNear = zN;
	zFar = zF;
}

void Lina_Transform::SetCamera(Lina_Camera& cam)
{
	sceneCamera = &cam;
}


