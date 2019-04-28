/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: Internal
Timestamp: 4/17/2019 1:49:50 AM

*/

#pragma once

#ifndef Internal_HPP
#define Internal_HPP

#include "DataStructures.hpp"

namespace LinaEngine
{
	namespace Internal
	{
		template <typename T> struct comparison_traits {
			static bool equal(const T& a, const T& b) {
				return a == b;
			}
		};

		static size_t StringToHash(LinaString str)
		{
			LinaHash<LinaString> hasher;
			return hasher(str);
		}

		template<typename T>
		static inline LinaString ToString(T val)
		{
			std::ostringstream convert;
			convert << val;
			return convert.str();
		}

		static FORCEINLINE LinaArray<LinaString> split(const LinaString& s, char delim)
		{
			LinaArray<LinaString> elems;

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

		static FORCEINLINE LinaString getFilePath(const LinaString& fileName)
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

		static FORCEINLINE bool loadTextFileWithIncludes(LinaString& output, const LinaString& fileName, const LinaString& includeKeyword)
		{
			std::ifstream file;
			file.open(fileName.c_str());

			LinaString filePath = getFilePath(fileName);
			std::stringstream ss;
			LinaString line;

			if (file.is_open()) {
				while (file.good()) {
					getline(file, line);

					if (line.find(includeKeyword) == LinaString::npos) {
						ss << line << "\n";
					}
					else {
						LinaString includeFileName = split(line, ' ')[1];
						includeFileName =
							includeFileName.substr(1, includeFileName.length() - 2);

						LinaString toAppend;
						loadTextFileWithIncludes(toAppend, filePath + includeFileName,
							includeKeyword);
						ss << toAppend << "\n";
					}
				}
			}
			else {
				LINA_CORE_ERR("Unable to load file: {0}", fileName.c_str());
				return false;
			}

			output = ss.str();
			return true;
		}

		template <typename E>
		constexpr auto to_underlying(E e) noexcept
		{
			return static_cast<std::underlying_type_t<E>>(e);
		}

	}
}

#endif