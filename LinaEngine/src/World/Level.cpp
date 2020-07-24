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
namespace LinaEngine::World
{
	Level* Level::SerializeLevel(Level& level)
	{
		std::ofstream os("levelname.linalevel");
		{
			cereal::BinaryOutputArchive oarchive(os); // Create an output archive

			oarchive(level); // Write the data to the archive
		} // archive goes out of scope, ensuring all contents are flushed

		return &level;
	}

	Level* Level::DeserializeLevel(const std::string& path)
	{
		std::ifstream is("levelname.linalevel");

		{
			cereal::BinaryInputArchive iarchive(is);

			// Create the level.
			Level* readLevel = new Level();

			// Read the data into it.
			iarchive(*readLevel);

			// Return the created level's pointer.
			return readLevel;
		}
	}
}