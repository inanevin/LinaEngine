/* 
This file is a part of: Lina Engine
https://github.com/inanevin/Lina

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

#include "Resources/LevelResource.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Log/Log.hpp"
#include <cereal/archives/portable_binary.hpp>
#include <fstream>


namespace Lina::Resources
{
	bool LevelResource::Export(const std::string& path, ECS::Registry& ecs)
    {

		if (Utility::FileExists(path))
			Utility::DeleteFileInPath(path);

		bool saved = false;
		{
			std::ofstream stream(path, std::ios::binary);
			{
				cereal::PortableBinaryOutputArchive oarchive(stream); // Create an output archive		
				// ECS::Snapshot(ecs).entities(oarchive);
				// ECS::SerializeComponentsInRegistry(ecs, oarchive);
				oarchive(*this);
				saved = true;
			}
		}

		LINA_TRACE("[Level Exporter] -> Level exported to: {0}", path);
		return saved;
    }


    bool LevelResource::LoadFromFile(const std::string& path, ECS::Registry& ecs)
    {
		// LoadFromFile the level file if exists.
		if (Utility::FileExists(path))
		{
			ecs.clear();
			{
				std::ifstream stream(path, std::ios::binary);
				{
					cereal::PortableBinaryInputArchive iarchive(stream);		
					//ECS::DeserializeComponentsInRegistry(ecs, iarchive, ECS::SnapshotLoader(ecs).entities(iarchive));
					iarchive(*this);
				}
			}
		}
		else
		{
			LINA_WARN("[Level Loader] -> Level does not exist: {0}", path);
			return false;
		}

		LINA_TRACE("[Level Loader] -> Level loaded from file: {0}", path);
		return true;		
    }

}