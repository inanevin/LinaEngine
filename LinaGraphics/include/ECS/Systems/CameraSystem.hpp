/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: CameraSystem
Timestamp: 5/2/2019 12:40:46 AM

*/

#pragma once

#ifndef CameraSystem_HPP
#define CameraSystem_HPP

#include "ECS/ECSSystem.hpp"
#include "Utility/Math/Matrix.hpp"
#include "Utility/Math/Color.hpp"

namespace LinaEngine
{
	namespace Graphics
	{
		class RenderContext;
	}
}

namespace LinaEngine::ECS
{
	struct TransformComponent;
	struct CameraComponent;

	class CameraSystem : public BaseECSSystem
	{
	public:

		CameraSystem() {};
		virtual void UpdateComponents(float delta) override;

		// Construct the system.
		FORCEINLINE void Construct(ECSRegistry& registry, LinaEngine::Graphics::RenderContext& contextIn) { BaseECSSystem::Construct(registry); context = &contextIn; }

		// Get view matrix.
		FORCEINLINE Matrix& GetViewMatrix() { return m_View; }

		// Get view matrix for skybox.
		FORCEINLINE Matrix& GetSkyboxViewTransformation() { return m_SkyboxViewTransformation; }

		// Get projection matrix
		FORCEINLINE Matrix& GetProjectionMatrix() { return m_Projection; }

		// Get camera location, if camera is not defined, get zero.
		Vector3F GetCameraLocation();

		// Set aspect ratio for the camera.
		FORCEINLINE void SetAspectRatio(float aspect) { m_AspectRatio = aspect; }

		// Get clear color of the current camera.
		Color& GetCurrentClearColor();

		FORCEINLINE CameraComponent& GetCurrentCameraComponent() { return *m_CurrentCameraComponent; }
	private:

		LinaEngine::Graphics::RenderContext* context = nullptr;
		Matrix m_View = Matrix::identity();
		Matrix m_Projection =Matrix::perspective(35, 1.33f, 0.01f, 1000.0f);
		Matrix m_SkyboxViewTransformation = Matrix::identity();
		CameraComponent* m_CurrentCameraComponent = nullptr;
		TransformComponent* m_CurrentCameraTransform = nullptr;
		float m_AspectRatio = 1.33f;
		
	};
}


#endif