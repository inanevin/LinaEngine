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
Class: LinaArray

Alternative std::vector wrapper.

Timestamp: 12/29/2018 10:43:46 PM

*/

#pragma once
#ifndef LinaArray_HPP
#define LinaArray_HPP

#include <vector>

namespace Lina
{
	// Custom array w/ extension functions defined.
	template<typename T>
	class LinaArray : public std::vector<T>
	{
	public:
		LinaArray<T>() : std::vector<T>() {}
		LinaArray<T>(size_t n) : std::vector<T>(n) {}

		// Swap the element to the end of this vector & pop it out.
		inline void swap_remove(size_t index)
		{
			std::swap((*this)[index], (*this)[this->size() - 1]);
			this->pop_back();
		}
	};
}

#endif