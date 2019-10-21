/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: LightingSystem
Timestamp: 5/13/2019 12:49:19 AM

*/

#pragma once

#ifndef LightingSystem_HPP
#define LightingSystem_HPP

#include "ECS/EntityComponentSystem.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Lighting.hpp"


using namespace LinaEngine::Graphics;

namespace LinaEngine::ECS
{
	class LightingSystem : public BaseECSSystem
	{
	public:

		LightingSystem() : BaseECSSystem()
		{
			AddComponentType(TransformComponent::ID);
			
		}

		FORCEINLINE void Construct(PAMRenderDevice& renderDeviceIn, Shader& shaderIn)
		{
			m_Shader = &shaderIn;
			m_RenderDevice = &renderDeviceIn;
		}

		FORCEINLINE Vector3F GetCameraPosition()
		{
			return m_CameraPosition;
		}

		FORCEINLINE void SetCameraPosition(Vector3F& pos)
		{
			m_CameraPosition = pos;
		}

		FORCEINLINE void SetAmbientIntensity(float intensity) { m_AmbientLight.intensity = intensity; }
		FORCEINLINE void SetAmbientColor(Color color) { m_AmbientLight.color = color; }
		FORCEINLINE float GetAmbientIntensity() { return m_AmbientLight.intensity; }
		FORCEINLINE Color GetAmbientColor() { return m_AmbientLight.color; }

		virtual void UpdateComponents(float delta, BaseECSComponent** components);

	

	private:

		PAMRenderDevice* m_RenderDevice;
		AmbientLightComponent m_AmbientLight;
		Shader* m_Shader;
		Vector3F m_CameraPosition;

	};
}


#endif