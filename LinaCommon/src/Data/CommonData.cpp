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

#include "Common/Data/CommonData.hpp"

namespace Lina
{
	namespace Internal
	{

		extern Vector<String> SplitIntoUTF8Chars(const String& str)
		{
			Vector<String>	  chars;
			String::size_type i = 0;
			while (i < str.size())
			{
				unsigned char	  c	   = str[i];
				String::size_type size = 1;
				if (c >= 0xF0)
					size = 4; // first byte of 4-byte sequence
				else if (c >= 0xE0)
					size = 3; // first byte of 3-byte sequence
				else if (c >= 0xC0)
					size = 2; // first byte of 2-byte sequence
				// for 0x00-0x7F size is 1 (ASCII range)

				chars.push_back(str.substr(i, size));
				i += size;
			}
			return chars;
		}

	} // namespace Internal
} // namespace Lina
