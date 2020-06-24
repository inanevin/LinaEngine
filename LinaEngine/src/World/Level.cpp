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
#include <cereal/archives/xml.hpp>
#include <fstream>

namespace LinaEngine::World
{
	struct MyClass
	{
		int x, y, z;

		// This method lets cereal know which data members to serialize
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(x, y, z); // serialize things by passing them to the archive
		}
	};

	void Level::SerializeLevel()
	{

		{
			std::ofstream os("data.xml");
			MyClass m1;
			cereal::XMLOutputArchive archive(os);


			entt::snapshot{ *m_ECS }.entities(archive);
			archive(CEREAL_NVP(m1));

			//MyData m1, m2, m3;
			//oarchive(m1, m2, m3); // Write the data to the archive
		} // archive goes out of scope, ensuring all contents are flushed

	} 
}