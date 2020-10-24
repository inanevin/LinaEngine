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
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/FreeLookComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/SpriteRendererComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "ECS/Components/RigidbodyComponent.hpp"
#include <cereal/archives/json.hpp>
#include <stdio.h>
#include <cereal/archives/binary.hpp>
#include <fstream>


namespace LinaEngine::World
{
	bool Level::Install(bool loadFromFile, const std::string& path, const std::string& levelName)
	{
		if (loadFromFile)
			DeserializeLevelData(path, levelName);

		return true;
	}

	void Level::SerializeLevelData(const std::string& path, const std::string& levelName)
	{
		LinaEngine::ECS::ECSRegistry& registry = LinaEngine::Application::GetECSRegistry();


		std::ofstream registrySnapshotStream(path + "/" + levelName + "_ecsSnapshot.linasnapshot");
		{
			cereal::BinaryOutputArchive oarchive(registrySnapshotStream); // Create an output archive

			entt::snapshot{ registry }
				.entities(oarchive)
				.component<
				LinaEngine::ECS::ECSEntityData, 
				LinaEngine::ECS::CameraComponent,
				LinaEngine::ECS::FreeLookComponent,
				LinaEngine::ECS::PointLightComponent,
				LinaEngine::ECS::DirectionalLightComponent,
				LinaEngine::ECS::SpotLightComponent,
				LinaEngine::ECS::RigidbodyComponent,
				LinaEngine::ECS::MeshRendererComponent,
				LinaEngine::ECS::SpriteRendererComponent,
				LinaEngine::ECS::TransformComponent
				>(oarchive);
		}

		std::ofstream levelDataStream(path + "/" + levelName + ".linaleveldata");
		{
			cereal::BinaryOutputArchive oarchive(levelDataStream); // Create an output archive

			oarchive(m_levelData); // Write the data to the archive
		} 

	}

	void Level::DeserializeLevelData(const std::string& path, const std::string& levelName)
	{
		LinaEngine::ECS::ECSRegistry& registry = LinaEngine::Application::GetECSRegistry();

		std::ifstream levelDataStream(path + "/" + levelName + ".linaleveldata");
		{
			cereal::BinaryInputArchive iarchive(levelDataStream);

			// Read the data into it.
			iarchive(m_levelData);

		}

		registry.clear();

		std::ifstream regSnapshotStream(path + "/" + levelName + "_ecsSnapshot.linasnapshot");
		{
			cereal::BinaryInputArchive iarchive(regSnapshotStream);

			entt::snapshot_loader{ registry }
				.entities(iarchive)
				.component<
				LinaEngine::ECS::ECSEntityData,
				LinaEngine::ECS::CameraComponent,
				LinaEngine::ECS::FreeLookComponent,
				LinaEngine::ECS::PointLightComponent,
				LinaEngine::ECS::DirectionalLightComponent,
				LinaEngine::ECS::SpotLightComponent,
				LinaEngine::ECS::RigidbodyComponent,
				LinaEngine::ECS::MeshRendererComponent,
				LinaEngine::ECS::SpriteRendererComponent,
				LinaEngine::ECS::TransformComponent
				>(iarchive);
				
		}

	}
}