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
Class: ArrayBitmap

Used for loading textures, this class is an intermediate step to hold bitmap data
loaded from the image files. After the data is loaded into memory and given to
GPU backend, its discarded.

Timestamp: 4/14/2019 11:59:32 AM
*/

#pragma once

#ifndef ArrayBitmap_HPP
#define ArrayBitmap_HPP

#include "Core/SizeDefinitions.hpp"
#include "Log/Log.hpp"
#include <string>

namespace Lina::Graphics
{
	class ArrayBitmap
	{
	public:

		// Param constructors including width, height, pixel array and offsets
		~ArrayBitmap();

		// Load the bitmap from a file in resources.
		int Load(const std::string& fileName);

		// Load the bätmap from memory-
		int Load(unsigned char* data, size_t dataSize);

		static void SetImageFlip(bool flip);
		static unsigned char* LoadImmediate(const char* filename, int& w, int& h,  int& nrchannels);
		static float* LoadImmediateHDRI(const char* fileName, int& w, int& h, int& nrChannels);
		static float* LoadImmediateHDRI(unsigned char* data, size_t dataSize, int& w, int& h, int& nrChannels);
		static bool Free(unsigned char* data);

		// Clr colors.
		void Clear(int32 color);

		// Accessors & Mutators.
		int32 GetWidth() const { return m_width; };
		int32 GetHeight() const { return m_height; };

		unsigned char* GetPixelArray() { return m_pixels; }
		const unsigned char* GetPixelArray() const { return m_pixels; }

	private:
		int32 m_width = 0;
		int32 m_height = 0;
		unsigned char* m_pixels = nullptr;

	};
}


#endif