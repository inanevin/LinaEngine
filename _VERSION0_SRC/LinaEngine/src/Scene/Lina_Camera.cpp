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
10/17/2018 9:29:43 PM

*/

#include "pch.h"
#include "Scene/Lina_Camera.hpp"  
#include "Utility/Lina_Time.hpp"
#include "Core/Lina_CoreMessageBus.hpp"
#include "Input/Lina_InputEngine.hpp"
#include "Math/Lina_Math.hpp"
#include "Game/Lina_Actor.hpp"

Lina_Camera::Lina_Camera()
{
	this->position = Vector3::Zero();
	this->up = Vector3(0, 1, 0);
	this->forward = Vector3(0, 0, 1);
	this->projection.InitPerspective((float)Lina_Math::ToRadians(70), 1.33f, 0.1f, 1000.0f);
}

Lina_Camera::Lina_Camera(float fov, float aspect, float zNear, float zFar)
{
	this->position = Vector3::Zero();
	this->up = Vector3(0, 1, 0);
	this->forward = Vector3(0, 0, 1);
	this->projection.InitPerspective(fov, aspect, zNear, zFar);
};

void Lina_Camera::Move(Vector3 dir, float amount)
{
	//GetTransform().SetPosition(GetTransform().GetPosition() + (dir * amount));
	position += (dir * amount);
}


void Lina_Camera::RotateX(float angle)
{
	Vector3 horizontal = Vector3::Up().Cross(forward).Normalized();

	// Rotate the forward axis with respect to the World's horizontal axis.
	forward.Rotate(angle, horizontal);
	forward.Normalize();

	// Update the up vector.
	up = forward.Cross(horizontal).Normalized();
}

void Lina_Camera::RotateY(float angle)
{
	Vector3 horizontal = Vector3::Up().Cross(forward).Normalized();

	// Rotate the forward axis with respect to the World's horizontal axis.
	forward.Rotate(angle, Vector3::Up());
	forward.Normalize();

	// Update the up vector.
	up = forward.Cross(horizontal).Normalized();

}

/*
void Lina_Camera::RotateX(float angle)
{
	Vector3 horizontal = Vector3::Up().Cross(GetTransform().GetRotation().GetForward()).Normalized();

	// Rotate the forward axis with respect to the World's horizontal axis.
	GetTransform().GetRotation().GetForward().Rotate(Lina_Math::ToRadians(angle), horizontal);

	GetTransform().GetRotation().GetForward().Normalize();

	// Update the up vector.
	GetTransform().GetRotation().GetUp() = GetTransform().GetRotation().GetForward().Cross(horizontal).Normalized();
}

void Lina_Camera::RotateY(float angle)
{
	Vector3 horizontal = Vector3::Up().Cross(GetTransform().GetRotation().GetForward()).Normalized();

	// Rotate the forward axis with respect to the World's horizontal axis.
	GetTransform().GetRotation().GetForward().Rotate(Lina_Math::ToRadians(angle), Vector3::Up());
	GetTransform().GetRotation().GetForward().Normalize();

	// Update the up vector.
	GetTransform().GetRotation().GetUp() = GetTransform().GetRotation().GetForward().Cross(horizontal).Normalized();

}*/

Matrix4 Lina_Camera::GetViewProjection()
{
	Matrix4 cameraRotation;
	Matrix4 cameraTranslation;

	cameraRotation.InitRotationFromDirection(forward, up);
	cameraTranslation.InitTranslation(Vector3(-position.x, -position.y, -position.z));

	return projection * (cameraRotation * cameraTranslation);

	/*Matrix4 cameraRotation = GetTransform().GetRotation().ToRotationMatrix();
	Matrix4 cameraTranslation = Matrix4().InitTranslation(Vector3(-GetTransform().GetPosition().x, -GetTransform().GetPosition().y, -GetTransform().GetPosition().z));

	return projection * (cameraRotation * cameraTranslation);*/
}

void Lina_Camera::AttachToActor(Lina_Actor& act)
{
	Lina_ActorComponent::AttachToActor(act);

	//act.Engine()->RenderingEngine()->AddCamera(*this);
}

void Lina_Camera::TempInput()
{
	float move = (float)(10 * Lina_Time::GetDelta());
	float rot = (float)(1 * Lina_Time::GetDelta());


	if (Lina_CoreMessageBus::Instance().GetInputEngine()->GetKey(SDL_SCANCODE_W))
	{
		Move(GetForward(), move);
	}
	if (Lina_CoreMessageBus::Instance().GetInputEngine()->GetKey(SDL_SCANCODE_S))
	{
		Move(GetForward(), -move);
	}
	if (Lina_CoreMessageBus::Instance().GetInputEngine()->GetKey(SDL_SCANCODE_A))
	{
		Move(GetLeft(), move);

	}
	if (Lina_CoreMessageBus::Instance().GetInputEngine()->GetKey(SDL_SCANCODE_D))
	{
		Move(GetRight(), move);

	}

	if (Lina_CoreMessageBus::Instance().GetInputEngine()->GetKey(SDL_SCANCODE_UP))
	{
		RotateX(-rot);
	}

	if (Lina_CoreMessageBus::Instance().GetInputEngine()->GetKey(SDL_SCANCODE_DOWN))
	{
		RotateX(rot);
	}

	if (Lina_CoreMessageBus::Instance().GetInputEngine()->GetKey(SDL_SCANCODE_LEFT))
	{
		RotateY(-rot);
	}

	if (Lina_CoreMessageBus::Instance().GetInputEngine()->GetKey(SDL_SCANCODE_RIGHT))
	{
		RotateY(rot);
	}
}

void Lina_Camera::ProcessInput(float tickRate)
{
	float move = (float)(10 * Lina_Time::GetDelta());
	float rot = (float)(50 * Lina_Time::GetDelta());
	
	/*if (Lina_CoreMessageBus::Instance().GetInputEngine()->GetKey(SDL_SCANCODE_W))
	{
		Vector3 dir = GetTransform().GetRotation().GetForward();
		std::cout << dir.x << " " << dir.y << " " << dir.z << " " << std::endl;
		Move(GetTransform().GetRotation().GetForward(), move);
	}
	if (Lina_CoreMessageBus::Instance().GetInputEngine()->GetKey(SDL_SCANCODE_S))
	{
		Move(GetTransform().GetRotation().GetForward(), -move);
	}
	if (Lina_CoreMessageBus::Instance().GetInputEngine()->GetKey(SDL_SCANCODE_A))
	{
		Move(GetTransform().GetRotation().GetLeft(), move);

	}
	if (Lina_CoreMessageBus::Instance().GetInputEngine()->GetKey(SDL_SCANCODE_D))
	{
		Move(GetTransform().GetRotation().GetRight(), move);
		
	}*/

	if (Lina_CoreMessageBus::Instance().GetInputEngine()->GetKey(SDL_SCANCODE_W))
	{
		Move(GetForward(), move);
	}
	if (Lina_CoreMessageBus::Instance().GetInputEngine()->GetKey(SDL_SCANCODE_S))
	{
		Move(GetForward(), -move);
	}
	if (Lina_CoreMessageBus::Instance().GetInputEngine()->GetKey(SDL_SCANCODE_A))
	{
		Move(GetLeft(), move);

	}
	if (Lina_CoreMessageBus::Instance().GetInputEngine()->GetKey(SDL_SCANCODE_D))
	{
		Move(GetRight(), move);

	}

	if (Lina_CoreMessageBus::Instance().GetInputEngine()->GetKey(SDL_SCANCODE_UP))
	{
		RotateX(-rot);
	}

	if (Lina_CoreMessageBus::Instance().GetInputEngine()->GetKey(SDL_SCANCODE_DOWN))
	{
		RotateX(rot);
	}

	if (Lina_CoreMessageBus::Instance().GetInputEngine()->GetKey(SDL_SCANCODE_LEFT))
	{
		RotateY(-rot);
	}

	if (Lina_CoreMessageBus::Instance().GetInputEngine()->GetKey(SDL_SCANCODE_RIGHT))
	{
		RotateY(rot);
	}
}
Vector3 Lina_Camera::GetLeft()
{
	Vector3 left = forward.Cross(up);
	left.Normalize();
	return left;
}

Vector3 Lina_Camera::GetRight()
{
	Vector3 right = up.Cross(forward);
	right.Normalize();
	return right;
}

Vector3 Lina_Camera::GetPosition() { return position; }
Vector3 Lina_Camera::GetForward() { return forward; }
Vector3 Lina_Camera::GetUp() { return up; }
void Lina_Camera::SetPosition(Vector3 p) { position = p; };
void Lina_Camera::SetForward(Vector3 p) { forward = p; };
void Lina_Camera::SetUp(Vector3 p) { up = p; };