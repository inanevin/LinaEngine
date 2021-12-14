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

#include "Log/Log.hpp"
#include "World/Level.hpp"
#include "ECS/ECS.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Core/Application.hpp"
#include "Core/RenderEngineBackend.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/FreeLookComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/SpriteRendererComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "ECS/Components/RigidbodyComponent.hpp"
#include "ECS/Components/ModelRendererComponent.hpp"
#include <cereal/archives/json.hpp>
#include <cereal/archives/xml.hpp>
#include <stdio.h>
#include <fstream>


namespace Lina::World
{
	bool Level::Install(bool loadFromFile, const std::string& path, const std::string& levelName)
	{
		Lina::Event::EventSystem::Get()->Connect<Event::ETick, &Level::Tick>(this);

		if (loadFromFile)
		{
			if (Lina::Utility::FileExists(path + "/" + levelName + ".linalevel"))
			{
				DeserializeLevelData(path, levelName);
				LoadLevelResources();

			}
		}
		return true;
	}

	void Level::Uninstall()
	{
		Lina::Event::EventSystem::Get()->Disconnect<Event::ETick>(this);
	}

	void Level::LoadLevelResources()
	{
		ECS::Registry* ecs = ECS::Registry::Get();

		auto view = ecs->view<ECS::ModelRendererComponent>();
		LINA_TRACE("Loading Level Resources...");

		for (ECS::Entity entity : view)
		{
			ECS::ModelRendererComponent& mr = view.get<ECS::ModelRendererComponent>(entity);

			// Load the model pointed by the model renderer.
			if (!Graphics::Model::ModelExists(mr.m_modelPath))
			{
				if (Utility::FileExists(mr.m_modelPath))
				{
					const std::string paramsPath = Utility::FileExists(mr.m_modelParamsPath) ? mr.m_modelParamsPath : "";
					Graphics::Model& model = Graphics::Model::CreateModel(mr.m_modelPath, Graphics::ModelParameters(), -1, paramsPath);
					mr.m_modelID = model.GetID();
				}
			}
			else
			{
				mr.m_modelID = Graphics::Model::GetModel(mr.m_modelPath).GetID();
			}


			// Load all the materials pointed by the model renderer.
			for (int i = 0; i < mr.m_materialPaths.size(); i++)
			{
				auto& path = mr.m_materialPaths[i];
				if (!Graphics::Material::MaterialExists(path))
				{
					if (Utility::FileExists(path))
					{
						Graphics::Material::LoadMaterialFromFile(path);
					}
				}

			}

			// Iterate model renderer's children, find all mesh renderer components & assign their mesh & material IDs accordingly.
			ECS::EntityDataComponent& data = ecs->get<ECS::EntityDataComponent>(entity);
			for (auto& child : data.m_children)
			{
				ECS::MeshRendererComponent* meshRenderer = ecs->try_get<ECS::MeshRendererComponent>(child);

				if (meshRenderer != nullptr)
				{
					if (Graphics::Material::MaterialExists(meshRenderer->m_materialPath))
						meshRenderer->m_materialID = Graphics::Material::GetMaterial(meshRenderer->m_materialPath).GetID();

					meshRenderer->m_modelID = Graphics::Model::GetModel(meshRenderer->m_modelPath).GetID();
				}
			}
		}

		auto viewSprites = ecs->view<ECS::SpriteRendererComponent>();

		for (ECS::Entity entity : viewSprites)
		{
			ECS::SpriteRendererComponent& sprite = viewSprites.get<ECS::SpriteRendererComponent>(entity);

			if (!Graphics::Material::MaterialExists(sprite.m_materialPaths))
			{
				if (Utility::FileExists(sprite.m_materialPaths))
				{
					Graphics::Material& mat = Graphics::Material::LoadMaterialFromFile(sprite.m_materialPaths);
					sprite.m_materialID = mat.GetID();

					// Load material textures.
					for (std::map<std::string, Graphics::MaterialSampler2D>::iterator it = mat.m_sampler2Ds.begin(); it != mat.m_sampler2Ds.end(); ++it)
					{
						if (Utility::FileExists(it->second.m_path))
						{
							Graphics::SamplerParameters samplerParams;

							if (Utility::FileExists(it->second.m_paramsPath))
								samplerParams = Graphics::Texture::LoadParameters(it->second.m_paramsPath);

							Graphics::Texture& texture = Graphics::Texture::CreateTexture2D(it->second.m_path, samplerParams, false, false, it->second.m_paramsPath);

							mat.SetTexture(it->first, &texture, it->second.m_bindMode);
						}
					}
				}
			}
			else
			{
				Graphics::Material& mat = Graphics::Material::GetMaterial(sprite.m_materialPaths);
				sprite.m_materialID = mat.GetID();
			}
		}

		Lina::Graphics::RenderEngineBackend* renderEngine = Lina::Graphics::RenderEngineBackend::Get();

		if (Utility::FileExists(m_levelData.m_skyboxMaterialPath))
		{
			Graphics::Material& mat = Graphics::Material::LoadMaterialFromFile(m_levelData.m_skyboxMaterialPath);
			renderEngine->SetSkyboxMaterial(&mat);
		}
		else
			renderEngine->SetSkyboxMaterial(nullptr);

		renderEngine->GetLightingSystem()->SetAmbientColor(m_levelData.m_ambientColor);
	}

	void Level::SetSkyboxMaterial()
	{
		Lina::Graphics::RenderEngineBackend* renderEngine = Lina::Graphics::RenderEngineBackend::Get();

		if (Graphics::Material::MaterialExists(m_levelData.m_skyboxMaterialPath))
		{
			renderEngine->SetSkyboxMaterial(&Graphics::Material::GetMaterial(m_levelData.m_skyboxMaterialPath));
		}
		else
			renderEngine->SetSkyboxMaterial(nullptr);
	}


	void Level::SerializeLevelData(const std::string& path, const std::string& levelName)
	{
		Lina::ECS::Registry* registry = ECS::Registry::Get();
		{

			std::ofstream levelDataStream(path + "/" + levelName + ".linalevel", std::ios::binary);
			{
				cereal::PortableBinaryOutputArchive oarchive(levelDataStream); // Build an output archive
				//
				oarchive(m_levelData); // write the level data to the archive.
				registry->SerializeComponentsInRegistry(oarchive);
			}
		}
	}

	void Level::DeserializeLevelData(const std::string& path, const std::string& levelName)
	{
		Lina::ECS::Registry* registry = ECS::Registry::Get();

		{
			std::ifstream levelDataStream(path + "/" + levelName + ".linalevel", std::ios::binary);
			{
				cereal::PortableBinaryInputArchive iarchive(levelDataStream);

				// Read the data into it.
				iarchive(m_levelData);

				registry->clear();
				registry->DeserializeComponentsInRegistry(iarchive);
			}
		}
	}
}