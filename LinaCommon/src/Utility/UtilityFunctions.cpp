#include "..\..\include\Utility\UtilityFunctions.hpp"
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
Timestamp: 1/5/2019 1:39:45 PM

*/


#include <fstream>
#include <iostream>
#include <sstream>

namespace LinaEngine
{
	namespace Utility
	{
		size_t StringToHash(std::string str)
		{
			std::hash<std::string> hasher;
			return hasher(str);
		}

		std::vector<std::string> split(const std::string& s, char delim)
		{
			std::vector<std::string> elems;

			const char* cstr = s.c_str();
			unsigned int strLength = (unsigned int)s.length();
			unsigned int start = 0;
			unsigned int end = 0;

			while (end <= strLength) {
				while (end <= strLength) {
					if (cstr[end] == delim) {
						break;
					}
					end++;
				}

				elems.push_back(s.substr(start, end - start));
				start = end + 1;
				end = start;
			}

			return elems;
		}
		std::string getFilePath(const std::string& fileName)
		{
			const char* cstr = fileName.c_str();
			unsigned int strLength = (unsigned int)fileName.length();
			unsigned int end = strLength - 1;

			while (end != 0) {
				if (cstr[end] == '/') {
					break;
				}
				end--;
			}

			if (end == 0) {
				return fileName;
			}
			else {
				unsigned int start = 0;
				end = end + 1;
				return fileName.substr(start, end - start);
			}
		}
		bool LoadTextFileWithIncludes(std::string& output, const std::string& fileName, const std::string& includeKeyword)
		{
			std::ifstream file;
			file.open(fileName.c_str());

			std::string filePath = getFilePath(fileName);
			std::stringstream ss;
			std::string line;

			if (file.is_open()) {
				while (file.good()) {
					getline(file, line);

					if (line.find(includeKeyword) == std::string::npos) {
						ss << line << "\n";
					}
					else {
						std::string includeFileName = split(line, ' ')[1];
						includeFileName =
							includeFileName.substr(1, includeFileName.length() - 2);

						std::string toAppend;
						LoadTextFileWithIncludes(toAppend, filePath + includeFileName,
							includeKeyword);
						ss << toAppend << "\n";
					}
				}
			}
			else {
				std::cout << "Unable to read file!" << std::endl;
				return false;
			}

			output = ss.str();
			return true;
		}
	}
}