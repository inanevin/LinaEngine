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
#include "Utility/Log.hpp"
#include <string>

namespace LinaEngine::Graphics
{
	class ArrayBitmap
	{
	public:

		// Param constructors including width, height, pixel array and offsets
		ArrayBitmap(int32 width = 1, int32 height = 1);
		ArrayBitmap(int32 width, int32 height, int32* pixels);
		ArrayBitmap(int32 width, int32 height, int32* pixels, int32 offsetX, int32 offsetY, int32 rowOffset);
		~ArrayBitmap();

		// Load the bitmap from a file in resources.
		int Load(const std::string& fileName);

		// Save the bitmap into a file in resources.
		bool Save(const std::string& fileName) const;

		static void SetImageFlip(bool flip);
		static unsigned char* LoadImmediate(const char* filename, int& w, int& h,  int& nrchannels);
		static float* LoadImmediateHDRI(const char* fileName, int& w, int& h, int& nrChannels);

		// Clr colors.
		void Clear(int32 color);

		// Accessors & Mutators.
		int32 GetWidth() const { return m_width; };
		int32 GetHeight() const { return m_heigth; };

		int32 Get(int32 x, int32 y) const
		{
			LINA_CORE_ASSERT((x > 0 && x < m_width) && (y >= 0 && y < m_heigth), "Conditions are not map on array bitmap !");
			return m_pixels[x + y * m_width];
		};

		void Set(int32 x, int32 y, int32 pixel)
		{
			LINA_CORE_ASSERT((x > 0 && x < m_width) && (y >= 0 && y < m_heigth), "Conditions are not map on array bitmap !");
			m_pixels[x + y * m_width] = pixel;
		};

		int32* GetPixelArray() { return m_pixels; };
		const int32* GetPixelArray() const { return m_pixels; };

	private:

		// Bitmap array properties.
		int32 m_width = 0;
		int32 m_heigth = 0;
		int32* m_pixels = nullptr;

	private:

		uintptr GetPixelsSize() const { return (uintptr)(m_width * m_heigth) * sizeof(m_pixels[0]); }

	};
}


#endif