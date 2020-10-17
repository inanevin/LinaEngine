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
#define STB_IMAGE_IMPLEMENTATION 
#include "Rendering/ArrayBitmap.hpp"  
#include "Utility/stb/stb_image.h"
#include "PackageManager/PAMMemory.hpp"

namespace LinaEngine::Graphics
{
	ArrayBitmap::ArrayBitmap(int32 widthIn, int32 heightIn) : m_width(widthIn), m_heigth(heightIn)
	{
		LINA_CORE_ASSERT(m_width > 0 && m_heigth > 0, " Can not construct bitmap! Width & height must be bigger than 0! Width: {0}, Height: {1} ", m_width, m_heigth);
		m_pixels = (int32*)Memory::malloc(GetPixelsSize());
	}

	ArrayBitmap::ArrayBitmap(int32 widthIn, int32 heightIn, int32* pixelsIn) : m_width(widthIn), m_heigth(heightIn)
	{
		LINA_CORE_ASSERT(m_width > 0 && m_heigth > 0, " Can not construct bitmap! Width & height must be bigger than 0! Width: {0}, Height: {1} ", m_width, m_heigth);
		LINA_CORE_ASSERT(pixelsIn == nullptr, " Can not construct bitmap! Pixels are null");
		uintptr size = GetPixelsSize();
		m_pixels = (int32*)Memory::malloc(size);
		Memory::memcpy(m_pixels, pixelsIn, size);
	}

	ArrayBitmap::ArrayBitmap(int32 widthIn, int32 heightIn, int32* pixelsIn, int32 offsetX, int32 offsetY, int32 rowOffset) : m_width(widthIn), m_heigth(heightIn)
	{
		LINA_CORE_ASSERT(m_width > 0 && m_heigth > 0, " Can not construct bitmap! Width & height must be bigger than 0! Width: {0}, Height: {1} ", m_width, m_heigth);
		LINA_CORE_ASSERT(pixelsIn == nullptr, " Can not construct bitmap! Pixels are null");
		LINA_CORE_ASSERT(offsetX > 0 && offsetY > 0 && rowOffset > 0, "Can not construct bitmap, offsets are not right! X: {0}, Y: {1}, Row: {2}", offsetX, offsetY, rowOffset);

		uintptr size = GetPixelsSize();
		m_pixels = (int32*)Memory::malloc(size);
		int32* pixelsSrc = pixelsIn + offsetY + offsetX * rowOffset;

		for (uintptr i = 0; i < (uintptr)m_heigth;
			++i, m_pixels += m_width, pixelsSrc += rowOffset) {
			Memory::memcpy(m_pixels, pixelsSrc, (uintptr)m_width);
		}
	}

	ArrayBitmap::~ArrayBitmap()
	{
		m_pixels = (int32*)Memory::free(m_pixels);
	}

	int ArrayBitmap::Load(const std::string& fileName)
	{
		int32 texWidth, texHeight, nrComps;

		uint8* data = stbi_load(fileName.c_str(), &texWidth, &texHeight, &nrComps, 4);

		if (data == nullptr) {
			LINA_CORE_ERR("Bitmap with the name {0} could not be loaded!", fileName);
			return -1;
		}

		if (texWidth == m_width && texHeight == m_heigth) {
			Memory::memcpy(m_pixels, data, GetPixelsSize());
		}
		else {
			m_width = texWidth;
			m_heigth = texHeight;
			m_pixels = (int32*)Memory::free(m_pixels);
			m_pixels = (int32*)Memory::malloc(GetPixelsSize());
			Memory::memcpy(m_pixels, data, GetPixelsSize());
		}

		stbi_image_free(data);
		return nrComps;
	}

	bool ArrayBitmap::Save(const std::string& fileName) const
	{
		// TODO: Serialization
		return false;
	}

	void ArrayBitmap::SetImageFlip(bool flip)
	{
		stbi_set_flip_vertically_on_load(flip);
	}

	unsigned char * ArrayBitmap::LoadImmediate(const char * filename, int & w, int & h, int & nrChannels)
	{
		unsigned char *data = stbi_load(filename, &w, &h, &nrChannels, 0);
		return data;
	}

	float* ArrayBitmap::LoadImmediateHDRI(const char* fileName, int& w, int& h, int& nrChannels)
	{
		return stbi_loadf(fileName, &w, &h, &nrChannels, 0);
	}

	void ArrayBitmap::Clear(int32 color)
	{
		Memory::memset(m_pixels, color, GetPixelsSize());
	}

}

