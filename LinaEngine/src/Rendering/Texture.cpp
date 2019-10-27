/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Texture
Timestamp: 1/7/2019 1:55:47 PM

*/

#include "LinaPch.hpp"
#include "Rendering/Texture.hpp"  
#include "Rendering/ArrayBitmap.hpp"
#include "Rendering/DDSTexture.hpp"

namespace LinaEngine::Graphics
{
	Texture::~Texture()
	{
		m_ID = renderDevice->ReleaseTexture2D(m_ID);
	}

	void Texture::Construct(RenderDevice & deviceIn, const ArrayBitmap & data, PixelFormat internalPixelFormat, bool generateMipMaps, bool shouldCompress)
	{
		renderDevice = &deviceIn;
		m_ID = renderDevice->CreateTexture2D(data.GetWidth(), data.GetHeight(), data.GetPixelArray(), PixelFormat::FORMAT_RGBA, internalPixelFormat, generateMipMaps, shouldCompress);
		m_Width = (uint32)data.GetWidth();
		m_Height = (uint32)data.GetHeight();
		isCompressed = shouldCompress;
		hasMipMaps = generateMipMaps;
	}

	void Texture::Construct(RenderDevice & deviceIn, const DDSTexture & ddsTexture)
	{
		renderDevice = &deviceIn;
		m_ID = renderDevice->CreateDDSTexture2D(ddsTexture.GetWidth(), ddsTexture.GetHeight(), ddsTexture.GetBuffer(), ddsTexture.GetFourCC(), ddsTexture.GetMipMapCount());
		m_Width = ddsTexture.GetWidth();
		m_Height = ddsTexture.GetHeight();
		isCompressed = true;
		hasMipMaps = ddsTexture.GetMipMapCount() > 1;
	}

	void Texture::Construct(RenderDevice & deviceIn, const LinaArray<ArrayBitmap*>& data, PixelFormat internalPixelFormat, bool generateMipMaps, bool shouldCompress)
	{
		if (data.size() != 6)
		{
			LINA_CORE_WARN("Could not construct cubemap texture! ArrayBitmap data size needs to be 6, aborting!");
			return;
		}

		renderDevice = &deviceIn;
		m_Width = (uint32)data[0]->GetWidth();
		m_Height = (uint32)data[0]->GetHeight();

		LinaArray<int32*> cubeMapData;

		for (uint32 i = 0; i < 6; i++)
			cubeMapData.push_back(data[i]->GetPixelArray());
		
		m_ID = renderDevice->CreateCubemapTexture(m_Width, m_Height, cubeMapData);
		isCompressed = shouldCompress;
		hasMipMaps = generateMipMaps;

		cubeMapData.clear();

	}


}

