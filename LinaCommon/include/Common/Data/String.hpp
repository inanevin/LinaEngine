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

#pragma once

#include <string>
#include "Common/Data/Vector.hpp"

namespace Lina
{
	typedef std::string	 String;
	typedef std::wstring WString;

#define TO_STRING(...) std::to_string(__VA_ARGS__)

	class UtilStr
	{
	public:
		static String		  WideStringToString(const WString& wstring);
		static WString		  StringToWString(const String& string);
		static String		  WCharToString(wchar_t wch);
		static String		  EncodeUTF8(wchar_t wch);
		static String		  RemoveAllDotsExceptFirst(const String& str);
		static String		  FixStringNumber(const String& str);
		static String		  FloatToString(float val, uint32 decimals);
		static String		  ReplaceAll(const String& str, const String& toReplace, const String& replacement);
		static float		  StringToFloat(const String& str, uint32& outDecimals);
		static int			  StringToInt(const String& str);
		static uint64		  StringToBigInt(const String& str);
		static String		  GetUntilFirstOf(const String& str);
		static char*		  WCharToChar(const wchar_t* wch);
		static const wchar_t* CharToWChar(const char* ch);
		static String		  ToUpper(const String& str);
		static String		  ToLower(const String& str);
		static void			  SeperateByChar(const String& str, Vector<String>& out, char c);
		static String		  SizeBytesToString(size_t sz, uint32 decimals);
		static String		  RemoveWhitespaces(const String& str);

		/// <summary>
		/// Returns a list of strings that fall in between each of the split string.
		/// e.g. given a filesystem path will return a list of all directories in the path and the last file if there is one
		/// </summary>
		static Vector<String> SplitBy(const String& str, const String& splitStr);
	};

} // namespace Lina
