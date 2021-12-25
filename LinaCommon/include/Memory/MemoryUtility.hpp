/*
This file is a part of: Lina Engine
https://github.com/inanevin/Lina

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

Custom Memory Allocators: Copyright (c) 2016 Mariano Trebino

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
Class: MemoryUtils

Various utility methods used by memory allocators.

Timestamp: 12/19/2020 1:32:31 AM
*/

#pragma once

#ifndef MemoryUtils_HPP
#define MemoryUtils_HPP

// Headers here.
#include <cstddef>

namespace Lina
{
	class MemoryUtility
	{

	public:

		static const std::size_t CalculatePadding(const std::size_t baseAddress, const std::size_t alignment)
		{
			const std::size_t multiplier = (baseAddress / alignment) + 1;
			const std::size_t alignedAddress = multiplier * alignment;
			const std::size_t padding = alignedAddress - baseAddress;
			return padding;
		}

		static const std::size_t CalculatePaddingWithHeader(const std::size_t baseAddress, const std::size_t alignment, const std::size_t headerSize)
		{
			std::size_t padding = CalculatePadding(baseAddress, alignment);
			std::size_t neededSpace = headerSize;

			if (padding < neededSpace)
			{
				// Header does not fit - Calculate next aligned address that header fits
				neededSpace -= padding;

				// How many alignments I need to fit the header        
				if (neededSpace % alignment > 0)
					padding += alignment * (1 + (neededSpace / alignment));
				else
					padding += alignment * (neededSpace / alignment);

			}
			return padding;
		}

	};
}

#endif
