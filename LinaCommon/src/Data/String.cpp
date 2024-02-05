/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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

#include "Common/Data/String.hpp"
#include <string>
#include <codecvt>
#include <locale>
#include <iostream>
#include <cwchar>
#include <cstring>

#ifdef LINA_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable : 4996)
#pragma warning(disable : 4333)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

namespace Lina
{

	String UtilStr::WideStringToString(const WString& wstring)
	{
		std::wstring											  wstr = wstring.c_str();
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
		auto													  converted = converter.to_bytes(wstr);
		return converted.c_str();
	}

	WString UtilStr::StringToWString(const String& string)
	{
		std::string												  str = string.c_str();
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
		auto													  converted = converter.from_bytes(str);
		return converted.c_str();
	}

	String UtilStr::EncodeUTF8(wchar_t ch)
	{
		String utf8str;
		if (ch < 0x80)
		{
			utf8str.push_back(static_cast<char>(ch));
		}
		else if (ch < 0x800)
		{
			utf8str.push_back(static_cast<char>(0xC0 | (ch >> 6)));
			utf8str.push_back(static_cast<char>(0x80 | (ch & 0x3F)));
		}
		else if (ch < 0x10000)
		{
			utf8str.push_back(static_cast<char>(0xE0 | (ch >> 12)));
			utf8str.push_back(static_cast<char>(0x80 | ((ch >> 6) & 0x3F)));
			utf8str.push_back(static_cast<char>(0x80 | (ch & 0x3F)));
		}
		else
		{
			utf8str.push_back(static_cast<char>(0xF0 | (ch >> 18)));
			utf8str.push_back(static_cast<char>(0x80 | ((ch >> 12) & 0x3F)));
			utf8str.push_back(static_cast<char>(0x80 | ((ch >> 6) & 0x3F)));
			utf8str.push_back(static_cast<char>(0x80 | (ch & 0x3F)));
		}
		return utf8str;
	}

	float UtilStr::StringToFloat(const String& str, int& decimals)
	{
		try
		{
			std::size_t pos = str.find('.');
			if (pos != std::string::npos)
				decimals = static_cast<int>(str.length() - pos - 1);

			return std::stof(str.c_str());
		}
		catch (const std::exception& e)
		{
			LINA_ERR("Exception: StringToFloat() string: {0} - decimals: {1}", str, decimals);
			return 0.0f;
		}
	}

	int UtilStr::StringToInt(const String& str)
	{
		try
		{
			return std::stoi(str.c_str());
		}
		catch (const std::exception& e)
		{
			LINA_ERR("Exception: StringToInt() string: {0}", str);
			return 0;
		}
	}

	String UtilStr::RemoveAllDotsExceptFirst(const String& str)
	{
		String		result = str;
		std::size_t pos	   = result.find('.'); // find the first dot

		// if there is a dot in the string
		if (pos != std::string::npos)
		{
			// remove all subsequent dots
			pos++; // start from the next character
			std::size_t next;
			while ((next = result.find('.', pos)) != std::string::npos)
			{
				result.erase(next, 1); // erase the dot
			}
		}

		return result;
	}

	String UtilStr::FixStringNumber(const String& str)
	{
		String copy = str;

		std::size_t dot = copy.find(".");

		if (dot != std::string::npos)
		{
			copy = RemoveAllDotsExceptFirst(copy);

			const int dotPos = static_cast<int>(dot);
			const int sz	 = static_cast<int>(copy.length());

			// If nothing exists after dot, insert 0.
			if (dotPos == sz - 1)
				copy.insert(dot + 1, "0");

			// If nothing exists before dot insert 0.
			if (dotPos == 0)
				copy.insert(0, "0");
		}

		return copy;
	}

	String UtilStr::FloatToString(float val, int decimals)
	{
		std::ostringstream out;
		out << std::fixed << std::setprecision(decimals) << val;
		const String str = out.str().c_str();
		return str;
	}

	String UtilStr::GetUntilFirstOf(const String& str)
	{
		const size_t pos = str.find_first_of(str);

		if (pos == String::npos)
			return "";

		return str.substr(0, str.find_first_of(str));
	}

	Vector<String> UtilStr::SplitBy(const String& str, const String& splitStr)
	{
		// Split the path into directories
		Vector<String> directories;
		size_t		   start = 0, end = str.find(splitStr.c_str());
		while (end != String::npos)
		{
			const auto aq = str.substr(start, end - start);
			directories.push_back(aq);
			start = end + splitStr.size();
			end	  = str.find(splitStr.c_str(), start);
		}
		directories.push_back(str.substr(start));
		return directories;
	}

} // namespace Lina

#ifdef LINA_COMPILER_MSVC
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif
