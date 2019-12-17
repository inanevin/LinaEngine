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

#include "ECS/ECSSystem.hpp"
#include "Utility/Math/Color.hpp"
#include "Utility/Math/Vector.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "PackageManager/PAMRenderDevice.hpp"

namespace LinaEngine
{
	namespace Graphics
	{
		class Shader;
	}	
}

namespace LinaEngine::ECS
{
	class LightingSystem : public BaseECSSystem
	{
	public:

		LightingSystem();

		FORCEINLINE void Construct(RenderDevice& renderDeviceIn, LinaEngine::Graphics::Shader& shaderIn)
		{
			m_Shader = &shaderIn;
			m_RenderDevice = &renderDeviceIn;
		}

		// Sets ambient light intensity.
		FORCEINLINE void SetAmbientIntensity(float intensity) { m_AmbientLight.intensity = intensity; }

		// Sets ambient light color.
		FORCEINLINE void SetAmbientColor(Color color) { m_AmbientLight.color = color; }

		// Gets ambient light intensity.
		FORCEINLINE float GetAmbientIntensity() { return m_AmbientLight.intensity; }

		// Gets ambient light color.
		FORCEINLINE Color GetAmbientColor() { return m_AmbientLight.color; }

		virtual void UpdateComponents(float delta, BaseECSComponent** components);


	private:

		RenderDevice* m_RenderDevice;

		LinaEngine::Graphics::Shader* m_Shader;
		Vector3F m_CameraPosition;
		LightComponent m_AmbientLight;
	};
}


#endif