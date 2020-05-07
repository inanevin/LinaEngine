/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Level
Timestamp: 1/2/2019 1:43:13 AM

*/

#include "LinaPch.hpp"
#include "World/Level.hpp"  
#include "Utility/Log.hpp"
#include "Rendering/RenderEngine.hpp"
#include  "Rendering/ShaderConstants.hpp"

namespace LinaEngine::World
{
	void LinaEngine::World::Level::Install()
	{
		LINA_CORE_TRACE("[Constructor] -> Level Installed ({0})", typeid(*this).name());
		
		// Create & setup level skybox.
		LoadSkybox(SkyboxType::Gradient);
	}

	void Level::LoadSkybox(SkyboxType type)
	{
		if (type == SkyboxType::SingleColor)
		{
			m_RenderEngine->CreateMaterial("skyboxMaterial", LinaEngine::Graphics::ShaderConstants::skyboxSingleColorShader);
			m_RenderEngine->GetMaterial("skyboxMaterial").SetColor("material.color", Colors::Yellow);
			m_RenderEngine->SetSkyboxMaterial(m_RenderEngine->GetMaterial("skyboxMaterial"));
		}
		else if (type == SkyboxType::Gradient)
		{
			m_RenderEngine->CreateMaterial("skyboxMaterial", LinaEngine::Graphics::ShaderConstants::skyboxGradientShader);
			m_RenderEngine->GetMaterial("skyboxMaterial").SetColor("material.startColor", Colors::Red);
			m_RenderEngine->GetMaterial("skyboxMaterial").SetColor("material.endColor", Colors::White);
			m_RenderEngine->SetSkyboxMaterial(m_RenderEngine->GetMaterial("skyboxMaterial"));
		}
		else if (type == SkyboxType::Procedural)
		{
			m_RenderEngine->CreateMaterial("skyboxMaterial", LinaEngine::Graphics::ShaderConstants::skyboxProceduralShader);
			m_RenderEngine->GetMaterial("skyboxMaterial").SetColor("material.startColor", Colors::Maroon);
			m_RenderEngine->GetMaterial("skyboxMaterial").SetColor("material.endColor", Colors::Red);
			m_RenderEngine->GetMaterial("skyboxMaterial").SetVector3("material.sunDirection", Vector3F(0.0f, -1.0f, 0.0f));
			m_RenderEngine->SetSkyboxMaterial(m_RenderEngine->GetMaterial("skyboxMaterial"));
		}
		else if (type == SkyboxType::Cubemap)
		{
			m_RenderEngine->CreateMaterial("skyboxMaterial", LinaEngine::Graphics::ShaderConstants::skyboxCubemapShader);

			const std::string fp[6] = {

				"resources/textures/defaultSkybox/right.png",
				"resources/textures/defaultSkybox/left.png",
				"resources/textures/defaultSkybox/up.png",
				"resources/textures/defaultSkybox/down.png",
				"resources/textures/defaultSkybox/front.png",
				"resources/textures/defaultSkybox/back.png",
			};
			LinaEngine::Graphics::SamplerData data = LinaEngine::Graphics::SamplerData();
			data.minFilter = LinaEngine::Graphics::FILTER_NEAREST;
			m_RenderEngine->CreateTexture("skyboxTexture", fp, LinaEngine::Graphics::PixelFormat::FORMAT_RGB, true, false, data);
			m_RenderEngine->GetMaterial("skyboxMaterial").SetTexture("material.diffuse", &m_RenderEngine->GetTexture("skyboxTexture"), 0, LinaEngine::Graphics::BindTextureMode::BINDTEXTURE_CUBEMAP);
			m_RenderEngine->SetSkyboxMaterial(m_RenderEngine->GetMaterial("skyboxMaterial"));
		}

	}
}

