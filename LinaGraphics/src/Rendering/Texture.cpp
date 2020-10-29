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
#include "Rendering/RenderEngine.hpp"
#include <stdio.h>
#include <cereal/archives/binary.hpp>
#include <fstream>

namespace LinaEngine::Graphics
{
	std::map<int, Texture*> Texture::s_loadedTextures;

	Texture::~Texture()
	{
		m_id = s_renderDevice->ReleaseTexture2D(m_id);

	}

	Texture& Texture::Construct(RenderDevice& deviceIn, const ArrayBitmap& data, SamplerParameters samplerParams, bool shouldCompress, const std::string& path)
	{
		s_renderDevice = &deviceIn;
		m_size = Vector2(data.GetWidth(), data.GetHeight());
		m_bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D;
		m_sampler.Construct(deviceIn, samplerParams, m_bindMode);
		m_id = s_renderDevice->CreateTexture2D(m_size, data.GetPixelArray(), samplerParams, shouldCompress, false, Color::White);
		m_sampler.SetTargetTextureID(m_id);
		m_isCompressed = shouldCompress;
		m_hasMipMaps = samplerParams.m_textureParams.m_generateMipMaps;
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

		s_renderDevice = &deviceIn;
		m_size = Vector2(data[0]->GetWidth(), data[0]->GetHeight());
		m_bindMode = TextureBindMode::BINDTEXTURE_CUBEMAP;

		std::vector<int32*> cubeMapData;

		for (uint32 i = 0; i < 6; i++)
			cubeMapData.push_back(data[i]->GetPixelArray());

		m_sampler.Construct(deviceIn, samplerParams, m_bindMode);
		m_id = s_renderDevice->CreateCubemapTexture(m_size, samplerParams, cubeMapData, 6U);
		m_sampler.SetTargetTextureID(m_id);
		m_isCompressed = shouldCompress;
		m_hasMipMaps = samplerParams.m_textureParams.m_generateMipMaps;
		m_isEmpty = false;
		m_path = path;
		cubeMapData.clear();
		return *this;
	}

	Texture& Texture::ConstructHDRI(RenderDevice& deviceIn, SamplerParameters samplerParams, Vector2 size, float* data, const std::string& path)
	{
		s_renderDevice = &deviceIn;
		m_size = size;
		m_bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D;
		m_sampler.Construct(deviceIn, samplerParams, m_bindMode);
		m_id = s_renderDevice->CreateTextureHDRI(size, data, samplerParams);
		m_sampler.SetTargetTextureID(m_id);
		m_isCompressed = false;
		m_hasMipMaps = samplerParams.m_textureParams.m_generateMipMaps;
		m_isEmpty = false;
		m_path = path;
		return *this;
	}

	Texture& Texture::ConstructRTCubemapTexture(RenderDevice& deviceIn,  Vector2 size, SamplerParameters samplerParams, const std::string& path)
	{
		s_renderDevice = &deviceIn;
		m_size = size;
		m_bindMode = TextureBindMode::BINDTEXTURE_CUBEMAP;
		m_sampler.Construct(deviceIn,samplerParams, m_bindMode);
		m_id = s_renderDevice->CreateCubemapTextureEmpty(m_size, samplerParams);
		m_sampler.SetTargetTextureID(m_id);
		m_isCompressed = false;
		m_isEmpty = false;
		m_hasMipMaps = samplerParams.m_textureParams.m_generateMipMaps;
		m_path = path;
		return *this;
	}

	Texture& Texture::ConstructRTTexture(RenderDevice& deviceIn, Vector2 size, SamplerParameters samplerParams, bool useBorder, const std::string& path)
	{
		// Frame buffer texture.

		s_renderDevice = &deviceIn;
		SamplerParameters params;
		m_bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D;
		m_sampler.Construct(deviceIn, params, m_bindMode);
		m_id = s_renderDevice->CreateTexture2D(size, NULL, samplerParams, false, useBorder, Color::White);
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
		s_renderDevice = &deviceIn;
		SamplerParameters params;
		m_bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D_MULTISAMPLE;
		m_sampler.Construct(deviceIn, params, m_bindMode);
		m_id = s_renderDevice->CreateTexture2DMSAA(size, samplerParams, sampleCount);
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
		s_renderDevice = &deviceIn;

		SamplerParameters params;

		m_bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D;
		m_sampler.Construct(deviceIn, params, m_bindMode);
		m_id = s_renderDevice->CreateTexture2DEmpty(Vector2::One, samplerParams);
		m_sampler.SetTargetTextureID(m_id);
		m_size = Vector2::One;
		m_isCompressed = false;
		m_hasMipMaps = false;
		m_isEmpty = true;
		m_path = path;
		return *this;
	}

	SamplerParameters Texture::LoadParameters(const std::string& path)
	{
		SamplerParameters params;

		std::ifstream stream(path);
		{
			cereal::BinaryInputArchive iarchive(stream);

			// Read the data into it.
			iarchive(params);
		}

		return params;
	}

	void Texture::SaveParameters(const std::string& path, SamplerParameters params)
	{
		std::ofstream stream(path);
		{
			cereal::BinaryOutputArchive oarchive(stream); // Create an output archive

			oarchive(params); // Write the data to the archive
		}
	}

	Texture& Texture::CreateTexture2D(const std::string& filePath, SamplerParameters samplerParams, bool compress, bool useDefaultFormats, const std::string& paramsPath)
	{
		// Create pixel data.
		ArrayBitmap* textureBitmap = new ArrayBitmap();

		int nrComponents = textureBitmap->Load(filePath);
		if (nrComponents == -1)
		{
			LINA_CORE_WARN("Texture with the path {0} doesn't exist, returning empty texture", filePath);
			delete textureBitmap;
			return RenderEngine::GetDefaultTexture();
		}

		if (useDefaultFormats)
		{
			if (nrComponents == 1)
				samplerParams.m_textureParams.m_internalPixelFormat = samplerParams.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_R;
			if (nrComponents == 2)
				samplerParams.m_textureParams.m_internalPixelFormat = samplerParams.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_RG;
			else if (nrComponents == 3)
				samplerParams.m_textureParams.m_internalPixelFormat = samplerParams.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_RGB;
			else if (nrComponents == 4)
				samplerParams.m_textureParams.m_internalPixelFormat = samplerParams.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_RGBA;

		}

		// Create texture & construct.
		Texture* texture = new Texture();
		texture->Construct(RenderEngine::GetRenderDevice(), *textureBitmap, samplerParams, compress, filePath);
		s_loadedTextures[texture->GetID()] = texture;
		texture->m_paramsPath = paramsPath;

		// Delete pixel data.
		delete textureBitmap;

		LINA_CORE_TRACE("Texture created. {0}", filePath);

		// Return
		return *s_loadedTextures[texture->GetID()];
	}


	Texture& Texture::CreateTextureHDRI(const std::string filePath)
	{
		// Create pixel data.
		int w, h, nrComponents;
		float* data = ArrayBitmap::LoadImmediateHDRI(filePath.c_str(), w, h, nrComponents);

		if (!data)
		{
			LINA_CORE_WARN("Texture with the path {0} doesn't exist, returning empty texture", filePath);
			return RenderEngine::GetDefaultTexture();
		}

		// Create texture & construct.
		SamplerParameters samplerParams;
		samplerParams.m_textureParams.m_wrapR = samplerParams.m_textureParams.m_wrapS = samplerParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;
		samplerParams.m_textureParams.m_minFilter = samplerParams.m_textureParams.m_magFilter = SamplerFilter::FILTER_LINEAR;
		samplerParams.m_textureParams.m_internalPixelFormat = PixelFormat::FORMAT_RGB16F;
		samplerParams.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_RGB;

		Texture* texture = new Texture();
		texture->ConstructHDRI(RenderEngine::GetRenderDevice(), samplerParams, Vector2(w, h), data, filePath);
		s_loadedTextures[texture->GetID()] = texture;

		// Return
		return *s_loadedTextures[texture->GetID()];
	}


	Texture& Texture::GetTexture(int id)
	{
		if (!TextureExists(id))
		{
			// Mesh not found.
			LINA_CORE_WARN("Texture with the id {0} was not found, returning un-constructed texture...", id);
			return Texture();
		}

		return *s_loadedTextures[id];
	}

	Texture& Texture::GetTexture(const std::string& path)
	{
		const auto it = std::find_if(s_loadedTextures.begin(), s_loadedTextures.end(), [path]
		(const auto& item) -> bool { return item.second->GetPath().compare(path) == 0; });

		if (it == s_loadedTextures.end())
		{
			// Mesh not found.
			LINA_CORE_WARN("Texture with the path {0} was not found, returning un-constructed texture...", path);
			return Texture();
		}

		return *it->second;
	}

	void Texture::UnloadTextureResource(int id)
	{
		if (!TextureExists(id))
		{
			LINA_CORE_WARN("Texture not found! Aborting... ");
			return;
		}

		delete s_loadedTextures[id];
		s_loadedTextures.erase(id);
	}

	void Texture::UnloadAll()
	{
		// Delete textures.
		for (std::map<int, Texture*>::iterator it = s_loadedTextures.begin(); it != s_loadedTextures.end(); it++)
			delete it->second;

		s_loadedTextures.clear();
	}

	bool Texture::TextureExists(int id)
	{
		if (id < 0) return false;
		return !(s_loadedTextures.find(id) == s_loadedTextures.end());
	}

	bool Texture::TextureExists(const std::string& path)
	{
		const auto it = std::find_if(s_loadedTextures.begin(), s_loadedTextures.end(), [path]
		(const auto& it) -> bool { 	return it.second->GetPath().compare(path) == 0; 	});
		return it != s_loadedTextures.end();
	}

}

