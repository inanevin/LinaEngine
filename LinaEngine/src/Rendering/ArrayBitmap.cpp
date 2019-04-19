/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: ArrayBitmap
Timestamp: 4/14/2019 11:59:32 AM

*/

#include "LinaPch.hpp"
#include "Rendering/ArrayBitmap.hpp"  
#include "stb_image.h"

namespace LinaEngine::Graphics
{
	ArrayBitmap::ArrayBitmap(int32 widthIn, int32 heightIn) : m_Width(widthIn), m_Heigth(heightIn)
	{
		LINA_CORE_ASSERT(m_Width > 0 && m_Heigth > 0, " Can not construct bitmap! Width & height must be bigger than 0! Width: {0}, Height: {1} ", m_Width, m_Heigth);
		m_Pixels = (int32*)Memory::malloc(GetPixelsSize());
	}

	ArrayBitmap::ArrayBitmap(int32 widthIn, int32 heightIn, int32* pixelsIn) : m_Width(widthIn), m_Heigth(heightIn)
	{
		LINA_CORE_ASSERT(m_Width > 0 && m_Heigth > 0, " Can not construct bitmap! Width & height must be bigger than 0! Width: {0}, Height: {1} ", m_Width, m_Heigth);
		LINA_CORE_ASSERT(pixelsIn == nullptr, " Can not construct bitmap! Pixels are null");
		uintptr size = GetPixelsSize();
		m_Pixels = (int32*)Memory::malloc(size);
		Memory::memcpy(m_Pixels, pixelsIn, size);
	}

	ArrayBitmap::ArrayBitmap(int32 widthIn, int32 heightIn, int32* pixelsIn, int32 offsetX, int32 offsetY, int32 rowOffset) : m_Width(widthIn), m_Heigth(heightIn)
	{
		LINA_CORE_ASSERT(m_Width > 0 && m_Heigth > 0, " Can not construct bitmap! Width & height must be bigger than 0! Width: {0}, Height: {1} ", m_Width, m_Heigth);
		LINA_CORE_ASSERT(pixelsIn == nullptr, " Can not construct bitmap! Pixels are null");
		LINA_CORE_ASSERT(offsetX > 0 && offsetY > 0 && rowOffset > 0, "Can not construct bitmap, offsets are not right! X: {0}, Y: {1}, Row: {2}", offsetX, offsetY, rowOffset);

		uintptr size = GetPixelsSize();
		m_Pixels = (int32*)Memory::malloc(size);
		int32* pixelsSrc = pixelsIn + offsetY + offsetX * rowOffset;

		for (uintptr i = 0; i < (uintptr)m_Heigth;
			++i, m_Pixels += m_Width, pixelsSrc += rowOffset) {
			Memory::memcpy(m_Pixels, pixelsSrc, (uintptr)m_Width);
		}
	}

	ArrayBitmap::~ArrayBitmap()
	{
		m_Pixels = (int32*)Memory::free(m_Pixels);
	}

	bool ArrayBitmap::Load(const LinaString& fileName)
	{
		int32 texWidth, texHeight, bytesPerPixel;

		uint8* data = stbi_load(fileName.c_str(), &texWidth, &texHeight,
			&bytesPerPixel, 4);

		if (data == nullptr) {
			return false;
		}

		if (texWidth == m_Width && texHeight == m_Heigth) {
			Memory::memcpy(m_Pixels, data, GetPixelsSize());
		}
		else {
			m_Width = texWidth;
			m_Heigth = texHeight;
			m_Pixels = (int32*)Memory::free(m_Pixels);
			m_Pixels = (int32*)Memory::malloc(GetPixelsSize());
			Memory::memcpy(m_Pixels, data, GetPixelsSize());
		}

		stbi_image_free(data);
		return true;
	}

	bool ArrayBitmap::Save(const LinaString& fileName) const
	{
		// TODO: Serialization
		return false;
	}

	void ArrayBitmap::Clear(int32 color)
	{
		Memory::memset(m_Pixels, color, GetPixelsSize());
	}
}

