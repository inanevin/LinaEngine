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
#include <cereal/archives/json.hpp>
namespace LinaEngine::World
{
	Level* Level::SerializeLevel(const std::string& path, const std::string& levelName, Level& level)
	{

		std::ofstream reg(path + "ecsreg.linaregistry");
		{
			cereal::BinaryOutputArchive oarchive(reg); // Create an output archive

			entt::snapshot{ *level.m_ECS }
				.entities(oarchive)
				.component<LinaEngine::ECS::TransformComponent>(oarchive);
		}

		std::ofstream os(path + levelName);
		{
			cereal::BinaryOutputArchive oarchive(os); // Create an output archive

			oarchive(level); // Write the data to the archive
		} // archive goes out of scope, ensuring all contents are flushed

		return &level;
	}

	Level* Level::DeserializeLevel(const std::string& path, const std::string& levelName)
	{

		// Create the level.
		Level* readLevel = new Level();

		std::ifstream is(path + levelName);
		{
			cereal::BinaryInputArchive iarchive(is);

			// Read the data into it.
			iarchive(*readLevel);

		}

		std::ifstream reg(path + "ecsreg.linaregistry");
		{
			cereal::BinaryInputArchive iarchive(reg);

			entt::snapshot_loader{ *readLevel->m_ECS }
				.entities(iarchive)
				.component<LinaEngine::ECS::TransformComponent>(iarchive)
				.orphans();
		}


		// Return the created level's pointer.
		return readLevel;
	}
}