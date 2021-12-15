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
#include "Core/RenderEngineBackend.hpp"
#include <stdio.h>
#include <cereal/archives/portable_binary.hpp>
#include <fstream>

namespace Lina::Graphics
{
	std::map<StringIDType, Texture*> Texture::s_loadedTextures;

	Texture::~Texture()
	{
		m_id = m_renderDevice->ReleaseTexture2D(m_id);
	}

	Texture& Texture::Construct(const ArrayBitmap& data, SamplerParameters samplerParams, bool shouldCompress, const std::string& path)
	{
		m_renderDevice = RenderEngineBackend::Get()->GetRenderDevice();
		m_size = Vector2(data.GetWidth(), data.GetHeight());
		m_bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D;
		m_sampler.Construct(samplerParams, m_bindMode);
		m_id = m_renderDevice->CreateTexture2D(m_size, data.GetPixelArray(), samplerParams, shouldCompress, false, Color::White);
		m_sampler.SetTargetTextureID(m_id);
		m_isCompressed = shouldCompress;
		m_hasMipMaps = samplerParams.m_textureParams.m_generateMipMaps;
		m_isEmpty = false;
		m_path = path;
		return *this;
	}


	Texture& Texture::ConstructCubemap(SamplerParameters samplerParams, const std::vector<ArrayBitmap*>& data, bool shouldCompress, const std::string& path)
	{
		if (data.size() != 6)
		{
			LINA_WARN("Could not construct cubemap texture! ArrayBitmap data size needs to be 6, returning un-constructed texture...");
			return Texture();
		}

		m_renderDevice = RenderEngineBackend::Get()->GetRenderDevice();
		m_size = Vector2(data[0]->GetWidth(), data[0]->GetHeight());
		m_bindMode = TextureBindMode::BINDTEXTURE_CUBEMAP;

		std::vector<unsigned char*> cubeMapData;

		for (uint32 i = 0; i < 6; i++)
			cubeMapData.push_back(data[i]->GetPixelArray());

		m_sampler.Construct(samplerParams, m_bindMode);
		m_id = m_renderDevice->CreateCubemapTexture(m_size, samplerParams, cubeMapData, 6U);
		m_sampler.SetTargetTextureID(m_id);
		m_isCompressed = shouldCompress;
		m_hasMipMaps = samplerParams.m_textureParams.m_generateMipMaps;
		m_isEmpty = false;
		m_path = path;
		cubeMapData.clear();
		return *this;
	}

	Texture& Texture::ConstructHDRI(SamplerParameters samplerParams, Vector2 size, float* data, const std::string& path)
	{
		m_renderDevice = RenderEngineBackend::Get()->GetRenderDevice();
		m_size = size;
		m_bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D;
		m_sampler.Construct( samplerParams, m_bindMode);
		m_id = m_renderDevice->CreateTextureHDRI(size, data, samplerParams);
		m_sampler.SetTargetTextureID(m_id);
		m_isCompressed = false;
		m_hasMipMaps = samplerParams.m_textureParams.m_generateMipMaps;
		m_isEmpty = false;
		m_path = path;
		return *this;
	}

	Texture& Texture::ConstructRTCubemapTexture(Vector2 size, SamplerParameters samplerParams, const std::string& path)
	{
		m_renderDevice = RenderEngineBackend::Get()->GetRenderDevice();
		m_size = size;
		m_bindMode = TextureBindMode::BINDTEXTURE_CUBEMAP;
		m_sampler.Construct(samplerParams, m_bindMode, true);
		m_id = m_renderDevice->CreateCubemapTextureEmpty(m_size, samplerParams);
		m_sampler.SetTargetTextureID(m_id);
		m_isCompressed = false;
		m_isEmpty = false;
		m_hasMipMaps = samplerParams.m_textureParams.m_generateMipMaps;
		m_path = path;
		return *this;
	}

	Texture& Texture::ConstructRTTexture(Vector2 size, SamplerParameters samplerParams, bool useBorder, const std::string& path)
	{
		// Frame buffer texture.

		m_renderDevice = RenderEngineBackend::Get()->GetRenderDevice();
		SamplerParameters params;
		m_bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D;
		m_sampler.Construct( params, m_bindMode);
		m_id = m_renderDevice->CreateTexture2D(size, NULL, samplerParams, false, useBorder, Color::White);
		m_sampler.SetTargetTextureID(m_id);
		m_size = size;
		m_isCompressed = false;
		m_isEmpty = false;
		m_hasMipMaps = false;
		m_path = path;
		return *this;
	}

	Texture& Texture::ConstructRTTextureMSAA(Vector2 size, SamplerParameters samplerParams, int sampleCount, const std::string& path)
	{
		m_renderDevice = RenderEngineBackend::Get()->GetRenderDevice();
		SamplerParameters params;
		m_bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D_MULTISAMPLE;
	//	m_sampler.Construct(deviceIn, params, m_bindMode);
		m_id = m_renderDevice->CreateTexture2DMSAA(size, samplerParams, sampleCount);
	//	m_sampler.SetTargetTextureID(m_id);
		m_size = size;
		m_isCompressed = false;
		m_isEmpty = false;
		m_hasMipMaps = false;
		m_path = path;
		return *this;
	}

	Texture& Texture::ConstructEmpty(SamplerParameters samplerParams, const std::string& path)
	{
		m_renderDevice = RenderEngineBackend::Get()->GetRenderDevice();

		SamplerParameters params;

		m_bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D;
		m_sampler.Construct( params, m_bindMode);
		m_id = m_renderDevice->CreateTexture2DEmpty(Vector2::One, samplerParams);
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

		std::ifstream stream(path, std::ios::binary);
		{
			cereal::PortableBinaryInputArchive iarchive(stream);
			iarchive(params);
		}

		return params;
	}

	SamplerParameters Texture::LoadParametersFromMemory(unsigned char* data, size_t dataSize)
	{
		SamplerParameters params;

		{
			std::string data((char*)data, dataSize);
			std::istringstream stream(data, std::ios::binary);
			{
				cereal::PortableBinaryInputArchive iarchive(stream);
				iarchive(params);
			}
		}

		return params;
	}

	void Texture::SaveParameters(const std::string& path, SamplerParameters params)
	{
		std::ofstream stream(path, std::ios::binary);
		{
			cereal::PortableBinaryOutputArchive oarchive(stream);
			oarchive(params);
		}
	}

	Texture& Texture::CreateTexture2D(const std::string& path, const std::string& paramsPath, unsigned char* data, size_t dataSize, SamplerParameters samplerParams, bool compress, bool useDefaultFormats)
	{
		// Build pixel data.
		ArrayBitmap* textureBitmap = new ArrayBitmap();

		int nrComponents = textureBitmap->Load(data, dataSize);
		
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

		StringIDType sid = StringID(path.c_str()).value();

		// Build texture & construct.
		Texture* texture = new Texture();
		texture->Construct(*textureBitmap, samplerParams, compress, path);
		texture->m_sid = sid;
		texture->m_path = path;
		texture->m_paramsPath = paramsPath;
		s_loadedTextures[sid] = texture;

		// Delete pixel data.
		delete textureBitmap;

		// Return
		return *s_loadedTextures[sid];
	}

	Texture& Texture::CreateTextureHDRI(const std::string& path, unsigned char* data, size_t dataSize)
	{
		// Build pixel data.
		int w, h, nrComponents;
		float* bitmapData = ArrayBitmap::LoadImmediateHDRI(data, dataSize, w, h, nrComponents);

		LINA_ASSERT(bitmapData != nullptr, "HDR texture could not be loaded!");

		// Build texture & construct.
		SamplerParameters samplerParams;
		samplerParams.m_textureParams.m_wrapR = samplerParams.m_textureParams.m_wrapS = samplerParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;
		samplerParams.m_textureParams.m_minFilter = samplerParams.m_textureParams.m_magFilter = SamplerFilter::FILTER_LINEAR;
		samplerParams.m_textureParams.m_internalPixelFormat = PixelFormat::FORMAT_RGB16F;
		samplerParams.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_RGB;

		StringIDType sid = StringID(path.c_str()).value();
		Texture* texture = new Texture();
		texture->ConstructHDRI(samplerParams, Vector2(w, h), bitmapData, path);
		texture->m_sid = sid;
		s_loadedTextures[sid] = texture;

		// Return
		return *s_loadedTextures[sid];
	}

	Texture& Texture::CreateTexture2D(const std::string& filePath, SamplerParameters samplerParams, bool compress, bool useDefaultFormats, const std::string& paramsPath)
	{
		// Build pixel data.
		ArrayBitmap* textureBitmap = new ArrayBitmap();

		int nrComponents = textureBitmap->Load(filePath);
		if (nrComponents == -1)
		{
			LINA_WARN("Texture with the path {0} doesn't exist, returning empty texture", filePath);
			delete textureBitmap;
			return RenderEngineBackend::GetDefaultTexture();
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

		StringIDType sid = StringID(filePath.c_str()).value();

		// Build texture & construct.
		Texture* texture = new Texture();
		texture->Construct(*textureBitmap, samplerParams, compress, filePath);
		texture->m_paramsPath = paramsPath;
		texture->m_sid = sid;
		s_loadedTextures[sid] = texture;

		// Delete pixel data.
		delete textureBitmap;

		// Return
		return *s_loadedTextures[sid];
	}


	Texture& Texture::CreateTextureHDRI(const std::string filePath)
	{
		// Build pixel data.
		int w, h, nrComponents;
		float* data = ArrayBitmap::LoadImmediateHDRI(filePath.c_str(), w, h, nrComponents);

		if (!data)
		{
			LINA_WARN("Texture with the path {0} doesn't exist, returning empty texture", filePath);
			return RenderEngineBackend::GetDefaultTexture();
		}

		// Build texture & construct.
		SamplerParameters samplerParams;
		samplerParams.m_textureParams.m_wrapR = samplerParams.m_textureParams.m_wrapS = samplerParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;
		samplerParams.m_textureParams.m_minFilter = samplerParams.m_textureParams.m_magFilter = SamplerFilter::FILTER_LINEAR;
		samplerParams.m_textureParams.m_internalPixelFormat = PixelFormat::FORMAT_RGB16F;
		samplerParams.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_RGB;

		StringIDType sid = StringID(filePath.c_str()).value();
		Texture* texture = new Texture();
		texture->ConstructHDRI(samplerParams, Vector2(w, h), data, filePath);
		texture->m_sid = sid;
		s_loadedTextures[sid] = texture;

		// Return
		return *s_loadedTextures[sid];
	}


	Texture& Texture::GetTexture(StringIDType id)
	{
		bool texture = TextureExists(id);
		LINA_ASSERT(texture, "Texture does not exist!");
		return *s_loadedTextures[id];
	}

	Texture& Texture::GetTexture(const std::string& path)
	{
		return GetTexture(StringID(path.c_str()).value());
	}

	void Texture::UnloadTextureResource(StringIDType id)
	{
		if (!TextureExists(id))
		{
			LINA_WARN("Texture not found! Aborting... ");
			return;
		}

		delete s_loadedTextures[id];
		s_loadedTextures.erase(id);
	}

	void Texture::UnloadAll()
	{
		// Delete textures.
		for (std::map<StringIDType, Texture*>::iterator it = s_loadedTextures.begin(); it != s_loadedTextures.end(); it++)
			delete it->second;

		s_loadedTextures.clear();
	}

	bool Texture::TextureExists(StringIDType id)
	{
		if (id < 0) return false;
		return !(s_loadedTextures.find(id) == s_loadedTextures.end());
	}

	bool Texture::TextureExists(const std::string& path)
	{
		return TextureExists(StringID(path.c_str()).value());
	}

}

