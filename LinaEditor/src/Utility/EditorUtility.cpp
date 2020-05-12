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
#include "boost/filesystem.hpp"
#include <filesystem>


namespace LinaEditor::Utility
{
	bool EditorUtility::CreateFolderInPath(const std::string& path)
	{
		std::string newPath = path;
		int i = 0;
		const int counterLimit = 50;

		// Check if file exists, append number if so.
		while (boost::filesystem::exists(newPath))
		{	
			newPath = path + " [" + std::to_string(i) + "]";
			i++;

			if (i == counterLimit)
			{
				return false;
			}
		}
		
		// Create file.
		return boost::filesystem::create_directories(newPath);
	}

	bool EditorUtility::GetDirectories(std::vector<std::string>& vec, const std::string& path)
	{
		// Get all folders in path.
		std::vector<std::string> r;
		for (auto& p : std::filesystem::recursive_directory_iterator(path))
			if (p.is_directory())
				r.push_back(p.path().filename().string());
		vec = r;
		return true;
	}
}