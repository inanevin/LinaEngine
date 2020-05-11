/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: DDSTexture
Timestamp: 4/14/2019 4:04:34 PM

*/

#include "LinaPch.hpp"
#include "Rendering/DDSTexture.hpp"  
#include "PackageManager/PAMMemory.hpp"
#include <fileapi.h>

#ifdef LINA_PLATFORM_WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#endif

namespace LinaEngine::Graphics
{

	bool DDSTexture::Load(const char * fileName)
	{
		unsigned char header[124];
		FILE* fp;
		fopen_s(&fp, fileName, "rb");

		if (fp == NULL)  return false;	

		// Verify valid DDS file
		char filecode[4];
		fread(filecode, 1, 4, fp);
		if (strncmp(filecode, "DDS ", 4) != 0)
		{
			fclose(fp);
			return false;
		}

		// Read header
		fread(&header, ARRAY_SIZE_IN_ELEMENTS(header), 1, fp);
		m_Height = *(uint32*)&(header[8]);
		m_Width = *(uint32*)&(header[12]);
		uint32 linearSize = *(uint32*)&(header[16]);
		m_MipMapCount = *(uint32*)&(header[24]);
		m_FourCC = *(uint32*)&(header[80]);

		// Allocate memory for DDS file
		uint32 bufsize = m_MipMapCount > 1 ? linearSize * 2 : linearSize;
		CleanUp();
		m_Buffer = (unsigned char*)Memory::malloc(bufsize * sizeof(unsigned char));
		fread(m_Buffer, 1, bufsize, fp);
		fclose(fp);

		return true;
	}

	void DDSTexture::CleanUp()
	{
		if (m_Buffer != nullptr) 
			Memory::free(m_Buffer);
		
		m_Buffer = nullptr;
	}
}

