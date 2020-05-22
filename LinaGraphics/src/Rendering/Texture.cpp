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

	Texture& Texture::ConstructFBTexture(RenderDevice& deviceIn, Vector2 size, SamplerParameters samplerParams, bool useBorder)
	{
		// Frame buffer texture.

		renderDevice = &deviceIn;
		SamplerParameters params;
		m_Sampler.Construct(deviceIn, params);
		m_ID = renderDevice->CreateTexture2D(size.x, size.y, NULL, samplerParams.textureParams.pixelFormat, samplerParams.textureParams.internalPixelFormat, false, false, samplerParams.textureParams.minFilter, samplerParams.textureParams.magFilter, samplerParams.textureParams.wrapS, samplerParams.textureParams.wrapT, 0, false, useBorder, Color::White);
		m_Size = size;
		isCompressed = false;
		hasMipMaps = false;
		return *this;
	}

	Texture& Texture::ConstructRTTextureMSAA(RenderDevice& deviceIn, Vector2 size, SamplerParameters samplerParams, int sampleCount)
	{
		renderDevice = &deviceIn;
		SamplerParameters params;
		m_Sampler.Construct(deviceIn, params);
		m_ID = renderDevice->CreateTexture2DMSAA(size, samplerParams, sampleCount);
		m_Size = size;
		isCompressed = false;
		hasMipMaps = false;
		return *this;
	}

	Texture& Texture::ConstructEmpty(RenderDevice& deviceIn, SamplerParameters samplerParams)
	{
		renderDevice = &deviceIn;
	
		SamplerParameters params;

		m_Sampler.Construct(deviceIn, params);
		m_ID = renderDevice->CreateTexture2DEmpty(Vector2::One, samplerParams);
		m_Size = Vector2::One;
		isCompressed = false;
		hasMipMaps = false;
		dataAssigned = true;
		return *this;
	}


}

