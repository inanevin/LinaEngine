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

#include "Rendering/Texture.hpp"  
#include "Rendering/ArrayBitmap.hpp"

namespace LinaEngine::Graphics
{
	Texture::~Texture()
	{
		m_id = m_renderDevice->ReleaseTexture2D(m_id);

	}

	Texture& Texture::Construct(RenderDevice& deviceIn, const ArrayBitmap& data, SamplerParameters samplerParams, bool shouldCompress, const std::string& path)
	{
		m_renderDevice = &deviceIn;
		m_size = Vector2(data.GetWidth(), data.GetHeight());
		m_bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D;
		m_sampler.Construct(deviceIn, samplerParams, m_bindMode);
		m_id = m_renderDevice->CreateTexture2D(m_size, data.GetPixelArray(), samplerParams, shouldCompress, false, Color::White);
		m_sampler.SetTargetTextureID(m_id);
		m_isCompressed = shouldCompress;
		m_hasMipMaps = samplerParams.textureParams.generateMipMaps;
		m_isEmpty = false;
		m_path = path;
		return *this;
	}


	Texture& Texture::ConstructCubemap(RenderDevice& deviceIn, SamplerParameters samplerParams, const std::vector<ArrayBitmap*>& data, bool shouldCompress, const std::string& path)
	{
		if (data.size() != 6)
		{
			LINA_CORE_WARN("Could not construct cubemap texture! ArrayBitmap data size needs to be 6, returning un-constructed texture...");
			return Texture();
		}

		m_renderDevice = &deviceIn;
		m_size = Vector2(data[0]->GetWidth(), data[0]->GetHeight());
		m_bindMode = TextureBindMode::BINDTEXTURE_CUBEMAP;

		std::vector<int32*> cubeMapData;

		for (uint32 i = 0; i < 6; i++)
			cubeMapData.push_back(data[i]->GetPixelArray());

		m_sampler.Construct(deviceIn, samplerParams, m_bindMode);
		m_id = m_renderDevice->CreateCubemapTexture(m_size, samplerParams, cubeMapData, 6U);
		m_sampler.SetTargetTextureID(m_id);
		m_isCompressed = shouldCompress;
		m_hasMipMaps = samplerParams.textureParams.generateMipMaps;
		m_isEmpty = false;
		m_path = path;
		cubeMapData.clear();
		return *this;
	}

	Texture& Texture::ConstructHDRI(RenderDevice& deviceIn, SamplerParameters samplerParams, Vector2 size, float* data, const std::string& path)
	{
		m_renderDevice = &deviceIn;
		m_size = size;
		m_bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D;
		m_sampler.Construct(deviceIn, samplerParams, m_bindMode);
		m_id = m_renderDevice->CreateTextureHDRI(size, data, samplerParams);
		m_sampler.SetTargetTextureID(m_id);
		m_isCompressed = false;
		m_hasMipMaps = samplerParams.textureParams.generateMipMaps;
		m_isEmpty = false;
		m_path = path;
		return *this;
	}

	Texture& Texture::ConstructRTCubemapTexture(RenderDevice& deviceIn,  Vector2 size, SamplerParameters samplerParams, const std::string& path)
	{
		m_renderDevice = &deviceIn;
		m_size = size;
		m_bindMode = TextureBindMode::BINDTEXTURE_CUBEMAP;
		m_sampler.Construct(deviceIn,samplerParams, m_bindMode);
		m_id = m_renderDevice->CreateCubemapTextureEmpty(m_size, samplerParams);
		m_sampler.SetTargetTextureID(m_id);
		m_isCompressed = false;
		m_isEmpty = false;
		m_hasMipMaps = samplerParams.textureParams.generateMipMaps;
		m_path = path;
		return *this;
	}

	Texture& Texture::ConstructRTTexture(RenderDevice& deviceIn, Vector2 size, SamplerParameters samplerParams, bool useBorder, const std::string& path)
	{
		// Frame buffer texture.

		m_renderDevice = &deviceIn;
		SamplerParameters params;
		m_bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D;
		m_sampler.Construct(deviceIn, params, m_bindMode);
		m_id = m_renderDevice->CreateTexture2D(size, NULL, samplerParams, false, useBorder, Color::White);
		m_sampler.SetTargetTextureID(m_id);
		m_size = size;
		m_isCompressed = false;
		m_isEmpty = false;
		m_hasMipMaps = false;
		m_path = path;
		return *this;
	}

	Texture& Texture::ConstructRTTextureMSAA(RenderDevice& deviceIn, Vector2 size, SamplerParameters samplerParams, int sampleCount, const std::string& path)
	{
		m_renderDevice = &deviceIn;
		SamplerParameters params;
		m_bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D_MULTISAMPLE;
		m_sampler.Construct(deviceIn, params, m_bindMode);
		m_id = m_renderDevice->CreateTexture2DMSAA(size, samplerParams, sampleCount);
		m_sampler.SetTargetTextureID(m_id);
		m_size = size;
		m_isCompressed = false;
		m_isEmpty = false;
		m_hasMipMaps = false;
		m_path = path;
		return *this;
	}

	Texture& Texture::ConstructEmpty(RenderDevice& deviceIn, SamplerParameters samplerParams, const std::string& path)
	{
		m_renderDevice = &deviceIn;

		SamplerParameters params;

		m_bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D;
		m_sampler.Construct(deviceIn, params, m_bindMode);
		m_id = m_renderDevice->CreateTexture2DEmpty(Vector2::One, samplerParams);
		m_sampler.SetTargetTextureID(m_id);
		m_size = Vector2::One;
		m_isCompressed = false;
		m_hasMipMaps = false;
		m_isEmpty = true;
		m_path = path;
		return *this;
	}


}

