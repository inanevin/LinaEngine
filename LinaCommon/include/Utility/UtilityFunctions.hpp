/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: UtilityFunctions
Timestamp: 1/5/2019 12:42:58 AM

*/

#ifndef UtilityFunctions_HPP
#define UtilityFunctions_HPP

#include <string>
#include <vector>

namespace LinaEngine
{
	namespace Utility
	{
		// Create a unique hash from string.
		size_t StringToHash(std::string str);

		// Split a string vector.
		std::vector<std::string> split(const std::string& s, char delim);

		// Return path via filename.
		std::string getFilePath(const std::string& fileName);

		// Loads a text file, parsing through include keywords.
		bool LoadTextFileWithIncludes(std::string& output, const std::string& fileName, const std::string& includeKeyword);
	}
}

#endif