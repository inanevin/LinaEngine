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
#include  "Rendering/RenderConstants.hpp"

using namespace LinaEngine::Graphics;

namespace LinaEngine::World
{
	void LinaEngine::World::Level::Install()
	{
		LINA_CORE_TRACE("[Constructor] -> Level Installed ({0})", typeid(*this).name());
		
		// Create & setup level skybox.
		LoadSkybox(SkyboxType::Gradient);
	}

	void Level::UpdateSkybox(SkyboxProperties newProperties)
	{

	}

	void Level::LoadSkybox(SkyboxType type)
	{
		if (type == SkyboxType::SingleColor)
		{
			// Create & set material.
			m_RenderEngine->CreateMaterial(MaterialConstants::skyboxMaterialName, ShaderConstants::skyboxSingleColorShader);
			m_RenderEngine->GetMaterial(MaterialConstants::skyboxMaterialName).SetColor(MaterialConstants::colorProperty, Colors::Black);
			m_RenderEngine->SetSkyboxMaterial(m_RenderEngine->GetMaterial(MaterialConstants::skyboxMaterialName));

			// Set level properties.
			m_SkyboxProperties.type == SkyboxType::SingleColor;
			m_SkyboxProperties.shaderID = ShaderConstants::skyboxSingleColorShader;
			m_SkyboxProperties.color1 = Colors::Black;
		}
		else if (type == SkyboxType::Gradient)
		{
			// Create & set material.
			m_RenderEngine->CreateMaterial(MaterialConstants::skyboxMaterialName, ShaderConstants::skyboxGradientShader);
			m_RenderEngine->GetMaterial(MaterialConstants::skyboxMaterialName).SetColor(MaterialConstants::startColorProperty, Colors::White);
			m_RenderEngine->GetMaterial(MaterialConstants::skyboxMaterialName).SetColor(MaterialConstants::endColorProperty, Colors::Black);
			m_RenderEngine->SetSkyboxMaterial(m_RenderEngine->GetMaterial(MaterialConstants::skyboxMaterialName));

			// Set level properties.
			m_SkyboxProperties.type == SkyboxType::SingleColor;
			m_SkyboxProperties.shaderID = ShaderConstants::skyboxGradientShader;
			m_SkyboxProperties.color1 = Colors::White;
			m_SkyboxProperties.color2 = Colors::Black;
		}
		else if (type == SkyboxType::Procedural)
		{
			// Create & set material.
			m_RenderEngine->CreateMaterial(MaterialConstants::skyboxMaterialName, ShaderConstants::skyboxProceduralShader);
			m_RenderEngine->GetMaterial(MaterialConstants::skyboxMaterialName).SetColor(MaterialConstants::startColorProperty, Colors::White);
			m_RenderEngine->GetMaterial(MaterialConstants::skyboxMaterialName).SetColor(MaterialConstants::endColorProperty, Colors::Black);
			m_RenderEngine->GetMaterial(MaterialConstants::skyboxMaterialName).SetVector3(MaterialConstants::sunDirectionProperty, Vector3F(0.0f, -1.0f, 0.0f));
			m_RenderEngine->SetSkyboxMaterial(m_RenderEngine->GetMaterial(MaterialConstants::skyboxMaterialName));

			// Set level properties.
			m_SkyboxProperties.type == SkyboxType::SingleColor;
			m_SkyboxProperties.shaderID = ShaderConstants::skyboxProceduralShader;
			m_SkyboxProperties.color1 = Colors::White;
			m_SkyboxProperties.color2 = Colors::Black;

		}
		else if (type == SkyboxType::Cubemap)
		{
			m_RenderEngine->CreateMaterial(MaterialConstants::skyboxMaterialName, ShaderConstants::skyboxCubemapShader);

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
			m_RenderEngine->GetMaterial(MaterialConstants::skyboxMaterialName).SetTexture(MaterialConstants::diffuseTextureProperty, &m_RenderEngine->GetTexture("skyboxTexture"), 0, LinaEngine::Graphics::BindTextureMode::BINDTEXTURE_CUBEMAP);
			m_RenderEngine->SetSkyboxMaterial(m_RenderEngine->GetMaterial(MaterialConstants::skyboxMaterialName));

			// Set level properties.
			m_SkyboxProperties.type == SkyboxType::SingleColor;
			m_SkyboxProperties.shaderID = ShaderConstants::skyboxCubemapShader;
		
		}

	}
}

