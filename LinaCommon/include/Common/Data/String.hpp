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

#ifndef DataStructuresString_HPP
#define DataStructuresString_HPP

#include <EASTL/string.h>
#include "Common/Data/Vector.hpp"

#ifdef LINA_DEBUG
#define FMT_HEADER_ONLY
#include <fmt/format.h>
#endif

namespace Lina
{
	typedef eastl::string  String;
	typedef eastl::wstring WString;

#define TO_STRING(...) eastl::to_string(__VA_ARGS__)

	class UtilStr
	{
	public:
		static String  WideStringToString(const WString& wstring);
		static WString StringToWString(const String& string);
		static String  EncodeUTF8(wchar_t wch);
		static String  RemoveAllDotsExceptFirst(const String& str);
		static String  FixStringNumber(const String& str);
		static String  FloatToString(float val, int decimals);
		static float   StringToFloat(const String& str, int& decimals);
		static int	   StringToInt(const String& str);
		static String  GetUntilFirstOf(const String& str);

		/// <summary>
		/// Returns a list of strings that fall in between each of the split string.
		/// e.g. given a filesystem path will return a list of all directories in the path and the last file if there is one
		/// </summary>
		static Vector<String> SplitBy(const String& str, const String& splitStr);
	};

} // namespace Lina

template <> struct std::hash<eastl::string>
{
	std::size_t operator()(eastl::string const& s) const noexcept
	{
		return eastl::hash<eastl::string>()(s);
	}
};

#ifdef LINA_DEBUG

template <> struct fmt::formatter<eastl::string>
{
	// Presentation format: 'f' - fixed, 'e' - exponential.
	char presentation = 'f';

	// Parses format specifications of the form ['f' | 'e'].
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
	{
		// [ctx.begin(), ctx.end()) is a character range that contains a part of
		// the format string starting from the format specifications to be parsed,
		// e.g. in
		//
		//   fmt::format("{:f} - point of interest", point{1, 2});
		//
		// the range will contain "f} - point of interest". The formatter should
		// parse specifiers until '}' or the end of the range. In this example
		// the formatter should parse the 'f' specifier and return an iterator
		// pointing to '}'.

		// Please also note that this character range may be empty, in case of
		// the "{}" format string, so therefore you should check ctx.begin()
		// for equality with ctx.end().

		// Parse the presentation format and store it in the formatter:
		auto it = ctx.begin(), end = ctx.end();
		if (it != end && (*it == 'f' || *it == 'e'))
			presentation = *it++;

		// Check if reached the end of the range:
		if (it != end && *it != '}')
			throw format_error("invalid format");

		// Return an iterator past the end of the parsed range:
		return it;
	}

	// Formats the point p using the parsed format specification (presentation)
	// stored in this formatter.
	template <typename FormatContext> auto format(const eastl::string& str, FormatContext& ctx) const -> decltype(ctx.out())
	{
		// ctx.out() is an output iterator to write to.
		return fmt::format_to(ctx.out(), str.c_str());
	}
};

#endif

#endif