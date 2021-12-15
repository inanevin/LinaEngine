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

/*
Class: Texture

Represents a texture creatd in the GPU backend, is responsible for delegating the creation of
all different types of textures, 2D, 3D, cubemap, HDRI etc.

Timestamp: 1/7/2019 1:55:47 PM
*/

#pragma once

#ifndef Texture_HPP
#define Texture_HPP

#include "Core/RenderBackendFwd.hpp"
#include "Rendering/RenderingCommon.hpp"
#include "Core/Common.hpp"
#include "Sampler.hpp"

namespace Lina::Graphics
{
	class ArrayBitmap;
	class DDSTexture;

	class Texture
	{

	public:

		Texture() {};
		~Texture();

		Texture& Construct( const class ArrayBitmap& data, SamplerParameters samplerParams, bool shouldCompress, const std::string& path = "");
		Texture& ConstructCubemap( SamplerParameters samplerParams, const std::vector<class ArrayBitmap*>& data, bool compress, const std::string& path = "");
		Texture& ConstructHDRI(SamplerParameters samplerParams, Vector2 size, float* data, const std::string& path = "");
		Texture& ConstructRTCubemapTexture(Vector2 size, SamplerParameters samplerParams, const std::string& path = "");
		Texture& ConstructRTTexture(Vector2 size, SamplerParameters samplerParams, bool useBorder = false, const std::string& path = "");
		Texture& ConstructRTTextureMSAA(Vector2 size, SamplerParameters samplerParams, int sampleCount, const std::string& path = "");
		Texture& ConstructEmpty( SamplerParameters samplerParams = SamplerParameters(), const std::string& path = "");

		uint32 GetID() const { return m_id; };
		StringIDType GetSID() const { return m_sid; }
		uint32 GetSamplerID() const { return m_sampler.GetID(); }
		Sampler& GetSampler() { return m_sampler; }
		bool IsCompressed() const { return m_isCompressed; }
		bool HasMipmaps() const { return m_hasMipMaps; }
		Vector2 GetSize() { return m_size; }
		bool GetIsEmpty() { return m_isEmpty; }
		const std::string& GetPath() const { return m_path; }
		const std::string& GetParamsPath() const { return m_paramsPath; }

		static SamplerParameters LoadParameters(const std::string& path);
		static SamplerParameters LoadParametersFromMemory(unsigned char* data, size_t dataSize);
		static void SaveParameters(const std::string& path, SamplerParameters params);
		static Texture& CreateTexture2D(const std::string& path, const std::string& paramsPath, unsigned char* data, size_t dataSize, SamplerParameters samplerParams = SamplerParameters(), bool compress = false, bool useDefaultFormats = false);
		static Texture& CreateTextureHDRI(const std::string& path, unsigned char* data, size_t dataSize);
		static Texture& CreateTexture2D(const std::string& filePath, SamplerParameters samplerParams = SamplerParameters(), bool compress = false, bool useDefaultFormats = false, const std::string& paramsPath = "");
		static Texture& CreateTextureHDRI(const std::string filePath);
		static Texture& GetTexture(StringIDType id);
		static Texture& GetTexture(const std::string& path);
		static bool TextureExists(StringIDType id);
		static bool TextureExists(const std::string& path);
		static void UnloadTextureResource(StringIDType id);
		static void UnloadAll();

	private:

		static std::map<StringIDType, Texture*> s_loadedTextures;

		friend RenderEngine;

		TextureBindMode m_bindMode;
		Sampler m_sampler;
		RenderDevice* m_renderDevice = nullptr;
		uint32 m_id = 0;
		StringIDType m_sid = 0;
		Vector2 m_size = Vector2::One;
		bool m_isCompressed = false;
		bool m_hasMipMaps = true;
		bool m_isEmpty = true;
		std::string m_path = "";
		std::string m_paramsPath = "";

	};
}


#endif