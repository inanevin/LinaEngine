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

	}

	Texture& Texture::Construct(RenderDevice& deviceIn, const ArrayBitmap& data, SamplerParameters samplerParams, bool shouldCompress, const std::string& path)
	{
		renderDevice = &deviceIn;
		m_Size = Vector2(data.GetWidth(), data.GetHeight());
		m_BindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D;
		m_Sampler.Construct(deviceIn, samplerParams, m_BindMode);
		m_ID = renderDevice->CreateTexture2D(m_Size, data.GetPixelArray(), samplerParams, shouldCompress, false, Color::White);
		m_Sampler.SetTargetTextureID(m_ID);
		isCompressed = shouldCompress;
		hasMipMaps = samplerParams.textureParams.generateMipMaps;
		m_IsEmpty = false;
		m_path = path;
		return *this;
	}


	Texture& Texture::ConstructCubemap(RenderDevice& deviceIn, SamplerParameters samplerParams, const LinaArray<ArrayBitmap*>& data, bool shouldCompress, const std::string& path)
	{
		if (data.size() != 6)
		{
			LINA_CORE_WARN("Could not construct cubemap texture! ArrayBitmap data size needs to be 6, returning un-constructed texture...");
			return Texture();
		}

		renderDevice = &deviceIn;
		m_Size = Vector2(data[0]->GetWidth(), data[0]->GetHeight());
		m_BindMode = TextureBindMode::BINDTEXTURE_CUBEMAP;

		LinaArray<int32*> cubeMapData;

		for (uint32 i = 0; i < 6; i++)
			cubeMapData.push_back(data[i]->GetPixelArray());

		m_Sampler.Construct(deviceIn, samplerParams, m_BindMode);
		m_ID = renderDevice->CreateCubemapTexture(m_Size, samplerParams, cubeMapData, 6U);
		m_Sampler.SetTargetTextureID(m_ID);
		isCompressed = shouldCompress;
		hasMipMaps = samplerParams.textureParams.generateMipMaps;
		m_IsEmpty = false;
		m_path = path;
		cubeMapData.clear();
		return *this;
	}

	Texture& Texture::ConstructHDRI(RenderDevice& deviceIn, SamplerParameters samplerParams, Vector2 size, float* data, const std::string& path)
	{
		renderDevice = &deviceIn;
		m_Size = size;
		m_BindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D;
		m_Sampler.Construct(deviceIn, samplerParams, m_BindMode);
		m_ID = renderDevice->CreateTextureHDRI(size, data, samplerParams);
		m_Sampler.SetTargetTextureID(m_ID);
		isCompressed = false;
		hasMipMaps = samplerParams.textureParams.generateMipMaps;
		m_IsEmpty = false;
		m_path = path;
		return *this;
	}

	Texture& Texture::ConstructRTCubemapTexture(RenderDevice& deviceIn,  Vector2 size, SamplerParameters samplerParams, const std::string& path)
	{
		renderDevice = &deviceIn;
		m_Size = size;
		m_BindMode = TextureBindMode::BINDTEXTURE_CUBEMAP;
		m_Sampler.Construct(deviceIn,samplerParams, m_BindMode);
		m_ID = renderDevice->CreateCubemapTextureEmpty(m_Size, samplerParams);
		m_Sampler.SetTargetTextureID(m_ID);
		isCompressed = false;
		m_IsEmpty = false;
		hasMipMaps = samplerParams.textureParams.generateMipMaps;
		m_path = path;
		return *this;
	}

	Texture& Texture::ConstructRTTexture(RenderDevice& deviceIn, Vector2 size, SamplerParameters samplerParams, bool useBorder, const std::string& path)
	{
		// Frame buffer texture.

		renderDevice = &deviceIn;
		SamplerParameters params;
		m_BindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D;
		m_Sampler.Construct(deviceIn, params, m_BindMode);
		m_ID = renderDevice->CreateTexture2D(size, NULL, samplerParams, false, useBorder, Color::White);
		m_Sampler.SetTargetTextureID(m_ID);
		m_Size = size;
		isCompressed = false;
		m_IsEmpty = false;
		hasMipMaps = false;
		m_path = path;
		return *this;
	}

	Texture& Texture::ConstructRTTextureMSAA(RenderDevice& deviceIn, Vector2 size, SamplerParameters samplerParams, int sampleCount, const std::string& path)
	{
		renderDevice = &deviceIn;
		SamplerParameters params;
		m_BindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D_MULTISAMPLE;
		m_Sampler.Construct(deviceIn, params, m_BindMode);
		m_ID = renderDevice->CreateTexture2DMSAA(size, samplerParams, sampleCount);
		m_Sampler.SetTargetTextureID(m_ID);
		m_Size = size;
		isCompressed = false;
		m_IsEmpty = false;
		hasMipMaps = false;
		m_path = path;
		return *this;
	}

	Texture& Texture::ConstructEmpty(RenderDevice& deviceIn, SamplerParameters samplerParams, const std::string& path)
	{
		renderDevice = &deviceIn;

		SamplerParameters params;

		m_BindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D;
		m_Sampler.Construct(deviceIn, params, m_BindMode);
		m_ID = renderDevice->CreateTexture2DEmpty(Vector2::One, samplerParams);
		m_Sampler.SetTargetTextureID(m_ID);
		m_Size = Vector2::One;
		isCompressed = false;
		hasMipMaps = false;
		m_IsEmpty = true;
		m_path = path;
		return *this;
	}


}

