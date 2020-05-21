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

#include "Rendering/Texture.hpp"  
#include "Rendering/ArrayBitmap.hpp"
#include "Rendering/DDSTexture.hpp"

namespace LinaEngine::Graphics
{
	Texture::~Texture()
	{
		m_ID = renderDevice->ReleaseTexture2D(m_ID);

		if (dataAssigned)
			delete data;
	}

	Texture& Texture::Construct(RenderDevice& deviceIn, const ArrayBitmap& data, PixelFormat pixelFormat, PixelFormat internalPixelFormat, bool generateMipMaps, bool shouldCompress, SamplerData samplerData)
	{
		renderDevice = &deviceIn;
		m_Sampler.Construct(deviceIn, samplerData.minFilter, samplerData.maxFilter, samplerData.wrapS, samplerData.wrapT, samplerData.anisotropy);
		m_ID = renderDevice->CreateTexture2D(data.GetWidth(), data.GetHeight(), data.GetPixelArray(), pixelFormat, internalPixelFormat, generateMipMaps, shouldCompress, samplerData.minFilter, samplerData.maxFilter, samplerData.wrapS, samplerData.wrapT);
		m_Width = (uint32)data.GetWidth();
		m_Height = (uint32)data.GetHeight();
		isCompressed = shouldCompress;
		hasMipMaps = generateMipMaps;
		return *this;
	}

	Texture& Texture::Construct(RenderDevice& deviceIn, const DDSTexture& ddsTexture, SamplerData samplerData)
	{
		renderDevice = &deviceIn;
		m_Sampler.Construct(deviceIn, samplerData.minFilter, samplerData.maxFilter, samplerData.wrapS, samplerData.wrapT, samplerData.anisotropy);
		m_ID = renderDevice->CreateDDSTexture2D(ddsTexture.GetWidth(), ddsTexture.GetHeight(), ddsTexture.GetBuffer(), ddsTexture.GetFourCC(), ddsTexture.GetMipMapCount());
		m_Width = ddsTexture.GetWidth();
		m_Height = ddsTexture.GetHeight();
		isCompressed = true;
		hasMipMaps = ddsTexture.GetMipMapCount() > 1;
		return *this;
	}

	Texture& Texture::Construct(RenderDevice& deviceIn, const LinaArray<ArrayBitmap*>& data, PixelFormat pixelFormat, PixelFormat internalPixelFormat, bool generateMipMaps, bool shouldCompress, SamplerData samplerData)
	{
		if (data.size() != 6)
		{
			LINA_CORE_WARN("Could not construct cubemap texture! ArrayBitmap data size needs to be 6, returning un-constructed texture...");
			return Texture();
		}

		m_Sampler.Construct(deviceIn, samplerData.minFilter, samplerData.maxFilter, samplerData.wrapS, samplerData.wrapT, samplerData.anisotropy);
		renderDevice = &deviceIn;
		m_Width = (uint32)data[0]->GetWidth();
		m_Height = (uint32)data[0]->GetHeight();

		LinaArray<int32*> cubeMapData;

		for (uint32 i = 0; i < 6; i++)
			cubeMapData.push_back(data[i]->GetPixelArray());

		m_ID = renderDevice->CreateCubemapTexture(m_Width, m_Height, cubeMapData, 6U, internalPixelFormat, internalPixelFormat, generateMipMaps);
		isCompressed = shouldCompress;
		hasMipMaps = generateMipMaps;

		cubeMapData.clear();
		return *this;
	}

	Texture& Texture::ConstructFBTexture(RenderDevice& deviceIn, uint32 width, uint32 height, PixelFormat pixelFormat, PixelFormat internalPixelFormat, SamplerData samplerData, int sampleCount, bool useBorder, Color borderColor)
	{
		// Frame buffer texture.

		renderDevice = &deviceIn;
		m_Sampler.Construct(deviceIn, samplerData.minFilter, samplerData.maxFilter, samplerData.wrapS, samplerData.wrapT, samplerData.anisotropy);
		m_ID = renderDevice->CreateTexture2D(width, height, NULL, pixelFormat, internalPixelFormat, false, false, samplerData.minFilter, samplerData.maxFilter, samplerData.wrapS, samplerData.wrapT, sampleCount, false, useBorder, borderColor);
		m_Width = (uint32)width;
		m_Height = (uint32)height;
		isCompressed = false;
		hasMipMaps = false;
		return *this;
	}

	Texture& Texture::ConstructEmpty(RenderDevice& deviceIn, SamplerData samplerData)
	{
		renderDevice = &deviceIn;
	
		data = new unsigned char[4]{ 1 & 0x000000ff, (1 & 0x0000ff00) >> 8,(1 & 0x00ff0000) >> 16,(1 & 0xff000000) >> 24 };

		m_Sampler.Construct(deviceIn, samplerData.minFilter, samplerData.maxFilter, samplerData.wrapS, samplerData.wrapT, samplerData.anisotropy);
		m_ID = renderDevice->CreateTexture2D(1, 1, data, PixelFormat::FORMAT_RGBA, PixelFormat::FORMAT_RGBA, false, false, samplerData.minFilter, samplerData.maxFilter, samplerData.wrapS, samplerData.wrapT, 0, true);
		m_Width = (uint32)1;
		m_Height = (uint32)1;
		isCompressed = false;
		hasMipMaps = false;
		dataAssigned = true;
		return *this;
	}


}

