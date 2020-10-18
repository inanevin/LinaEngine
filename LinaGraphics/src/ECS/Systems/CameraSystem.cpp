/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "ECS/Systems/CameraSystem.hpp"  
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "Utility/Math/Math.hpp"

namespace LinaEngine::ECS
{

	void CameraSystem::UpdateComponents(float delta)
	{
		auto view = m_ecs->view<TransformComponent, CameraComponent>();
		if (view.size() == 0)
		{
			m_currentCameraComponent = nullptr;
			m_currentCameraTransform = nullptr;
		}

		// Find cameras, select the most active one, and 
		// update projection & view matrices according to it's data.
		for (auto entity : view)
		{
			CameraComponent& camera = view.get<CameraComponent>(entity);

			// If the current camera component exists and not active, continue
			if (!camera.m_isEnabled)
			{
				if(&camera == m_currentCameraComponent)
				{
					m_currentCameraComponent = nullptr;
					m_currentCameraTransform = nullptr;
				}
				continue;
			}

			TransformComponent& transform = view.get<TransformComponent>(entity);

			m_currentCameraComponent = &camera;
			m_currentCameraTransform = &transform;
			
			// Actual camera view matrix.
			m_view = Matrix::InitLookAt(transform.transform.m_location, transform.transform.m_location + transform.transform.m_rotation.GetForward(), transform.transform.m_rotation.GetUp());

			// Update projection matrix.
			m_projection = Matrix::Perspective(camera.m_fieldOfView / 2, m_aspectRatio, camera.m_zNear, camera.m_zFar);		
	
		}	
	}

	Vector3 CameraSystem::GetCameraLocation()
	{
		return (m_currentCameraComponent == nullptr || m_currentCameraTransform == nullptr)? Vector3(Vector3::Zero): m_currentCameraTransform->transform.m_location;
	}

	Matrix CameraSystem::GetLightMatrix(DirectionalLightComponent* c)
	{
		if (c == nullptr) return Matrix();
		Matrix lightProjection = Matrix::Orthographic(c->m_shadowOrthoProjection.x, c->m_shadowOrthoProjection.y, c->m_shadowOrthoProjection.z, c->m_shadowOrthoProjection.w, c->m_shadowZNear, c->m_shadowZFar);;
		Matrix lightView = Matrix::InitLookAt(m_currentCameraTransform->transform.m_location, m_currentCameraTransform->transform.m_location + m_currentCameraTransform->transform.m_rotation.GetForward().Normalized(), Vector3::Up);
		return lightProjection * lightView;
	}

	LinaEngine::Color& CameraSystem::GetCurrentClearColor()
	{
		return m_currentCameraComponent == nullptr ? LinaEngine::Color::Gray : m_currentCameraComponent->m_clearColor;
	}


}

