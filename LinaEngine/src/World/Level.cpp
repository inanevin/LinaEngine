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

#include "World/Level.hpp"
#include "ECS/ECS.hpp"
#include "Core/Application.hpp"
#include "Rendering/RenderEngine.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/FreeLookComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/SpriteRendererComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "ECS/Components/RigidbodyComponent.hpp"
#include <cereal/archives/json.hpp>
#include <cereal/archives/xml.hpp>
#include <stdio.h>
#include <fstream>


namespace LinaEngine::World
{
	bool Level::Install(bool loadFromFile, const std::string& path, const std::string& levelName)
	{
		if (loadFromFile)
		{
			if (LinaEngine::Utility::FileExists(path + "/" + levelName + ".linaleveldata"))
			{
				DeserializeLevelData(path, levelName);
				LoadLevelResources();
			}
		}
		return true;
	}

	void Level::LoadLevelResources()
	{
		ECS::ECSRegistry& ecs = Application::GetECSRegistry();

		/*auto view = ecs.view<ECS::MeshRendererComponent>();

		for (ECS::ECSEntity entity : view)
		{
			ECS::MeshRendererComponent& mr = view.get<ECS::MeshRendererComponent>(entity);
			LINA_CORE_TRACE("Mat size {0} ", mr.m_materialPath.size());

			for (int i = 0; i < mr.m_materialPath.size(); i++)
			{
				mr.m_materialID.push_back(0);
				auto& path = mr.m_materialPath[i];
				auto& id = mr.m_materialID[i];

				// Load used materials.
				if (!Graphics::Material::MaterialExists(path))
				{
					if (Utility::FileExists(path))
					{
						Graphics::Material& mat = Graphics::Material::LoadMaterialFromFile(path);
						id = mat.GetID();

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
					Graphics::Material& mat = Graphics::Material::GetMaterial(path);
					id = mat.GetID();
				}
			}

			// Load used meshes
			if (!Graphics::Model::ModelExists(mr.m_meshPath))
			{

				Graphics::ModelParameters params;
				if (Utility::FileExists(mr.m_meshParamsPath))
					params = Graphics::Model::LoadParameters(mr.m_meshParamsPath);

				Graphics::Model& mesh = Graphics::Model::CreateModel(mr.m_meshPath, params, -1, mr.m_meshParamsPath);
				mr.m_meshID = mesh.GetID();
			}
			else
				mr.m_meshID = Graphics::Model::GetModel(mr.m_meshPath).GetID();
		}*/

		auto viewSprites = ecs.view<ECS::SpriteRendererComponent>();

		for (ECS::ECSEntity entity : viewSprites)
		{
			ECS::SpriteRendererComponent& sprite = viewSprites.get<ECS::SpriteRendererComponent>(entity);

			if (!Graphics::Material::MaterialExists(sprite.m_materialPath))
			{
				if (Utility::FileExists(sprite.m_materialPath))
				{
					Graphics::Material& mat = Graphics::Material::LoadMaterialFromFile(sprite.m_materialPath);
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
				Graphics::Material& mat = Graphics::Material::GetMaterial(sprite.m_materialPath);
				sprite.m_materialID = mat.GetID();
			}
		}

		LinaEngine::Graphics::RenderEngine& renderEngine = LinaEngine::Application::GetRenderEngine();

		if (Utility::FileExists(m_levelData.m_skyboxMaterialPath))
		{
			Graphics::Material& mat = Graphics::Material::LoadMaterialFromFile(m_levelData.m_skyboxMaterialPath);
			renderEngine.SetSkyboxMaterial(&mat);
		}
		else
			renderEngine.SetSkyboxMaterial(nullptr);

		renderEngine.GetLightingSystem()->SetAmbientColor(m_levelData.m_ambientColor);
	}

	void Level::SetSkyboxMaterial()
	{
		LinaEngine::Graphics::RenderEngine& renderEngine = LinaEngine::Application::GetRenderEngine();

		if (Graphics::Material::MaterialExists(m_levelData.m_skyboxMaterialPath))
		{
			renderEngine.SetSkyboxMaterial(&Graphics::Material::GetMaterial(m_levelData.m_skyboxMaterialPath));
		}
		else
			renderEngine.SetSkyboxMaterial(nullptr);
	}


	void Level::SerializeLevelData(const std::string& path, const std::string& levelName)
	{
		LinaEngine::ECS::ECSRegistry& registry = LinaEngine::Application::GetECSRegistry();

		{

			std::ofstream registrySnapshotStream(path + "/" + levelName + "_ecsSnapshot.linasnapshot", std::ios::binary);
			{
				cereal::BinaryOutputArchive oarchive(registrySnapshotStream); // Build an output archive
				LinaEngine::Application::GetApp().SerializeRegistry(registry, oarchive);
			}

		}
		{


			std::ofstream levelDataStream(path + "/" + levelName + ".linaleveldata", std::ios::binary);
			{
				cereal::BinaryOutputArchive oarchive(levelDataStream); // Build an output archive

				oarchive(m_levelData); // Write the data to the archive
			}
		}
	}

	void Level::DeserializeLevelData(const std::string& path, const std::string& levelName)
	{
		LinaEngine::ECS::ECSRegistry& registry = LinaEngine::Application::GetECSRegistry();

		{
			std::ifstream levelDataStream(path + "/" + levelName + ".linaleveldata", std::ios::binary);
			{
				cereal::BinaryInputArchive iarchive(levelDataStream);

				// Read the data into it.
				iarchive(m_levelData);

			}
		}

		registry.clear();

		{
			std::ifstream regSnapshotStream(path + "/" + levelName + "_ecsSnapshot.linasnapshot", std::ios::binary);
			{
				cereal::BinaryInputArchive iarchive(regSnapshotStream);
				LinaEngine::Application::GetApp().DeserializeRegistry(registry, iarchive);
			}
		}

		registry.Refresh();

	}
}