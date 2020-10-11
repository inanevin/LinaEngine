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

Class: EditorUtility
Timestamp: 5/9/2020 1:23:05 AM

*/


#include "Utility/EditorUtility.hpp"
#include "Rendering/Material.hpp"
//#include "boost/filesystem.hpp"
#include <filesystem>

#include <cereal/archives/binary.hpp>
#include <fstream>

namespace LinaEditor
{
	bool EditorUtility::CreateFolderInPath(const std::string& path)
	{
		return std::filesystem::create_directory(path);
	}

	bool EditorUtility::GetDirectories(std::vector<std::string>& vec, const std::string& path)
	{

		return false;
	}

	bool EditorUtility::DeleteDirectory(const std::string& path)
	{
		return false;
	}

	void EditorUtility::SerializeMaterial(const std::string& path, LinaEngine::Graphics::Material& mat)
	{
		std::ofstream os(path);
		{
			cereal::BinaryOutputArchive oarchive(os); // Create an output archive
			oarchive(mat); // Write the data to the archive
		}
	}

}