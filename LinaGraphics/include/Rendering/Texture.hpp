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