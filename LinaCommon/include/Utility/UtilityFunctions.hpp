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

/*
Class: UtilityFunctions

Various utility functions for general purpose use.

Timestamp: 1/5/2019 12:42:58 AM
*/

#ifndef UtilityFunctions_HPP
#define UtilityFunctions_HPP

#include <string>
#include <vector>
#include <functional>

namespace Lina
{
	namespace Utility
	{
		static int s_uniqueID = 100;

		// Creates a GUID.
		int GetUniqueID();

		bool FileExists(const std::string& path);

		// Converts the current GUID to string via to_string and simply returns that.
		std::string GetUniqueIDString();

		size_t StringToHash(const std::string& str);

		std::vector<std::string> Split(const std::string& s, char delim);

		std::string GetFilePath(const std::string& fileName);
		std::string GetFileWithoutExtension(const std::string& filename);
		std::string GetFileNameOnly(const std::string& filename);
		std::string GetFileExtension(const std::string& file);

		// Mostly used for loading shaders.
		bool LoadTextFileWithIncludes(std::string& output, const std::string& fileName, const std::string& includeKeyword);

		std::string GetRunningDirectory();
	}
}

#endif