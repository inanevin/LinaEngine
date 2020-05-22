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

namespace LinaEngine::Graphics
{
	Texture::~Texture()
	{
		m_ID = renderDevice->ReleaseTexture2D(m_ID);

		if (dataAssigned)
			delete data;
	}

	Texture& Texture::Construct(RenderDevice& deviceIn, const ArrayBitmap& data, SamplerParameters samplerParams, bool shouldCompress)
	{
		renderDevice = &deviceIn;
		
		m_Sampler.Construct(deviceIn, samplerParams);
		m_ID = renderDevice->CreateTexture2D(data.GetWidth(), data.GetHeight(), data.GetPixelArray(), samplerParams.textureParams.pixelFormat, samplerParams.textureParams.internalPixelFormat, samplerParams.textureParams.generateMipMaps, shouldCompress, samplerParams.textureParams.minFilter, samplerParams.textureParams.magFilter, samplerParams.textureParams.wrapS, samplerParams.textureParams.wrapT);
		m_Width = (uint32)data.GetWidth();
		m_Height = (uint32)data.GetHeight();
		isCompressed = shouldCompress;
		hasMipMaps = samplerParams.textureParams.generateMipMaps;
		return *this;
	}


	Texture& Texture::Construct(RenderDevice& deviceIn, const LinaArray<ArrayBitmap*>& data, SamplerParameters samplerParams, bool shouldCompress)
	{
		if (data.size() != 6)
		{
			LINA_CORE_WARN("Could not construct cubemap texture! ArrayBitmap data size needs to be 6, returning un-constructed texture...");
			return Texture();
		}
		SamplerParameters params;

		m_Sampler.Construct(deviceIn, params);
		renderDevice = &deviceIn;
		m_Width = (uint32)data[0]->GetWidth();
		m_Height = (uint32)data[0]->GetHeight();

		LinaArray<int32*> cubeMapData;

		for (uint32 i = 0; i < 6; i++)
			cubeMapData.push_back(data[i]->GetPixelArray());

		m_ID = renderDevice->CreateCubemapTexture(m_Width, m_Height, cubeMapData, 6U, samplerParams.textureParams.pixelFormat, samplerParams.textureParams.internalPixelFormat, samplerParams.textureParams.generateMipMaps);
		isCompressed = shouldCompress;
		hasMipMaps = samplerParams.textureParams.generateMipMaps;

		cubeMapData.clear();
		return *this;
	}

	Texture& Texture::ConstructFBTexture(RenderDevice& deviceIn, uint32 width, uint32 height, PixelFormat pixelFormat, PixelFormat internalPixelFormat, SamplerData samplerData, int sampleCount, bool useBorder, Color borderColor)
	{
		// Frame buffer texture.

		renderDevice = &deviceIn;
		SamplerParameters params;

		m_Sampler.Construct(deviceIn, params);
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
	
		SamplerParameters params;

		m_Sampler.Construct(deviceIn, params);
		m_ID = renderDevice->CreateTexture2D(1, 1, data, PixelFormat::FORMAT_RGBA, PixelFormat::FORMAT_RGBA, false, false, samplerData.minFilter, samplerData.maxFilter, samplerData.wrapS, samplerData.wrapT, 0, true);
		m_Width = (uint32)1;
		m_Height = (uint32)1;
		isCompressed = false;
		hasMipMaps = false;
		dataAssigned = true;
		return *this;
	}


}

