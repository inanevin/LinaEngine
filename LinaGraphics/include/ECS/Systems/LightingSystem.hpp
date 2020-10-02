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

#include "ECS/ECS.hpp"
#include "Utility/Math/Color.hpp"
#include "Utility/Math/Vector.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "PackageManager/PAMRenderDevice.hpp"


namespace LinaEngine
{
	namespace Graphics
	{
		class RenderEngine;
	}	
}

namespace LinaEngine::ECS
{
	class LightingSystem : public BaseECSSystem
	{
	public:

		LightingSystem() {};

		FORCEINLINE void Construct(ECSRegistry& registry, RenderDevice& rdIn, Graphics::RenderEngine& renderEngineIn)
		{
			BaseECSSystem::Construct(registry);
			m_RenderDevice = &rdIn;
			m_RenderEngine = &renderEngineIn;
		}


		virtual void UpdateComponents(float delta) override;
		void SetLightingShaderData(uint32 shaderID);
		void ResetLightData();
		Matrix GetDirectionalLightMatrix();
		std::vector<Matrix> GetPointLightMatrices();
		FORCEINLINE Color& GetAmbientColor() { return m_ambientColor; }
		Vector3& GetDirectionalLightPos();

	private:

		RenderDevice* m_RenderDevice;
		Graphics::RenderEngine* m_RenderEngine;
		std::tuple < TransformComponent*, DirectionalLightComponent*> directionalLight;
		std::vector<std::tuple<TransformComponent*, PointLightComponent*>> pointLights;
		std::vector<std::tuple<TransformComponent*, SpotLightComponent*>> spotLights;
		Color m_ambientColor = Color(0.0f, 0.0f, 0.0f);
	};
}


#endif