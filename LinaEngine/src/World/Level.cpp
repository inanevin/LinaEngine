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


using namespace LinaEngine::Graphics;

namespace LinaEngine::World
{
	void LinaEngine::World::Level::Install()
	{
		LINA_CORE_TRACE("[Constructor] -> Level Installed ({0})", typeid(*this).name());
		
		// Create & setup level skybox.
		CreateSkyboxMaterial(m_SkyboxProperties);
	}

	void Level::CreateSkyboxMaterial(SkyboxProperties properties)
	{
		if (properties.type == SkyboxType::SingleColor)
		{
			// Create material.
			m_RenderEngine->CreateMaterial(MaterialConstants::skyboxMaterialName, ShaderConstants::skyboxSingleColorShader);

			// Store material reference.
			Material& skyboxMaterial = m_RenderEngine->GetMaterial(MaterialConstants::skyboxMaterialName);

			// Set properties.
			skyboxMaterial.SetColor(MaterialConstants::colorProperty, properties.color1);
			m_RenderEngine->SetSkyboxMaterial(skyboxMaterial);

			// Set level properties.
			m_SkyboxProperties.type == SkyboxType::SingleColor;
			m_SkyboxProperties.shaderID = ShaderConstants::skyboxSingleColorShader;
			m_SkyboxProperties.color1 = Colors::Black;
		}
		else if (properties.type == SkyboxType::Gradient)
		{
			// Create material.
			m_RenderEngine->CreateMaterial(MaterialConstants::skyboxMaterialName, ShaderConstants::skyboxGradientShader);

			// Store material reference.
			Material& skyboxMaterial = m_RenderEngine->GetMaterial(MaterialConstants::skyboxMaterialName);

			// Set properties.
			skyboxMaterial.SetColor(MaterialConstants::startColorProperty, properties.color1);
			skyboxMaterial.SetColor(MaterialConstants::endColorProperty, properties.color2);
			m_RenderEngine->SetSkyboxMaterial(skyboxMaterial);

			// Set level properties.
			m_SkyboxProperties.type == SkyboxType::SingleColor;
			m_SkyboxProperties.shaderID = ShaderConstants::skyboxGradientShader;
			m_SkyboxProperties.color1 = Colors::Black;
			m_SkyboxProperties.color2 = Colors::White;
		}
		else if (properties.type == SkyboxType::Procedural)
		{
			// Create material.
			m_RenderEngine->CreateMaterial(MaterialConstants::skyboxMaterialName, ShaderConstants::skyboxProceduralShader);

			// Store material reference.
			Material& skyboxMaterial = m_RenderEngine->GetMaterial(MaterialConstants::skyboxMaterialName);

			// Set properties.
			skyboxMaterial.SetColor(MaterialConstants::startColorProperty, properties.color1);
			skyboxMaterial.SetColor(MaterialConstants::endColorProperty, properties.color2);
			skyboxMaterial.SetVector3(MaterialConstants::sunDirectionProperty, Vector3F(0.0f, -1.0f, 0.0f));
			m_RenderEngine->SetSkyboxMaterial(skyboxMaterial);

			// Set level properties.
			m_SkyboxProperties.type == SkyboxType::SingleColor;
			m_SkyboxProperties.shaderID = ShaderConstants::skyboxProceduralShader;
			m_SkyboxProperties.color1 = Colors::Black;
			m_SkyboxProperties.color2 = Colors::White;

		}
		else if (properties.type == SkyboxType::Cubemap)
		{
			// Create material.
			m_RenderEngine->CreateMaterial(MaterialConstants::skyboxMaterialName, ShaderConstants::skyboxCubemapShader);

			// Store material reference.
			Material& skyboxMaterial = m_RenderEngine->GetMaterial(MaterialConstants::skyboxMaterialName);

			const std::string fp[6] = {

				"resources/textures/defaultSkybox/right.png",
				"resources/textures/defaultSkybox/left.png",
				"resources/textures/defaultSkybox/up.png",
				"resources/textures/defaultSkybox/down.png",
				"resources/textures/defaultSkybox/front.png",
				"resources/textures/defaultSkybox/back.png",
			};


			// Set properties.
			LinaEngine::Graphics::SamplerData data = LinaEngine::Graphics::SamplerData();
			data.minFilter = LinaEngine::Graphics::FILTER_NEAREST;
			m_RenderEngine->CreateTexture("skyboxTexture", fp, LinaEngine::Graphics::PixelFormat::FORMAT_RGB, true, false, data);
			skyboxMaterial.SetTexture(MaterialConstants::diffuseTextureProperty, &m_RenderEngine->GetTexture("skyboxTexture"), 0, LinaEngine::Graphics::BindTextureMode::BINDTEXTURE_CUBEMAP);
			m_RenderEngine->SetSkyboxMaterial(skyboxMaterial);

			// Set level properties.
			m_SkyboxProperties.type == SkyboxType::SingleColor;
			m_SkyboxProperties.shaderID = ShaderConstants::skyboxCubemapShader;
		
		}

	}
}

