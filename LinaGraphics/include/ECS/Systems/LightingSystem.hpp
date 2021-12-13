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

/*
Class: LightingSystem

Responsible for processing lights and light information to be used by rendering engine
to update the shaders that receive lighting.

Timestamp: 5/13/2019 12:49:19 AM
*/

#pragma once

#ifndef LightingSystem_HPP
#define LightingSystem_HPP

#include "ECS/ECS.hpp"
#include "Math/Color.hpp"
#include "Math/Vector.hpp"
#include "Core/RenderBackendFwd.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/LightComponent.hpp"


namespace Lina::ECS
{
	class LightingSystem : public BaseECSSystem
	{
	public:

		LightingSystem() {};

		virtual void Initialize() override;
		virtual void UpdateComponents(float delta) override;
		void SetLightingShaderData(uint32 shaderID);
		void ResetLightData();
		void SetAmbientColor(Color col) {m_ambientColor = col;}

		Matrix GetDirectionalLightMatrix();
		Matrix GetDirLightBiasMatrix();
		std::vector<Matrix> GetPointLightMatrices(Vector3 lightPos, Vector2 m_resolution, float near, float farPlane);
		const Vector3& GetDirectionalLightPos();

		DirectionalLightComponent* GetDirLight() { return std::get<1>(m_directionalLight); }
		Color GetAmbientColor() { return m_ambientColor; }
		std::vector<std::tuple<EntityDataComponent*, PointLightComponent*>>& GetPointLights() { return m_pointLights; }

	private:

		Lina::Graphics::RenderDevice* m_renderDevice = nullptr;
		Lina::Graphics::RenderEngine* m_renderEngine = nullptr;
		std::tuple < EntityDataComponent*, DirectionalLightComponent*> m_directionalLight;
		std::vector<std::tuple<EntityDataComponent*, PointLightComponent*>> m_pointLights;
		std::vector<std::tuple<EntityDataComponent*, SpotLightComponent*>> m_spotLights;
		Color m_ambientColor = Color(0.0f, 0.0f, 0.0f);
	};
}


#endif