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

		Texture& Construct(RenderDevice& deviceIn, const class ArrayBitmap& data, SamplerParameters samplerParams, bool shouldCompress);
		Texture& ConstructCubemap(RenderDevice& deviceIn, SamplerParameters samplerParams, const LinaArray<class ArrayBitmap*>& data, bool compress);
		Texture& ConstructRTTexture(RenderDevice& deviceIn, Vector2 size, SamplerParameters samplerParams, bool useBorder = false);
		Texture& ConstructRTTextureMSAA(RenderDevice& deviceIn, Vector2 size, SamplerParameters samplerParams, int sampleCount);
		Texture& ConstructEmpty(RenderDevice& deviceIn, SamplerParameters samplerParams = SamplerParameters());
		FORCEINLINE uint32 GetID() const { return m_ID; };
		FORCEINLINE uint32 GetSamplerID() const { return m_Sampler.GetID(); }
		FORCEINLINE bool IsCompressed() const { return isCompressed; }
		FORCEINLINE bool HasMipmaps() const { return hasMipMaps; }
		FORCEINLINE Vector2 GetSize() { return m_Size; }
	private:

		Sampler m_Sampler;
		RenderDevice* renderDevice = nullptr;
		uint32 m_ID = 0;
		Vector2 m_Size;
		unsigned char* data;
		bool dataAssigned = false;
		bool isCompressed = false;
		bool hasMipMaps = true;
	};
}


#endif