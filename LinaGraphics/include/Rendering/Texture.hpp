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

#include "Core/LinaArray.hpp"
#include "Rendering/RenderingCommon.hpp"
#include "Core/Common.hpp"
#include "PackageManager/OpenGL/GLRenderDevice.hpp"
#include "Sampler.hpp"
#include "Core/SizeDefinitions.hpp"

namespace LinaEngine::Graphics
{
	class ArrayBitmap;
	class DDSTexture;

	class Texture
	{

	public:

		Texture() {};
		~Texture();

		Texture& Construct(RenderDevice& deviceIn, const class ArrayBitmap& data, SamplerParameters samplerParams, bool shouldCompress, const std::string& path = "");
		Texture& ConstructCubemap(RenderDevice& deviceIn, SamplerParameters samplerParams, const LinaArray<class ArrayBitmap*>& data, bool compress, const std::string& path = "");
		Texture& ConstructHDRI(RenderDevice& deviceIn, SamplerParameters samplerParams, Vector2 size, float* data, const std::string& path = "");
		Texture& ConstructRTCubemapTexture(RenderDevice& deviceIn, Vector2 size, SamplerParameters samplerParams, const std::string& path = "");
		Texture& ConstructRTTexture(RenderDevice& deviceIn, Vector2 size, SamplerParameters samplerParams, bool useBorder = false, const std::string& path = "");
		Texture& ConstructRTTextureMSAA(RenderDevice& deviceIn, Vector2 size, SamplerParameters samplerParams, int sampleCount, const std::string& path = "");
		Texture& ConstructEmpty(RenderDevice& deviceIn, SamplerParameters samplerParams = SamplerParameters(), const std::string& path = "");
		FORCEINLINE uint32 GetID() const { return m_ID; };
		FORCEINLINE uint32 GetSamplerID() const { return m_Sampler.GetID(); }
		FORCEINLINE Sampler& GetSampler() { return m_Sampler; }
		FORCEINLINE bool IsCompressed() const { return isCompressed; }
		FORCEINLINE bool HasMipmaps() const { return hasMipMaps; }
		FORCEINLINE Vector2 GetSize() { return m_Size; }
		FORCEINLINE bool GetIsEmpty() { return m_IsEmpty; }
		FORCEINLINE const std::string& GetPath() const { return m_path; }

	private:

		TextureBindMode m_BindMode;
		Sampler m_Sampler;
		RenderDevice* renderDevice = nullptr;
		uint32 m_ID = 0;
		Vector2 m_Size;
		bool isCompressed = false;
		bool hasMipMaps = true;
		bool m_IsEmpty = true;
		std::string m_path = "";
	};
}


#endif