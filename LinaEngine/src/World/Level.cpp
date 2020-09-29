/*
Author: Inan Evin
www.inanevin.com
https://github.com/inanevin/LinaEngine

Copyright 2020~ Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Level
Timestamp: 5/23/2020 2:23:02 PM

*/

#include "World/Level.hpp"
#include <stdio.h>
#include <cereal/archives/binary.hpp>
#include <fstream>
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/FreeLookComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/LightComponent.hpp"

#include <cereal/archives/json.hpp>

namespace LinaEngine::World
{
	void Level::SerializeLevelData(const std::string& path, const std::string& levelName, Level& level, LinaEngine::ECS::ECSRegistry& registry)
	{

		std::ofstream reg(path + "/" + levelName + "_ecsreg.linaregistry");
		{
			cereal::BinaryOutputArchive oarchive(reg); // Create an output archive

			entt::snapshot{ registry }
				.entities(oarchive)
				.component<LinaEngine::ECS::TransformComponent,
				LinaEngine::ECS::CameraComponent,
				LinaEngine::ECS::FreeLookComponent,
				LinaEngine::ECS::LightComponent,
				LinaEngine::ECS::MeshRendererComponent>(oarchive);
		}

		std::ofstream os(path + "/" + levelName + ".linaleveldata");
		{
			cereal::BinaryOutputArchive oarchive(os); // Create an output archive

			oarchive(level.m_LevelData); // Write the data to the archive
		} // archive goes out of scope, ensuring all contents are flushed

	}

	void Level::DeserializeLevelData(const std::string& path, const std::string& levelName, Level& level, LinaEngine::ECS::ECSRegistry& registry)
	{


		std::ifstream is(path + "/" + levelName + ".linaleveldata");
		{
			cereal::BinaryInputArchive iarchive(is);

			// Read the data into it.
			iarchive(level.m_LevelData);

		}

		registry.clear();

		//std::ifstream reg(path + "/" + levelName + "_ecsreg.linaregistry");
		//{
		//	cereal::BinaryInputArchive iarchive(reg);
		//
		//	entt::snapshot_loader{ registry}
		//		.entities(iarchive)
		//		.component<LinaEngine::ECS::TransformComponent, 
		//		LinaEngine::ECS::CameraComponent,
		//		LinaEngine::ECS::FreeLookComponent,
		//		LinaEngine::ECS::LightComponent,
		//		LinaEngine::ECS::MeshRendererComponent>(iarchive)
		//		.orphans();
		//}
	}
}