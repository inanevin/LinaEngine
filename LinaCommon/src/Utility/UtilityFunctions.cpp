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

#include "Utility/UtilityFunctions.hpp"
#include "Log/Log.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

#ifdef LINA_PLATFORM_WINDOWS
#include <windows.h>
#include <iostream>
#include <string>
double g_timerFrequency;
bool g_isTimerInitialized = false;
#endif

namespace Lina
{
	namespace Utility
	{
		double GetCPUTime()
		{
#ifdef LINA_WINDOWS
			if (!g_isTimerInitialized)
			{
				LARGE_INTEGER li;
				if (!QueryPerformanceFrequency(&li))
					LINA_ERR("Initialization -> QueryPerformanceFrequency failed!");

				g_timerFrequency = double(li.QuadPart);
				g_isTimerInitialized = true;
			}

			LARGE_INTEGER li;
			if (!QueryPerformanceCounter(&li))
				LINA_ERR("GetTime -> QueryPerformanceCounter failed in get time!");

			return double(li.QuadPart) / g_timerFrequency;
#endif

#ifdef LINA_LINUX
			timespec ts;
			clock_gettime(CLOCK_REALTIME, &ts);
			return (double)(((long)ts.tv_sec * NANOSECONDS_PER_SECOND) + ts.tv_nsec) / ((double)(NANOSECONDS_PER_SECOND));
#endif

#ifdef LINA_UNKNOWN_PLATFORM
			return (double)glfwGetTime();
#endif
		}

		bool FileExists(const std::string& path)
		{
			struct stat buffer;
			return (stat(path.c_str(), &buffer) == 0);
		}

		int GetUniqueID()
		{
			return ++s_uniqueID;
		}

		std::string GetUniqueIDString()
		{
			return std::to_string(++s_uniqueID);
		}

		size_t StringToHash(const std::string& str)
		{
			std::hash<std::string> hasher;
			return hasher(str);
		}

		std::vector<std::string> Split(const std::string& s, char delim)
		{
			std::vector<std::string> elems;

			const char* cstr = s.c_str();
			unsigned int strLength = (unsigned int)s.length();
			unsigned int start = 0;
			unsigned int end = 0;

			while (end <= strLength)
			{
				while (end <= strLength)
				{
					if (cstr[end] == delim)
						break;

					end++;
				}

				elems.push_back(s.substr(start, end - start));
				start = end + 1;
				end = start;
			}

			return elems;
		}

		std::string GetFilePath(const std::string& fileName)
		{
			const char* cstr = fileName.c_str();
			unsigned int strLength = (unsigned int)fileName.length();
			unsigned int end = strLength - 1;

			while (end != 0)
			{
				if (cstr[end] == '/')
					break;

				end--;
			}

			if (end == 0)
				return fileName;

			else
			{
				unsigned int start = 0;
				end = end + 1;
				return fileName.substr(start, end - start);
			}
		}
		
		std::string GetFileExtension(const std::string& file)
		{
			return file.substr(file.find_last_of(".") + 1);
		}
		std::string GetFileNameOnly(const std::string& fileName)
		{
			return fileName.substr(fileName.find_last_of("/\\") + 1);
		}

		std::string GetFileWithoutExtension(const std::string& fileName)
		{
			size_t lastindex = fileName.find_last_of(".");
			return fileName.substr(0, lastindex);
		}
		bool LoadTextFileWithIncludes(std::string& output, const std::string& fileName, const std::string& includeKeyword)
		{
			std::ifstream file;
			file.open(fileName.c_str());

			std::string filePath = GetFilePath(fileName);
			std::stringstream ss;
			std::string line;

			if (file.is_open())
			{
				while (file.good())
				{
					getline(file, line);

					if (line.find(includeKeyword) == std::string::npos)
						ss << line << "\n";

					else
					{
						std::string includeFileName = Split(line, ' ')[1];
						includeFileName =
							includeFileName.substr(1, includeFileName.length() - 2);

						std::string toAppend;
						LoadTextFileWithIncludes(toAppend, filePath + includeFileName,
							includeKeyword);
						ss << toAppend << "\n";
					}
				}
			}
			else
			{
				LINA_ERR("File could not be loaded! {0}", fileName);
				return false;
			}

			output = ss.str();
			return true;
		}

		std::string GetRunningDirectory()
		{
#ifdef LINA_PLATFORM_WINDOWS

			TCHAR buffer[MAX_PATH] = { 0 };
			GetModuleFileName(NULL, buffer, MAX_PATH);
			std::string exeFilename = std::string(buffer);
			std::string runningDirectory = exeFilename.substr(0, exeFilename.find_last_of("\\/"));
			return runningDirectory;
#endif

			return "";
		}
	}
}