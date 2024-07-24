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

#include <vector>

namespace Lina
{
	template <typename T> using Vector = std::vector<T>;

	namespace UtilVector
	{
		template <typename T> inline int32 IndexOf(const Lina::Vector<T>& vec, const T& value)
		{
			const int32 sz = static_cast<int32>(vec.size());

			for (int32 i = 0; i < sz; ++i)
			{
				if (vec[i] == value)
					return i;
			}

			return -1;
		}

		template <typename T> inline int32 FindNextIndexIfItemIsRemoved(const Lina::Vector<T>& vec, const T& value)
		{
			const int32 currentIndex = IndexOf(vec, value);

			if (currentIndex == 0)
			{
				if (vec.size() == 1)
					return -1;

				return 0;
			}

			return currentIndex - 1;
		}

		template <typename T> inline void PlaceAfter(Lina::Vector<T>& vec, T& src, T& target)
		{
			auto itSrc	  = std::find_if(vec.begin(), vec.end(), [src](const T& child) { return child == src; });
			auto itTarget = std::find_if(vec.begin(), vec.end(), [target](const T& child) { return child == target; });
			vec.erase(itSrc);
			vec.insert(itTarget + 1, *itSrc);
		}

		template <typename T> inline void PlaceBefore(Lina::Vector<T>& vec, const T& src, const T& target)
		{
			auto itSrc	  = std::find_if(vec.begin(), vec.end(), [src](const T& child) { return child == src; });
			auto itTarget = std::find_if(vec.begin(), vec.end(), [target](const T& child) { return child == target; });

			vec.insert(itTarget, *itSrc);
			if (itSrc < itTarget)
				vec.erase(itSrc);
			else
				vec.erase(itSrc + 1);
		}

		template <typename T> inline bool Contains(const Lina::Vector<T>& vec, const T& data)
		{
			for (const auto& v : vec)
			{
				if (v == data)
					return true;
			}

			return false;
		}

	} // namespace UtilVector

} // namespace Lina
